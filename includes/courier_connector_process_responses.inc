<?php
/**
 * @file
 * The functions to process the responses.
 */

/**
 * the function getting called that will execute the update
 * @param $job_id
 * @return string
 */
function courier_connector_process_response($job_id) {
  require_once "courier_connector_mapping.inc";
  require_once "courier_connector_misc.inc";

  $response_parameters = file_get_contents(drupal_get_path('module', 'courier_connector_commerce') . "/includes/" . "response_parameters.json");
  $response_parameters = json_decode($response_parameters, TRUE);

  $query = db_select("courier_responses");
  $query->fields("courier_responses", array(
    "response_id",
    "response_xml",
    "job_id",
    "job_date",
    "status",
    "record_count",
    "number_of_chunks",
    "chunk_number",
  ));
  $query->condition("courier_responses.job_id", $job_id);
  $query->condition("processed", "0", "=");
  $query->orderBy("courier_responses.chunk_number", "DESC");

  $result = $query->execute();


  foreach ($result as $record) {
    $record = (array) $record;

    $doc = simplexml_load_string((string) $record['response_xml']);

    $method_name = (string) $doc->getName();

    //remove the _Response
    $method_name = str_replace('_Response', '', $method_name);

    $message = "Processing response from job id: {$record['job_id']} chunk number: {$record['chunk_number']} of {$record['number_of_chunks']}";

    if ($doc->ErrorMessage != '') {
      $message .= " error occured. Error Message: {$doc->ErrorMessage}";
      $status = "Error";
    }
    else {
      //write the request to the request table
      $status = "True";
    }

    //write the request to the request table
    db_insert('courier_transaction_log')
      ->fields(array(
        'type' => 1,
        'message' => $message,
        'status' => $status,
      ))
      ->execute();


    //call the function
//watchdog('Quickbooks import', print_r($response_parameters[COURIER_CONNECTOR_CART][$method_name]['file'], true) , WATCHDOG_DEBUG);
    require_once $response_parameters[COURIER_CONNECTOR_CART][$method_name]['file'];
    $response_parameters[COURIER_CONNECTOR_CART][$method_name]['method']($doc);

    //mark the response processed and mark a date on it
    $query = db_update("courier_responses");
    $query->fields(array("processed" => 1, "processed_date" => date('Y-m-d H:i:s', time())));
    $query->condition('response_id', $record['response_id']);
    $query->execute();
  }

  return "done";
}
