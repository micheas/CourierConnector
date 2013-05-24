<?php

function courier_ubercart_courier_ws()
{
  error_reporting(E_ERROR | E_WARNING | E_PARSE);
  require_once("lib/nusoap.php");
  require_once("log.php");
   
  $namespace = "courierservices";
  // create a new soap server
  $server = new soap_server();
  // configure our WSDL
  $server->configureWSDL("courierWebService");
  // set our namespace
  $server->wsdl->schemaTargetNamespace = $namespace;

  $server->register(
                   // method name:
                  'poll_job_requests',
                  // parameter list:
                  array('apiKey'=>'xsd:string'                                               
                       ),
                  // return value(s):
                  array('return'=>'xsd:string'),
                  // namespace:
                  $namespace,
                  // soapaction: (use default)
                  false,
                  // style: rpc or document
                  'rpc',
                  // use: encoded or literal
                  'encoded',
                  // description: documentation for the method
                  ''
                   );        
  
   $server->register(
                   // method name:
                  'post_job_response',
                  // parameter list:
                  array('apiKey'=>'xsd:string', 
                        'job_id'=>'xsd:int',                        
                        'status'=>'xsd:string',
                        'record_count'=>'xsd:int',
                        'number_of_chunks'=>'xsd:int',
                        'chunk_number'=>'xsd:int',
                        'response_xml'=>'xsd:string'
                       ),
                  // return value(s):
                  array('return'=>'xsd:string'),
                  // namespace:
                  $namespace,
                  // soapaction: (use default)
                  false,
                  // style: rpc or document
                  'rpc',
                  // use: encoded or literal
                  'encoded',
                  // description: documentation for the method
                  ''
                   );   
   
    $server->register(
                   // method name:
                  'test_connection',
                  // parameter list:
                  array('apiKey'=>'xsd:string'                        
                       ),
                  // return value(s):
                  array('return'=>'xsd:string'),
                  // namespace:
                  $namespace,
                  // soapaction: (use default)
                  false,
                  // style: rpc or document
                  'rpc',
                  // use: encoded or literal
                  'encoded',
                  // description: documentation for the method
                  ''
                   );    
                   
  // Get our posted data if the service is being consumed
  // otherwise leave this data blank.
  $POST_DATA = isset($GLOBALS['HTTP_RAW_POST_DATA'])
                  ? $GLOBALS['HTTP_RAW_POST_DATA'] : '';

  // pass our posted data (or nothing) to the soap service
  $server->service($POST_DATA);
}

//request the jobs in the request table
function poll_job_requests($apiKey)
{	
    include("config.php");
        
    $response = "<courier_response ";
    
    //check the api key
    if($apiKey == $api_key)      
    {
        include "request_processor.php";
    }    
    else
    {
        log_transaction(0, "Error", "Access requested with Invalid API Key");
        
        $response .= "status='Error'><Error_Message>Invalid API Key</Error_Message>";
    }
    
    $response .= "</courier_response>";        
    
    return $response;    
}

function post_job_response($apiKey, $job_id, $status, $record_count, $number_of_chunks, $chunk_number, $response_xml)
{
    include("config.php");
       
    $response = "<courier_response ";
    
    //check the api key
    if($apiKey == $api_key)
    {
        include "response_processor.php";
    }  
    else
    {
        log_transaction(1, "Error", "Access requested with Invalid API Key");
        $response .= " status='Error'><Error_Message>Invalid API Key</Error_Message>";
    }
    
    $response .= "</courier_response>";
    
    return $response;    
}

function test_connection($apiKey)
{
    include("config.php");

    $response = "<courier_response ";
    
    if($apiKey == $api_key)
    {        
        $response .= " status='OK'>";
    }
    else
    {
        log_transaction(1, "Error", "Access requested with Invalid API Key");
        $response .= " status='Error'><Error_Message>Invalid API Key</Error_Message>";
    }
    
    $response .= "</courier_response>";
    
    return $response; 
}

?>