<?php
/**
 * @file
 * the functions to process a response from quickbooks with commerce.
 */

/**
 * processes a response xml
 * @param $response_xml
 */
function courier_connector_commerce_process_order($response_xml) {
  require_once 'field_processor.inc';

  //write the error to some log if there was one
  if ($response_xml->ErrorMessage != '') {
    //handle error
  }

  $object_type = (string) $response_xml->getName();

  //remove the _Response
  $object_type = str_replace('_Response', '', $object_type);

  $field_config_data = db_query("select field_config_instance.field_name, field_config.type,
                field_config.module from field_config_instance inner join field_config on field_config.id = field_config_instance.field_id
                where field_config_instance.bundle = 'commerce_order' and field_config_instance.entity_type = 'commerce_order'")->fetchAllAssoc("field_name");

  $plural = $object_type . 's';
  $id = $object_type . 'ID';

  foreach ($response_xml->$plural->$object_type as $quickbooks_order_record) {
    //check the processed tag, to see what we need to do with this response
    //4 possibles
    //error
    //added
    //modded
    //read

    if ($quickbooks_order_record->Processed == "error") {
      //was already logged

    }
    else {
      if ($quickbooks_order_record->Processed == "added") {
        //this is just a response that the record in qb was added

        //update the record locally to attach the qb id from this add

        //try and find a node that has this quickbooks id
        $orderid = db_query("SELECT entity_id FROM {field_data_field_quickbooks_id} WHERE bundle = 'commerce_order' AND field_quickbooks_id_value = :v", array(":v" => $quickbooks_order_record->$id))->fetchField();

        //check if there is a hard node realtion
        if ($quickbooks_order_record->RelatedID && $quickbooks_order_record->RelatedID != '') {
          //get the record from the related id
          $commerce_order = commerce_order_load($quickbooks_order_record->RelatedID);
        }
        else {
          if ($orderid != '') {
            //use the $orderid
            $commerce_order = commerce_order_load($orderid);
          }
        }

        $commerce_order->language = LANGUAGE_NONE;

        //set the quickbooks related id
        if ($quickbooks_order_record->$id != '') {
          $commerce_order->field_quickbooks_id[$commerce_order->language][0]['value'] = (string) $quickbooks_order_record->$id;
        }

        commerce_order_save($commerce_order);

        //log the add
        db_insert('courier_transaction_log')
          ->fields(array(
            'type' => 1,
            'message' => "Order Sent to QuickBooks POS, Order ID: " . $commerce_order->order_id,
            'status' => "OK"
          ))
          ->execute();
      }
      else {
        if ($quickbooks_order_record->Processed == "modded") {
          //this is a response that the record has been modified, really nothing needs to be done
          //
          //check if there is a hard node realtion
          if ($quickbooks_order_record->RelatedID && $quickbooks_order_record->RelatedID != '') {
            $orderid = $quickbooks_order_record->RelatedID;
          }
          else {
            $orderid = db_query("SELECT entity_id FROM {field_data_field_quickbooks_id} WHERE bundle = 'commerce_order' AND field_quickbooks_id_value = :v", array(":v" => $quickbooks_order_record->$id))->fetchField();
          }

          //log the modification
          db_insert('courier_transaction_log')
            ->fields(array(
              'type' => 1,
              'message' => "Order update sent to QuickBooks POS, Order ID: " . $orderid,
              'status' => "OK"
            ))
            ->execute();
        }
        //these are the result of a query, we will update / insert these
        else {
          if ($quickbooks_order_record->Processed == "read") {
            //log the read
            db_insert('courier_transaction_log')
              ->fields(array(
                'type' => 1,
                'message' => "Received " . $object_type . " from QuickBooks POS. TransactionNumber ID: " . $quickbooks_order_record->TransactionNumber . " adding order.",
                'status' => "OK"
              ))
              ->execute();

            $commerce_order = new stdClass();

            try {
              //try and find a node that has this quickbooks id
              $orderid = db_query("SELECT entity_id FROM {field_data_field_quickbooks_id} WHERE bundle = 'uc_order' AND field_quickbooks_id_value = :v", array(":v" => $quickbooks_order_record->$id))->fetchField();

              //check if there is a hard node realtion
              if ($quickbooks_order_record->RelatedID && $quickbooks_order_record->RelatedID != '') {
                //get the record from the related id
                $uc_order = uc_order_load($quickbooks_order_record->RelatedID);
              }
              else {
                if ($orderid != '') {
                  //use the $orderid
                  $uc_order = uc_order_load($orderid);
                }
                else {
                  //new record

                  //need the customer id
                  $uid = 0;

                  if ($quickbooks_order_record->RelatedCustomerID && $quickbooks_order_record->RelatedCustomerID != '') {
                    $uid = $quickbooks_order_record->RelatedCustomerID;
                  }
                  else {
                    $uid = db_query("SELECT entity_id FROM {field_data_field_quickbooks_id} WHERE bundle = 'user' AND field_quickbooks_id_value = :v", array(":v" => $quickbooks_order_record->CustomerID))->fetchField();
                  }

                  if ($uid == '') {
                    $uid = 0;
                  }

                  //TODO offer a option for the uploaded order status?
                  $uc_order = uc_order_new($uid, 'completed');
                }
              }

              $uc_order->language = LANGUAGE_NONE;

              //load data based on the mapping
              //$mapping = courier_ubercart_get_order_mapping();
              $mapping = db_query("SELECT * FROM {courier_order_mapping}");

              //get some value from the database
              //be sure to pass the object type value in, so can use mapping based on type from qb

              //if not empty, parse mapping into the array

              //if it is empty
              //use the default

              foreach ($mapping as $map) {
                if ($map->qb_field != "Not_Mapped") {
                  if ($map->uc_field == 'delivery_zone' || $map->uc_field == 'billing_zone') {
                    $uc_order->$map->uc_field = uc_get_zone_code((string) $quickbooks_order_record->$map->qb_field);
                  }
                  //                else if($local == 'delivery_country' || $local == 'billing_country')
                  //                {
                  //                    $uc_order->$local = uc_get_country_data(array("country_id" => $ord->$uc_key));
                  //                }
                  else {
                    if ($map->qb_field == "Custom") {
                      $uc_order->$map->uc_field = (string) $map->custom_value;
                    }
                    else {
                      if ($quickbooks_order_record->$map->qb_field) {
                        $uc_order->$map->uc_field = (string) $quickbooks_order_record->$map->qb_field;
                      }
                    }
                  }
                }
              }

              //$product_mapping = courier_ubercart_get_product_line_mapping();
              $product_mapping = db_query("SELECT * FROM {courier_order_product_mapping}");

              $items = $object_type . 'LineItems';
              $item = $object_type . 'LineItem';

              $products = array();

              foreach ($quickbooks_order_record->$items->$item as $lineitem) {
                $uc_order_product = new stdClass();

                $itemid = db_query("SELECT entity_id FROM {field_data_field_quickbooks_id} WHERE bundle = 'product' AND field_quickbooks_id_value = :v", array(":v" => $lineitem->Itemid))->fetchField();

                //map the products
                foreach ($product_mapping as $map) {
                  if ($map->qb_field != "Not_Mapped") {
                    if ($lineitem->$map->qb_field) {
                      $uc_order_product->$map->uc_field = (string) $lineitem->$map->qb_field;
                    }
                    else {
                      if ($map->qb_field == "Custom") {
                        $uc_order_product->$map->uc_field = (string) $map->qb_field;
                      }
                      else {
                        $uc_order_product->$map->uc_field = (string) $lineitem->$map->uc_field;
                      }
                    }
                  }
                }

                $products[] = $uc_order_product;
              }

              $uc_order->products = $products;

              //set the quickbooks related id
              if ($quickbooks_order_record->$id != '') {
                $uc_order->field_quickbooks_id['und'][0]['value'] = (string) $quickbooks_order_record->$id;
              }

              uc_order_save($uc_order);
            } catch (Exception $ex) {
              $error_message = $ex->getMessage();

              print_r($error_message);
            }
          }
        }
      }
    }
  }
}