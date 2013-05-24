<?php

$count = db_result(db_query("select count(*) as count from courier_requests"));

$requests = db_query("select * from courier_requests");

//log that the request happened
log_transaction(0, "OK", "Received request for jobs, amount of jobs found: {$count}");

//the response id will be sent back down to the client so that it can log the fact the webservice got the response
$response .= "status='OK' number_of_jobs='{$count}'>";

while($request = db_fetch_object($requests))
{        
    log_transaction(0, "OK", "Added job id: {$request->job_id} to the response");
    
    $response .= "<courier_request job_id='{$request->job_id}' job_date='{$request->job_date}'>";
    
    if(is_file($request_poll_pre_process_script))
    {
        //include the script the pre process script
        include $request_poll_pre_process_script;        
    }

    $response .= $request->request_xml;
    
    //move the record off off the response table and into the log table
    db_query("insert into courier_requests_log (job_id, job_date, request_xml)
                            Select * from courier_requests where job_id = '%s'", array($request->job_id));

    db_query("delete from courier_requests where job_id = '%s'", array($request->job_id));

    if(is_file($request_poll_post_process_script))
    {
        //include the post process script
        include $request_poll_post_process_script;
    }
    
    $response .= "</courier_request>";
}

?>