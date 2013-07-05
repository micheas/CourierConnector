<?php

//function courier_ubercart_get_product_mapping()
//{
//    $mapping = db_query("select * from {courier_product_mapping}");
//    
//    if(db_affected_rows($mapping) == 0)
//    {
//        //define the default mapping, and check the settings to see if there is new mapping defined
//        $item_mapping = array();
//        $item_mapping['title'] = 'Name';            
//        $item_mapping['body'] = 'SalesDescription';
//        $item_mapping['status'] = '{1}';
//        $item_mapping['promote'] = '{0}';
//        $item_mapping['sticky'] = '{0}';            
//        $item_mapping['sell_price'] = 'SalePrice';
//        $item_mapping['model'] = 'ManufacturerPartNumber';
//        $item_mapping['weight_units'] = '{lb}';
//        $item_mapping['weight'] = '{1}';
//        $item_mapping['height'] = '{1}';
//        $item_mapping['width'] = '{1}';
//        $item_mapping['height_units'] = '{in}';
//        $item_mapping['length_units'] = '{in}';
//        $item_mapping['length'] = '{1}';
//        $item_mapping['pkg_qty'] = '{1}';
//        $item_mapping['default_qty'] = '{1}';
//        $item_mapping['list_price'] = 'SalePrice';
//        $item_mapping['cost'] = '{0}';
//        $item_mapping['shippable'] = '{0}';   
//    }
//    else
//    {
//        $item_mapping = array();
//        
//        while($row = db_fetch_object($mapping))
//        {                        
//            $item_mapping[$row->uc_field] = $row;            
//        }
//    }
//    
//    return $item_mapping;
//}

