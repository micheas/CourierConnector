<?php

function courier_ubercart_generate_ItemInventory_Request($items)
{
    require_once "item_request.inc";
                
    //calls the generic item request creation
    $item_data = courier_ubercart_generate_item_request("ItemInventory", $items);

    foreach($item_data as $item)
    {       
        //add any extra data here
        
        //write the to the database
        db_query("Insert into courier_requests (request_xml) values(:xml)", array(":xml" => trim(str_replace('<?xml version="1.0"?>', "", $item->saveXML())))); 
    }  
}

?>
