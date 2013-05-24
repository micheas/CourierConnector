<?php

//script included after the response has been received and posted to the response table
//the response_id is set, the script should do a query for the record with that id and process accordingly
$response_processor_script = "courier/update";

//the script included before the processing of the request polling is ran
$request_poll_pre_process_script = "";

//script included right after the processing has got the list of job
$request_poll_post_process_script = "";

//the api key required for the connection
$api_key = "ABC";


?>