<?php
/**
 * @file
 * The department functions.
 */

/**
 * @param $response_xml
 */
function courier_connector_commerce_process_department($response_xml) {
  require_once 'field_processor.inc';

  //write the error to some log if there was one
  if ($response_xml->ErrorMessage != '') {
    //handle error
  }

  $object_type = str_replace('_Response', '', $response_xml->getName());
  $plural = $object_type . 's';
  $id = $object_type . 'ID';

  foreach ($response_xml->$plural->$object_type as $department) {
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
        db_insert('courier_department_cache')->fields(array(
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
