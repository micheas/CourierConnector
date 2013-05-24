<?php

//the response id will be sent back down to the client so that it can log the fact the webservice got the response
$response .= "status='OK'><Reponse_Id>";
   
//log that the response happened
log_transaction(1, $status, "Got response to job_id: " . $job_id);

//update the database with this xml request
//set the job id so the response script can process this job response
db_query("insert into courier_responses (job_id, status, record_count, number_of_chunks, chunk_number, response_xml) 
                                        values('%s', '%s', '%s', '%s', '%s', '%s')", $job_id, $status, $record_count, $number_of_chunks, $chunk_number, $response_xml);

$response_id = db_last_insert_id("courier_reponses", "response_id");

if($response_processor_script != '')
{    
    //call the script that will process the response            
    $ch = curl_init(); 
    curl_setopt($ch, CURLOPT_URL, "http://" . $_SERVER['SERVER_ADDR'] . base_path() . "?q=" . $response_processor_script); 
    curl_setopt($ch, CURLOPT_HEADER, TRUE); 
    curl_setopt($ch, CURLOPT_NOBODY, TRUE); // remove body 
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE); 
    curl_exec($ch);     
    curl_close($ch); 
}

log_transaction(1, $status, "Processed response to job_id: " . $job_id);

//move the record off off the response table and into the log table
db_query("insert into courier_responses_log (response_id, job_id, job_date, status, record_count, number_of_chunks, chunk_number, response_xml)
                        Select * from courier_responses where response_id = '%s'", array($response_id));

db_query("delete from courier_responses where response_id = '%s'", array($response_id));
    
$response .= $response_id . "</Reponse_Id>";

?>