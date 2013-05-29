<?php

//the function getting called that will execute the update
function courier_connector_courier_update($job_id)
{     
  require_once "courier_connector_mapping.inc";
  require_once "courier_connector_misc.inc";
 
  if(courier_connector_get_drupal_version() == 6) {
    $result = db_query("select response_xml, job_id, job_date, status, record_count, number_of_chunks, chunk_number from courier_responses where job_id = %d order by chunk_number desc", $job_id);
  }
  else {  
    $query = db_select("courier_responses");
    $query->fields("courier_responses", array("response_xml", "job_id", "job_date", "status", "record_count", "number_of_chunks", "chunk_number"));
    $query->condition("courier_responses.job_id", $job_id);
    $query->orderBy("courier_responses.chunk_number", "DESC");  

    $result = $query->execute();
    
    if(is_array($result))
      $result = (array) $result;
  }
  
  foreach ($result as $record) 
  {                                    
      $doc = simplexml_load_string((string)$record->response_xml);

      $n = (string) $doc->getName();

      $message = "Processing response from job id: {$record->job_id} chunk number: {$record->chunk_number} of {$record->number_of_chunks}";

      if($doc->ErrorMessage != '')
      {
          $message .= " error occured. Error Message: {$doc->ErrorMessage}";
          $status = "Error";
      }
      else
      {                          
          //write the request to the request table
          $status = "True";
      }

      if(courier_connector_get_drupal_version() == 6) {
        $result = db_query("insert into courier_transaction_log (type, message, status) value('1', '%s', '%s')", $message, $status);
      }
      else { 
        //write the request to the request table
        db_insert('courier_transaction_log') 
            ->fields(array(
              'type' => 1,
              'message' => $message,
              'status' => $status
            ))
            ->execute();
      }

      //call the function
      $method = "courier_connector_" . $n;

      $source_file = "response/" . $n . ".inc";
      
      require_once $source_file;

      $method($doc);      
  }

  return "done";
}

?>