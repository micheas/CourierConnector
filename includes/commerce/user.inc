<?php

function courier_connector_commerce_process_user($responseXml) {
  require_once 'field_processor.inc';

  //write the error to some log if there was one
  if ($responseXml->ErrorMessage != '') {
    //handle error
  }

  $objectType = (string) $responseXml->getName();

  //remove the _Response
  $objectType = str_replace('_Response', '', $objectType);

  $plural = $objectType . 's';
  $id = $objectType . 'ID';

  // This should be rewritten so it can deal with table prefixes.
  $field_config_data = db_query("select field_config_instance.field_name, field_config.type,
          field_config.module from field_config_instance inner join field_config on field_config.id = field_config_instance.field_id
          where field_config_instance.bundle = 'billing' and field_config_instance.entity_type = 'commerce_customer_profile'")->fetchAllAssoc("field_name");


  foreach ($responseXml->$plural->$objectType as $user) {
    //check the processed tag, to see what we need to do with this response
    //4 possibles
    //error
    //added
    //modded
    //read

    if ($user->Processed == "error") {
      //was already logged

    }
    else if ($user->Processed == "added") {
      //this is just a response that the record in qb was added

      //update the record locally to attach the qb id from this add

      //try and find a node that has this quickbooks id
      $uid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'user' and field_quickbooks_id_value = :v", array(":v" => $user->$id))->fetchField();

      //check if there is a hard node realtion
      if ($user->RelatedID && $user->RelatedID != '') {
        $usr = user_load((string) $user->RelatedID);
      }
      else {
        if ($uid != '') {
          //load from the quickbooks related id
          $usr = user_load($uid);
        }
      }

      $usr->language = LANGUAGE_NONE;

      //set the quickbooks related id
      if (isset($user->$id)) {
        $usr->field_quickbooks_id['und'][0]['value'] = (string) $user->$id;
        //$edit['field_quickbooks_id'][$user->language][0]['value'] = $user->$id;
      }

      //user_save($usr, $edit);
      user_save($usr);

      //log the add
      db_insert('courier_transaction_log')
        ->fields(array(
          'type' => 1,
          'message' => "Customer Sent to QuickBooks, Customer ID: " . $user->uid,
          'status' => "OK"
        ))
        ->execute();
    }
    else {
      if ($user->Processed == "modded") {
        //this is a response that the record has been modified, really nothing needs to be done
        //
        //check if there is a hard node realtion
        if ($user->RelatedID && $user->RelatedID != '') {
          $userid = $user->RelatedID;
        }
        else {
          $userid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'user' and field_quickbooks_id_value = :v", array(":v" => $user->$id))->fetchField();
        }

        //log the modification
        db_insert('courier_transaction_log')
          ->fields(array(
            'type' => 1,
            'message' => "Customer update sent to QuickBooks, Customer ID: " . $userid,
            'status' => "OK"
          ))
          ->execute();
      }
      //these are the result of a query, we will update / insert these
      else {
        if ($user->Processed == "read") {
          //log the read
          db_insert('courier_transaction_log')
            ->fields(array(
              'type' => 1,
              'message' => "Received Customer from QuickBooks. Name: " . $user->FullName . ", Adding to Customers.",
              'status' => "OK"
            ))
            ->execute();

          $customer = new stdClass();
          $edit = array();

          try {
            //try and find a node that has this quickbooks id
            $userid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'user' and field_quickbooks_id_value = :v", array(":v" => $user->$id))->fetchField();

            //check if there is a hard node realtion
            if ($user->RelatedID && $user->RelatedID != '') {
              //get the record from the related id
              $customer = user_load((string) $user->RelatedID);
            }
            else {
              if ($userid != '') {
                //use the paymentid
                $customer = user_load($userid);
              }
              else {
                //new record
                //$customer->init = 'email address';
                $customer->is_new = TRUE;
              }
            }

            $customer->language = LANGUAGE_NONE;

            //load data based on the mapping
            //$mapping = courier_ubercart_get_user_mapping();
            //$mapping = db_query("select * from {courier_customer_mapping}");

            foreach ($field_config_data as $field => $value) {
              //this is the stock field that is more of a place holder, these fields dont change
              if ($field == "commerce_customer_address") {
                $new_field_value = courier_connector_map_value($objectType . "_response", "commerce_customer_address_country", $user);
                $edit->$field = courier_connector_process_field($new_field_value, "commerce_customer_address_country", $value->type);

//                      "commerce_customer_address_country"
//                      "commerce_customer_address_administrative_area"
//                      "commerce_customer_address_sub_administrative_area"
//                      "commerce_customer_address_locality"
//                      "commerce_customer_address_dependent_locality"
//                      "commerce_customer_address_postal_code"
//                      "commerce_customer_address_thoroughfare"
//                      "commerce_customer_address_premise"
//                      "commerce_customer_address_sub_premise"
//                      "commerce_customer_address_organisation_name"
//                      "commerce_customer_address_name_line"
//                      "commerce_customer_address_first_name"
//                      "commerce_customer_address_last_name"
              }
              else {
                $new_field_value = courier_connector_map_value($objectType . "_response", $field, $user);
                $edit->$field = courier_connector_process_field($new_field_value, $field, $value->type);
              }
            }

            //get some value from the database
            //be sure to pass the object type value in, so can use mapping based on type from qb

            //if not empty, parse mapping into the array

            //if it is empty
            //use the default

//                foreach ($mapping as $mapping)
//                {
//                    if($map->qb_field != "Not_Mapped")
//                    {
//                        if($map->qb_field == "Custom")
//                        {
//                            $customer->$map->uc_field = (string) $user->$map->custom_value;
//                            $edit[$map->uc_field] = (string) $user->$map->custom_value;
//                        }
//                        else
//                        {
//                           $customer->$map->uc_field = (string) $user->$map->qb_field;
//                            $edit[$map->uc_field] = (string) $user->$map->qb_field;
//                        }
//                    }
//                }
//
//                //set the quickbooks related id
//                if($user->$id != '')
//                {
//                    $customer->field_quickbooks_id[$customer->language][0]['value'] = $user->$id;
//                    $edit['field_quickbooks_id'][$customer->language][0]['value'] = $user->$id;
//                }

            user_save($customer, $edit);
          } catch (Exception $ex) {
            $errorMessage = $ex->getMessage();

            print_r($errorMessage);
          }
        }
      }
    }
  }
}