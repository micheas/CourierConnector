<?php

function courier_connector_commerce_process_department($responseXml) {
  require_once 'field_processor.inc';

  //write the error to some log if there was one
  if ($responseXml->ErrorMessage != '') {
    //handle error
  }

  $objectType = str_replace('_Response', '', $responseXml->getName());
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
    $i = 0;
    foreach ($responseXml->$plural->$objectType as $department) {
      watchdog('Courier import departments', ++$i . ' ' . $department->$id . ' is ' . $department->DepartmentName . ' Response status: ' . $department->Processed . "\n", array(), WATCHDOG_DEBUG);
    }

    if ($user->Processed == "error") {
      //was already logged

    }
    elseif ($user->Processed == "added") {
    }
    elseif ($user->Processed == "modded") {
    }
    //these are the result of a query, we will update / insert these
    elseif ($user->Processed == "read") {


    }


  }
}
