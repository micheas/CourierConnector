<?php

function courier_connector_create_responses($response, $job_id, $job_date, $status, $record_count, $number_of_chunks, $chunk_number) {

  if (courier_connector_get_drupal_version() == "6") {

  }
  else {
    $response_id = db_insert('courier_responses')
      ->fields(array(
        'job_id' => $job_id,
        'job_date' => $job_date,
        'status' => $status,
        'record_count' => $record_count,
        'number_of_chunks' => $number_of_chunks,
        'chunk_number' => $chunk_number,
        'response_xml' => $response,
      ))
      ->execute();

    //write the request to the request table
    db_insert('courier_transaction_log')
      ->fields(array(
        'type' => 1,
        'message' => "Received response for Job Date: {$job_date}, job id: {$job_id}, Status: {$status}, Record Count: {$record_count}, Number of Chunks: {$number_of_chunks}",
        'status' => $status
      ))
      ->execute();
  }
  return array("response_id" => $response_id); //"<response_id>{$response_id}</response_id>";
}

