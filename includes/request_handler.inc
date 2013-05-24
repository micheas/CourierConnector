<?php

function generate_request()
{
    if(isset($_GET['request_type']) && isset($_GET['request_method']))
    {        
        if($_GET['request_type'] == "read")
        {
            require_once("read_request.inc");
            courier_ubercart_generate_read_request($_GET['request_method'], $_GET['parameters']);                         
        }
        else
        {            
            //call the function
            $method = "courier_ubercart_generate_" . $_GET['request_method'] . '_Request';

            require_once("request/" . $_GET['request_method'] . '_Request.inc');

            $method($_GET['parameters']);
        }
        
        return "Request Queued.";
    }
}

?>