function courier_connector_ubercart_process_product($objectType, $responseXml) {
  //write the error to some log if there was one
  if ($responseXml->ErrorMessage != '') {
    //handle error
  }

  $plural = $objectType . 's';
  $id = $objectType . 'ID';

  foreach ($responseXml->$plural->$objectType as $item) {
    //check the processed tag, to see what we need to do with this response
    //4 possibles
    //error
    //added
    //modded
    //read

    if ($item->Processed == "error") {
      //was already logged

    }
    else if ($item->Processed == "added") {
      //this is just a response that the record in qb was added

      //update the record locally to attach the qb id from this add

      //try and find a node that has this quickbooks id
      $itemid = db_result(db_query("select entity_id from {courier_product_relations} where relation_id = '%s'", $item->$id));

      db_query("replace into courier_product_relations values('%s', '%s', '%s')");

      //log the add
      db_query("insert into courier_transaction_log (type, message, status) values(1, 'Product Sent to QuickBooks, Product ID: %s', 'OK')", $node->nid);

    }
    else {
      if ($item->Processed == "modded") {
        //this is a response that the record has been modified, really nothing needs to be done
        //
        //check if there is a hard node realtion
        if ($item->RelatedID && $item->RelatedID != '') {
          $itemid = $item->RelatedID;
        }
        else {
          $itemid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'product' and field_quickbooks_id_value = :v", array(":v" => $item->$id))->fetchField();
        }

        //log the modification
        db_query("insert into courier_transaction_log (type, message, status)
                values(1, 'Product update sent to QuickBooks, Product ID: %s', 'OK')", $node->nid);

      }
      //these are the result of a query, we will update / insert these
      else {
        if ($item->Processed == "read") {
          $node = new stdClass();

          $item_mapping = courier_quickbooks_pos_get_mapped_field_list('product');

          try {
            //the items are grouped by the combo of the desc1 and department1
            //so we need to use those 2 to build find the nid for this product

            //get the item model
            $m = $item_mapping['model']->qb_field;
            $model = (string) $item->$m;

            //try and find a node that has this quickbooks id
            $nid = db_result(db_query("select nid from {courier_product_relations} where desc1 = '%s' and department_id = '%s'", $item->Desc1, $item->DepartmentID));

            //check if there is a hard node realtion
            if ($item->RelatedID && $item->RelatedID != '') {
              $node = node_load((string) $item->RelatedID);

              //log the read
              db_query("insert into courier_transaction_log (type, message, status)
                        values(1, 'Received " . $objectType . " from QuickBooks. Name: " . $item->Name . ", Updated nid {$nid}', 'OK')");

            }
            else {
              if ($nid != FALSE) {
                //load from the quickbooks related id
                $node = node_load($nid);

                //log the read
                db_query("insert into courier_transaction_log (type, message, status)
                        values(1, 'Received " . $objectType . " from QuickBooks. Name: " . $item->Name . ", Updated nid {$nid}.', 'OK')");

              }
//                else if($model_field != "Not_Mapped")
//                {
//                    //try and do an auto relation base on the sku value
//                    $nid = db_query("select nid from uc_products where model = :model", array(":model" => $item->$model_field))->fetchField();
//                    $node = node_load($nid);
//
//                    //log the read
//                    db_query("insert into courier_transaction_log (type, message, status)
//                        values(1, 'Received " . $objectType . " from QuickBooks. Name: " . $item->Name . ", Auto related to nid {$nid}.', 'OK')");
//
//                }
              else {
                $node->type = 'product';
                $node->is_new = TRUE;
                $node->created = time();
                $node->language = LANGUAGE_NONE;

                //log the read
                db_query("insert into courier_transaction_log (type, message, status)
                        values(1, 'Received " . $objectType . " from QuickBooks. Name: " . $item->Name . ", Adding to products.', 'OK')");

              }
            }

            foreach ($item_mapping as $map) {
              $qbfield = $map->qb_field;
              $ucfield = $map->uc_field;

              //set the value of the field if it is custom
              if ($item->$qbfield == "Custom") {
                //custom value
                $node->$ucfield = (string) $map->custom_value;
              }
              else {
                $node->$ucfield = (string) $item->$qbfield;
              }
            }

            node_save($node);

            //update our relations table with this record
            db_query("replace into courier_product_relations values('%s', '%s', '%s', '%s', '%s')", $item->$id, $node->nid, $model, $item->Desc1, $item->DepartmentID);

            //handle the attributes this item has
            $options = array();

            if ((string) $item->Size != '') {
              courier_ubercart_process_attribute($node->nid, 'Size', (string) $item->Size, $options);
            }

            if ((string) $item->Attribute != '') {
              courier_ubercart_process_attribute($node->nid, 'Attribute', (string) $item->Attribute, $options);
            }

            courier_ubercart_product_adjustment($node->nid, $model, $options);

            courier_ubercart_stock($node->nid, $model, $item->QuantityOnHand);
          } catch (Exception $ex) {
            $errorMessage = $ex->getMessage();

            print_r($errorMessage);
          }
        }
      }
    }
  }
}

//update the stock for this adjustment record
function courier_ubercart_stock($nid, $sku, $stock) {
  if ($sku != '' && $nid != '')
    db_query("replace into uc_product_stock(sku, nid, active, stock, threshold) values('%s', '%s', 1, '%s', 0)", $sku, $nid, $stock);
}


//create the product adjustment record for this combo
function courier_ubercart_product_adjustment($nid, $model, $options) {
  if (is_array($options) && count($options) > 0) {
    db_query("delete from uc_product_adjustments where nid = '%s' and model = '%s'", $nid, $model);

    db_query("insert into uc_product_adjustments values('%s', '%s', '%s')", $nid, serialize($options), $model);
  }
}

//handle the addition of this attribute / option to the product if nessiscary
function courier_ubercart_process_attribute($nid, $attribute, $option, &$options_list) {
  //check for the existance of this attribute
  $query = "SELECT aid FROM uc_attributes WHERE name = '" . htmlspecialchars($attribute, ENT_QUOTES, "UTF-8") . "'";
  $aid = db_result(db_query($query));

  if (!$aid) {
    $query = "INSERT INTO uc_attributes (name, required, display, description) VALUES('" . htmlspecialchars($attribute, ENT_QUOTES, "UTF-8") . "', 0, 1, '')";
    db_query($query);

    $aid = db_last_insert_id("uc_attributes", "aid");
  }

  //get the oid
  $query = "SELECT oid FROM uc_attribute_options WHERE name = '" . htmlspecialchars($option, ENT_QUOTES, "UTF-8") . "' AND aid = " . $aid;
  $oid = db_result(db_query($query));

  //if it is blank create one
  if (!$oid) {
    $query = "INSERT INTO uc_attribute_options (aid, name, cost, price, weight) VALUES({$aid}, '" . htmlspecialchars($option, ENT_QUOTES, "UTF-8") . "', 0, 0, 0)";
    db_query($query);

    $oid = db_last_insert_id("uc_attribute_options", "oid");
  }

  $query = "SELECT * FROM uc_product_attributes WHERE nid = {$nid} AND aid = {$aid}";
  $product_attributes = db_fetch_object(db_query($query));

  if (!$product_attributes) {
    //insert the product attribute reference
    $query = "INSERT INTO uc_product_attributes (nid, aid) VALUES ({$nid}, {$aid})";

    db_query($query);
  }

  //insert the product option reference
  $query = "INSERT INTO uc_product_options (nid, oid, cost, price, weight) VALUES ({$nid}, {$oid}, 0, 0, 0)";
  db_query($query);

  //set the value on the list so we can setup a product adjustment for it
  $options_list[$aid] = $oid;
}

//generates a request xml for sending down orders or asking for a particular order or filter
//objectType - The request type, valid: InventoryItem, NonInventoryItem, ServiceItem, DiscountItem
//args - array of the query arguments, refer to xml documents for valid list, the default will give all records
//itemIDs - array of the ids of the products that you whish to push down 
function courier_ubercart_request_product($objectType, $args = array("FromModifiedDate" => "1-1-1990"), $itemIDs = array()) {
  $item_mapping = courier_ubercart_get_product_mapping();

  $items = array();

  //get the orders that are defined in the orderIDs list
  if (count($itemIDs) > 0) {
    foreach ($itemIDs as $itemid) {
      $items[] = node_load($itemid);
    }
  }

  //build the request
  $doc = new SimpleXMLElement('<' . $objectType . "_Request" . ' />');

  //add the query args
  foreach ($args as $arg_name => $arg) {
    //some of the arguments are arrays
    if (is_array($arg)) {
      //add the element
      $list = $doc->addChild($arg_name);

      //the key is plural, need to drop the s to make the singular for each record
      $arg_single = substr_replace($arg_name, "", -1);

      foreach ($arg as $a) {
        $list->addChild($arg_single, $a);
      }

      $doc->addChild($list);
    }
    else {
      $doc->addChild($arg_name, $arg);
    }
  }

  if (count($items) > 0) {
    //add the object we are pushing down
    $objects = $doc->addChild($objectType . "s");

    //loop through each of the orders
    foreach ($items as $itm) {
      $object = $objects->addChild($objectType);

      //add the keys that are mapped from the order item to the xml
      foreach ($item_mapping as $uc_key => $field) {
        if ($field != '') {
          //make sure the value is not a default value
          if (preg_match('/^{.*?}$/', $field) == 0) {
            if ($uc_key == "body") {
              //this is a special case, have to handle diferently
              $object->addChild($field, $itm->body['und'][0]['value']);
            }
            else if (isset($itm->$uc_key)) {
              $object->addChild($field, (string) $itm->$uc_key);
            }
          }
        }
      }

      //add the related id data, these are not mappable so wouldnt appear in that list
      $object->addChild("RelatedID", $itm->nid);

      //add the quickbooks related values
      $qb_itemid = db_query("select field_quickbooks_id_value from {field_data_field_quickbooks_id} where bundle = 'product' and entity_id = :v", array(":v" => $itm->nid))->fetchField();

      if ($qb_itemid != '')
        $object->addChild($objectType . "ID", $qb_itemid);
    }
  }

  //write the request to the request table
  db_insert('courier_requests') // Table name no longer needs {}
    ->fields(array(
      'request_xml' => trim(str_replace('<?xml version="1.0"?>', '', $doc->asXML()))
    ))
    ->execute();
}

