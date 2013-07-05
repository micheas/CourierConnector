<?php

function courier_ubercart_process_payment($objectType, $responseXml) {
  //write the error to some log if there was one
  if ($responseXml->ErrorMessage != '') {
    //handle error
  }

  print_r(uc_payment_load(1));
  exit;

  $plural = $objectType . 's';
  $id = $objectType . 'ID';

  foreach ($responseXml->$plural->$objectType as $payment) {
    $uc_payment = NULL;

    try {
      //try and find a node that has this quickbooks id
      $paymentid = db_query("select entity_id from {field_data_field_quickbooks_id} where field_quickbooks_id_value = :v", array(":v" => $payment->$id))->fetchField();

      //check if there is a hard node realtion
      if ($payment->RelatedID && $payment->RelatedID != '') {
        //get the record from the related id
      }
      else {
        if ($payment != '') {
          //use the paymentid
        }
        else {
          //new record
        }
      }

      //load data based on the mapping

      //define the default mapping, and check the settings to see if there is new mapping defined
      $mapping = array();
      $mapping['title'] = 'Name';

      //get some value from the database
      //be sure to pass the object type value in, so can use mapping based on type from qb

      //if not empty, parse mapping into the array

      //if it is empty
      //use the default

      foreach ($mapping as $local => $xmlkey) {
        if ($item->$xmlkey) {
          $uc_payment->$local = (string) $payment->$xmlkey;
        }
        else {
          //set the value if its not blank
          if ($xmlkey != '') {
            $uc_payment->$local = $xmlkey;
          }
        }
      }

      //set the quickbooks related id
      if ($payment->$id != '') {
        $uc_payment->field_quickbooks_id[$uc_payment->language][0]['value'] = $payment->$id;
      }


    } catch (Exception $ex) {
      $errorMessage = $ex->getMessage();

      print_r($errorMessage);
    }
  }
}

