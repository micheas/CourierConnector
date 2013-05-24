<?php

function courier_ubercart_generate_read_request($type, $parameters)
{   
    $doc = new DOMDocument();  
    $doc->formatOutput = true;
    $root = $doc->createElement($type . "_Request");
    $doc->appendChild($root);
    
    if($parameters != '')
        $params = explode('|', $parameters);
    else
        $root->appendChild($doc->createElement ("TimeModified", '1/1/2001'));
    
    if(is_array($params))
    {    
        foreach($params as $p)
        {
            $parameter = explode("~", $p);

            $root->appendChild($doc->createElement ($parameter[0], $parameter[1]));
        }    
    }       
    db_query("Insert into courier_requests (request_xml) values('%s')", trim(str_replace('<?xml version="1.0"?>', "", $doc->saveXML()))); 
}

?>
