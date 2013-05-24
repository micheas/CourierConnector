<?php

//the function getting called that will execute the update
function courier_update()
{                           
//    require 'uc_order.inc';
//    
//    $args = array();    
//    $ordersIDs = array();
//    
//    $ordersIDs[] = "25";
//    
////    $args['SalesReceiptIDs'] = array('33', '44');
////    $args['ReferenceNumbers'] = array('4', '3');
////    $args['FromModifiedDate'] = '1-1-1990';
////    $args['ToModifiedDate'] = '1-1-2013';
//       
//    courier_ubercart_request_order('SalesOrder', $args, $ordersIDs);
//    
//    echo "done3";
//    
//    exit;

//    require 'product.inc';    
//    $args = array();    
//    $ids = array();    
//    $ids[] = "22";    
////    $args['InventoryItemIDs'] = array('33', '44');
////    $args['ReferenceNumbers'] = array('4', '3');
////    $args['FromModifiedDate'] = '1-1-1990';
////    $args['ToModifiedDate'] = '1-1-2013';
////       
//    courier_ubercart_request_product('InventoryItem', $args, $ids);
//    
//    echo "done";
//    
//    exit;
    
    
//    require 'user.inc';    
//    $args = array();    
//    $ids = array();    
//    $ids[] = "25";    
////    $args['InventoryItemIDs'] = array('33', '44');
////    $args['ReferenceNumbers'] = array('4', '3');
////    $args['FromModifiedDate'] = '1-1-1990';
////    $args['ToModifiedDate'] = '1-1-2013';
////       
//    courier_ubercart_request_user('Customer', $args, $ids, true);
//    
//    echo "done";
//    
//    exit;
    
    //get the xml from the table    
    $result = db_query('SELECT * from courier_responses');
        
    while($record = db_fetch_object($result))
    {  
        try
        {
            $doc = simplexml_load_string((string)$record->response_xml);

            if($doc != null)
            {
                $n = (string) $doc->getName();

                $message = "Received response for job id: " . $record->job_id;

                if($doc->ErrorMessage != '')
                {
                    $message .= " error occured. Error Message: " . (string) $doc->ErrorMessage;
                    $status = "Error";
                }
                else
                {                          
                    //write the request to the request table
                    $status = "OK";
                }

                //write the request to the request table
                db_query("insert into courier_transaction_log (type, message, status) values('%s', '%s', '%s')", $type, $message, $status);

                //call the function
                $method = "courier_ubercart_" . $n;

                require_once("response/" . $n . '.inc');

                $method($doc);
                //courier_ubercart_InventoryItem_Response();
            }
        }
        catch(Exception $ex)
        {
            echo $ex->getMessage();
        }
    }
    
    return "done";
}

?>