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

  foreach ($responseXml->$plural->$objectType as $department) {
    //check the processed tag, to see what we need to do with this response
    //4 possibles
    //error
    //added
    //modded
    //read
    $i = 0;
    $department_name = $department->DepartmentName;
    $department_id = $department->$id;
    watchdog('Courier import departments', ++$i . ' ' . $department->$id . ' is ' . $department->DepartmentName . ' Response status: ' . $department->Processed . "\n", array(), WATCHDOG_DEBUG);


    if ($department->Processed == "error") {
      //was already logged

    }
    elseif ($department->Processed == "added" || $department->Processed == "modded" || $department->Processed == "read") {
      // Test if the department taxonomy term exists, if not create it.

      $vocabulary = taxonomy_vocabulary_machine_name_load('department');
      watchdog('Quickbooks import', print_r($vocabulary, TRUE) . '$value = ' . $department_name, array(), WATCHDOG_INFO);
      $vid = $vocabulary->vid;
      $term = taxonomy_get_term_by_name($department_name, 'department');

      if (count($term) === 0) {
        $term = new stdClass();
        $term->name = $department_name;
        $term->vid = $vid;
        $term->field_quickbooks_id[LANGUAGE_NONE][0]['value']  = $department_id;
        taxonomy_term_save($term);
        $tid = $term->tid;
        db_insert('geo_location_debug')->fields(array(
          'vid' => $vid,
          // 'timestamp' let the database server set it.
          'tid' => $tid,
          'department' => $department_name,
          'qb_id' => $department_id,
        ))->execute();
      }
      else {
        foreach ($term as $tid) {
          $tid = $tid->tid;
        }
      }

      watchdog('Quickbooks import', $department_name . ' has a TID of ' . $tid, array(), WATCHDOG_INFO , 'taxonomy/term/' . $tid);


    }
    else {
      // unexpected result write the result in watchdog
      watchdog('Courier import departments', 'Unexpected result of ' . $department->Processed . " returned.\n", array(), WATCHDOG_DEBUG);
    }


  }
}
