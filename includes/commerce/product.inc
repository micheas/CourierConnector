<?php

function courier_connector_commerce_process_product($response_xml, $product_type = 'product') {
  require_once 'field_processor.inc';

  $response_name = (string) $response_xml->getName();

  $object_type = str_replace('_Response', '', $response_name);

  $field_config_data = db_query('select field_config_instance.field_name, field_config.type,
              field_config.module from field_config_instance inner join field_config on field_config.id = field_config_instance.field_id
              where field_config_instance.bundle = :product_type and field_config_instance.entity_type = :entity_type',
    array(':product_type' => $product_type, ':entity_type' => 'commerce_product'))->fetchAllAssoc("field_name");

  if ($response_xml->ErrorMessage != '') {
    watchdog('courier connector', $response_xml->ErrorMessage, array(), WATCHDOG_ERROR );
  }

  $plural = $object_type . 's';
  $id = $object_type . 'ID';

  foreach ($response_xml->$plural->$object_type as $quickbooks_item_record) {
    //check the processed tag, to see what we need to do with this response
    //4 possibles
    //error
    //added
    //modded
    //read

    if ($quickbooks_item_record->Processed == "error") {
      //was already logged
    }
    else {
      if ($quickbooks_item_record->Processed == "added") {
        // this is a response that the record in qb was added
        // update the record locally to attach the qb id from this add

        // try and find a node that has this quickbooks id (?shouldn't this only be true in ubercart??)
        $entity_id = db_query("select entity_id from {field_data_field_quickbooks_id} where entity_type = 'commerce_product' and field_quickbooks_id_value = :v", array(":v" => (string) $quickbooks_item_record->ItemInventoryID))->fetchField();

        //check if there is a hard node relation
        if ($quickbooks_item_record->RelatedID && $quickbooks_item_record->RelatedID != '') {
          $commerce_product_record = commerce_product_load($quickbooks_item_record->RelatedID);
        }
        else {
          if ($entity_id !== FALSE) {
            $commerce_product_record = commerce_product_load($entity_id);
          }
        }

        //set the quickbooks related id//
        if ($quickbooks_item_record->ItemInventoryID != '') {
          $commerce_product_record->field_quickbooks_id['und'][0]['value'] = (string) $quickbooks_item_record->ItemInventoryID;
        }

        $result = commerce_product_save($commerce_product_record);

        //log the add
        db_insert('courier_transaction_log')
          ->fields(array(
            'type' => 1,
            'message' => "Product Sent to QuickBooks, Product ID: " . $entity_id,
            'status' => "OK"
          ))
          ->execute();
      }
      else {
        if ($quickbooks_item_record->Processed == "modded") {
          // this is a response that the record has been modified, really nothing needs to be done
          // check if there is a hard node relation (?shouldn't this only be true in ubercart??)
          if ($quickbooks_item_record->RelatedID && $quickbooks_item_record->RelatedID != '') {
            $itemid = $quickbooks_item_record->RelatedID;
          }
          else {
            $itemid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'product' and field_quickbooks_id_value = :v", array(":v" => $quickbooks_item_record->$id))->fetchField();
          }

          //log the modification
          db_insert('courier_transaction_log')
            ->fields(array(
              'type' => 1,
              'message' => "Product update sent to QuickBooks, Product ID: " . $itemid,
              'status' => "OK"
            ))
            ->execute();
        }
        //these are the result of a query, we will update / insert these
        else {
          if ($quickbooks_item_record->Processed == "read") {
            try {
              //try and find a node that has this quickbooks id
              $entity_id = db_query("select entity_id from {field_data_field_quickbooks_id} where entity_type = 'commerce_product' and field_quickbooks_id_value = :v", array(":v" => (string) $quickbooks_item_record->ItemInventoryID))->fetchField();

              if (isset($quickbooks_item_record->RelatedID) && trim((string) $quickbooks_item_record->RelatedID) != '') {
                $commerce_product_record = commerce_product_load($quickbooks_item_record->RelatedID);

                if (!$commerce_product_record) {
                   watchdog('courier connector', $response_xml->ErrorMessage, array(), WATCHDOG_ERROR );
                }
                else {
                  //log the read
                  db_insert('courier_transaction_log')
                    ->fields(array(
                      'type' => 1,
                      'message' => "Received {$object_type} from QuickBooks POS. Name: {$commerce_product_record->Name}, Updated entity_id {$entity_id}.",
                      'status' => "True"
                    ))
                    ->execute();
                }

              }
              else {
                if ($entity_id !== FALSE) {
                  $commerce_product_record = commerce_product_load($entity_id);

                  if (!$commerce_product_record) {
                    watchdog('courier connector', "commerce_product_load($entity_id) Failed to load", array(), WATCHDOG_ERROR );
                  }
                  else {
                    db_insert('courier_transaction_log')
                      ->fields(array(
                        'type' => 1,
                        'message' => "Received {$object_type} from QuickBooks POS. Name: {$quickbooks_item_record->Name}, Updated entity_id {$entity_id}.",
                        'status' => "True"
                      ))
                      ->execute();
                  }
                }
                else {
                  if ($sku_field = courier_connector_map_value($object_type . "_response", "sku", $quickbooks_item_record)) {
                    //try and do an auto relation base on the sku value

                    $commerce_product_record = commerce_product_load_by_sku($sku_field);

                    if (!$commerce_product_record) {
                      //log an error
                    }
                    else {
                      //log the read
                      db_insert('courier_transaction_log')
                        ->fields(array(
                          'type' => 1,
                          'message' => "Received {$object_type} from QuickBooks POS. Name: {$quickbooks_item_record->Name}, Updated entity_id {$entity_id}.",
                          'status' => "True"
                        ))
                        ->execute();
                    }
                  }
                }
              }

              if (!$commerce_product_record) {
                $commerce_product_record = commerce_product_new($product_type);

                $commerce_product_record->is_new = TRUE;

                //log the read
                db_insert('courier_transaction_log')
                  ->fields(array(
                    'type' => 1,
                    'message' => "Received " . $object_type . " from QuickBooks. Name: " . $quickbooks_item_record->Name . ", Adding to products.",
                    'status' => "OK"
                  ))
                  ->execute();
              }

              foreach ($field_config_data as $field => $value) {
                $new_field_value = courier_connector_map_value($object_type . "_response", $field, $quickbooks_item_record);

                $commerce_product_record->$field = courier_connector_process_field($new_field_value, $field, $value->type);
              }

              //These values are ever present so not found in the field config
              $title_value = courier_connector_map_value($object_type . "_response", "title", $quickbooks_item_record);
              $commerce_product_record->title = $title_value;

              $sku_value = courier_connector_map_value($object_type . "_response", "sku", $quickbooks_item_record);
              $commerce_product_record->sku = $sku_value;

              //set the quickbooks related id
              if ($quickbooks_item_record->ItemInventoryID != '') {
                $commerce_product_record->field_quickbooks_id['und'][0]['value'] = (string) $quickbooks_item_record->ItemInventoryID;
              }
              $commerce_product_record->uid = 0;
              $result = commerce_product_save($commerce_product_record);

              //echo $result;
            } catch (Exception $ex) {
              $errorMessage = print_r($ex->getMessage(), TRUE);
              watchdog('courier connector', $errorMessage, array(), WATCHDOG_ERROR );
            }
          }
        }
      }
    }
  }
}