<?php

function courier_ubercart_get_order_mapping()
{
    //define the default mapping, and check the settings to see if there is new mapping defined
    $mapping = array();
    $mapping['delivery_first_name'] = 'ShippingFirstName';   
    $mapping['delivery_last_name'] = 'ShippingLastName';   
    $mapping['delivery_street1'] = 'ShippingStreet';   
    $mapping['delivery_street2'] = 'ShippingStreet2';   
    $mapping['delivery_city'] = 'ShippingCity';   
    $mapping['delivery_zone'] = 'ShippingState';   
    $mapping['delivery_postal_code'] = 'ShippingPostalCode';   
    $mapping['delivery_country'] = 'ShippingCountry';   
    $mapping['billing_first_name'] = 'BillingFirstName';   
    $mapping['billing_last_name'] = 'BillingLastName';   
    $mapping['billing_street1'] = 'BillingStreet';   
    $mapping['billing_street2'] = 'BillingStreet2';   
    $mapping['billing_city'] = 'BillingCity';   
    $mapping['billing_zone'] = 'BillingState';   
    $mapping['billing_postal_code'] = 'BillingPostalCode';   
    $mapping['billing_country'] = 'BillingCountry';   
    $mapping['order_status'] = '{completed}';      
    
    return $mapping;
}

function courier_ubercart_get_product_line_mapping()
{
    //product mapping
    
    $product_mapping = array();
    
    $product_mapping['title'] = 'ItemName';   
    $product_mapping['model'] = '';   
    $product_mapping['qty'] = 'Quantity';   
    $product_mapping['cost'] = 'Amount';   
    $product_mapping['price'] = 'Amount';   
    $product_mapping['weight'] = '{0}';   
    $product_mapping['weight_units'] = '{lb}'; 
    
    return $product_mapping;
}

//processes a response xml
function courier_ubercart_process_order($objectType, $responseXml)
{
    //write the error to some log if there was one
    if($responseXml->ErrorMessage != '')
    {
        //handle error
        
    }
        
    $order = uc_order_load(1);
    
    $plural = $objectType . 's';
    $id = $objectType . 'ID';
    
    foreach($responseXml->$plural->$objectType as $order)
    {            
        //check the processed tag, to see what we need to do with this response
        //4 possibles
        //error 
        //added
        //modded
        //read 
        
        if($order->Processed == "error") 
        {
            //was already logged
            
        }
        else if($order->Processed == "added") 
        {
            //this is just a response that the record in qb was added
            
            //update the record locally to attach the qb id from this add
            
            //try and find a node that has this quickbooks id            
            $orderid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'uc_order' and field_quickbooks_id_value = :v", array(":v" => $order->$id))->fetchField();

            //check if there is a hard node realtion
            if($order->RelatedID && $order->RelatedID != '')
            {
                //get the record from the related id
                $uc_order = uc_order_load($order->RelatedID);
            }         
            else if($orderid != '')
            {
                //use the $orderid
                $uc_order = uc_order_load($orderid);
            }
            
            $uc_order->language = LANGUAGE_NONE;
            
            //set the quickbooks related id
            if($order->$id != '')
            {
                $uc_order->field_quickbooks_id[$uc_order->language][0]['value'] = $order->$id;                           
            }               

            uc_order_save($uc_order);
            
            //log the add            
            db_insert('courier_transaction_log') 
                ->fields(array(
                  'type' => 1,
                  'message' => "Order Sent to QuickBooks, Order ID: " . $uc_order->order_id,
                  'status' => "OK"
                ))
                ->execute();
        }    
        else if($order->Processed == "modded") 
        {
            //this is a response that the record has been modified, really nothing needs to be done
            //            
            //check if there is a hard node realtion
            if($order->RelatedID && $order->RelatedID != '')
            {
                $orderid = $order->RelatedID; 
            }
            else
            {
                $orderid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'uc_order' and field_quickbooks_id_value = :v", array(":v" => $order->$id))->fetchField();  
            }
            
            //log the modification
            db_insert('courier_transaction_log') 
                ->fields(array(
                  'type' => 1,
                  'message' => "Order update sent to QuickBooks, Order ID: " . $orderid,
                  'status' => "OK"
                ))
                ->execute();
        }
        //these are the result of a query, we will update / insert these
        else if($order->Processed == "read")        
        {
            //log the read
            db_insert('courier_transaction_log') 
                ->fields(array(
                  'type' => 1,
                  'message' => "Received " . $objectType . " from QuickBooks. TransactionNumber ID: " . $order->TransactionNumber . " adding order.",
                  'status' => "OK"
                ))
                ->execute();
            
            $uc_order = new stdClass();   

            try
            {	
                //try and find a node that has this quickbooks id            
                $orderid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'uc_order' and field_quickbooks_id_value = :v", array(":v" => $order->$id))->fetchField();

                //check if there is a hard node realtion
                if($order->RelatedID && $order->RelatedID != '')
                {
                    //get the record from the related id
                    $uc_order = uc_order_load($order->RelatedID);
                }         
                else if($orderid != '')
                {
                    //use the $orderid
                    $uc_order = uc_order_load($orderid);
                }
                else
                {
                    //new record

                    //need the customer id
                    $uid = 0;

                    if($order->RelatedCustomerID && $order->RelatedCustomerID != '')
                    {
                        $uid = $order->RelatedCustomerID;
                    }
                    else
                    {
                        $uid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'user' and field_quickbooks_id_value = :v", array(":v" => $order->CustomerID))->fetchField();
                    }

                    if($uid == '')
                        $uid = 0;

                    //TODO offer a option for the uploaded order status?
                    $uc_order = uc_order_new($uid, 'completed');
                }                 

                $uc_order->language = LANGUAGE_NONE;

                //load data based on the mapping                                 
                //$mapping = courier_ubercart_get_order_mapping();
                $mapping = db_query("select * from {courier_order_mapping}");
                
                //get some value from the database
                //be sure to pass the object type value in, so can use mapping based on type from qb

                //if not empty, parse mapping into the array

                //if it is empty
                //use the default

                foreach ($mapping as $map)
                {
                    if($map->qb_field != "Not_Mapped")
                    {
                        if($map->uc_field == 'delivery_zone' || $map->uc_field == 'billing_zone')
                        {
                            $uc_order->$map->uc_field = uc_get_zone_code((string) $order->$map->qb_field);
                        }
        //                else if($local == 'delivery_country' || $local == 'billing_country')
        //                {
        //                    $uc_order->$local = uc_get_country_data(array("country_id" => $ord->$uc_key));   
        //                } 
                        else if($map->qb_field == "Custom")
                        {
                             $uc_order->$map->uc_field = (string) $map->custom_value;
                        }
                        else if($order->$map->qb_field)
                        {                   
                            $uc_order->$map->uc_field = (string) $order->$map->qb_field;
                        }        
                    }
                }

                //$product_mapping = courier_ubercart_get_product_line_mapping();
                $product_mapping = db_query("select * from {courier_order_product_mapping}");

                $items = $objectType . 'LineItems';
                $item =  $objectType . 'LineItem';

                $products = array();

                foreach($order->$items->$item as $lineitem)
                {     
                    $uc_order_product = new stdClass(); 

                    $itemid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'product' and field_quickbooks_id_value = :v", array(":v" => $lineitem->Itemid))->fetchField();

                    //map the products
                    foreach ($product_mapping as $map)
                    {
                        if($map->qb_field != "Not_Mapped")
                        {
                            if($lineitem->$map->qb_field)
                            {                   
                                $uc_order_product->$map->uc_field = (string) $lineitem->$map->qb_field;
                            }
                            else if($map->qb_field == "Custom")
                            {
                                $uc_order_product->$map->uc_field = (string) $map->qb_field;
                            }
                            else
                            {                                                  
                                $uc_order_product->$map->uc_field = (string) $lineitem->$map->uc_field;                            
                            }
                        }
                    }

                    $products[] = $uc_order_product;
                }

                $uc_order->products = $products;

                //set the quickbooks related id
                if($order->$id != '')
                {
                    $uc_order->field_quickbooks_id[$uc_order->language][0]['value'] = $order->$id;                           
                }   

                uc_order_save($uc_order);
            }
            catch(Exception $ex)
            {
                $errorMessage = $ex->getMessage();      

                print_r($errorMessage);
            } 
        }
    }
}

//generates a request xml for sending down orders or asking for a particular order or filter
//objectType - The request type, valid: Invoice, SalesReceipt, SalesOrder, PurchaseOrder
//args - array of the query arguments, refer to xml documents for valid list, the default will give all records
//orderIDs - array of the ids of the orders that you whish to push down 
function courier_ubercart_request_order($objectType, $args = array("FromModifiedDate" => "1-1-1990"), $ordersIDs = array())
{
    //$order_mapping = courier_ubercart_get_order_mapping();
    //$product_mapping = courier_ubercart_get_product_line_mapping();  
    $order_mapping = db_query("select * from {courier_order_mapping}");
    $product_mapping = db_query("select * from {courier_order_product_mapping}");    
    
    $orders = array();
    
    //get the orders that are defined in the orderIDs list
    if(count($ordersIDs) > 0)
    {
        foreach($ordersIDs as $orderid)
        {
            $orders[] = uc_order_load($orderid);
        }
    }
    
    //build the request    
    $doc = new SimpleXMLElement('<' . $objectType . "_Request" . ' />');
           
    //add the query args
    foreach($args as $arg_name => $arg)        
    {
        //some of the arguments are arrays
        if(is_array($arg))
        {
            //add the element
            $list = $doc->addChild($arg_name);
            
            //the key is plural, need to drop the s to make the singular for each record
            $arg_single = substr_replace($arg_name ,"",-1);
            
            foreach($arg as $a)
            {
                $list->addChild($arg_single, $a);
            }
            
            $doc->addChild($list);
        }
        else
        {
            $doc->addChild($arg_name, $arg);
        }
    }
    
    if(count($orders) > 0)
    {
        //add the object we are pushing down
        $objects = $doc->addChild($objectType . "s");
    
        //loop through each of the orders
        foreach($orders as $ord)
        {
            $object = $objects->addChild($objectType);
            
            //add the keys that are mapped from the order item to the xml
            foreach($order_mapping as $map)
            {
                //make sure the value is not a default value                                
                if($map->qb_field != "Custom" && $map->qb_field != "Not_Mapped")
                {
                    //country and state are special case, need to be translated
                    if($map->uc_field == 'delivery_country' || $map->uc_field == 'billing_country')
                    {                       
                        $country_data = uc_get_country_data(array("country_id" => $ord->$map->uc_field));
                        $object->addChild($map->qb_field, $country_data[0]['country_name']);
                    }
                    else if($map->uc_field == 'delivery_country' || $map->uc_field == 'billing_country')
                    {
                        $object->addChild($map->qb_field, uc_get_zone_code($ord->$map->uc_field));
                    }
                    else
                    {
                        $object->addChild($map->qb_field, $ord->$map->uc_field);
                    }                        
                }
            }     
            
            //add the order date
            $object->addChild("TransactionDate", gmdate("M d Y H:i:s", $ord->created));                      
            
            //add the related id data, these are not mappable so wouldnt appear in that list
            $object->addChild("RelatedID", $ord->order_id);                       
            $object->addChild("RelatedCustomerID", $ord->uid);
                        
            //add the quickbooks related values
            $qb_userid = db_query("select field_quickbooks_id_value from {field_data_field_quickbooks_id} where bundle = 'user' and entity_id = :v", array(":v" => $ord->uid))->fetchField();
            $qb_orderid = db_query("select field_quickbooks_id_value from {field_data_field_quickbooks_id} where bundle = 'uc_order' and entity_id = :v", array(":v" => $ord->order_id))->fetchField();
            
            if($qb_userid != '')
                $object->addChild("CustomerID", $qb_userid);               

            if($qb_orderid != '')
                $object->addChild($objectType . "ID", $qb_orderid);               
            
            $products = $object->addChild($objectType . "LineItems");
            
            //add the products
            foreach($ord->products as $product)
            {
                $prod = $products->addChild($objectType . "LineItem");
            
                //add the keys that are mapped from the order item to the xml
                foreach($product_mapping as $map)
                {
                    if($map->qb_field != "Not_Mapped")
                    {
                        //make sure the value is not a default value                                                       
                        $prod->addChild($map->qb_field, $product->$map->uc_field);
                    }
                }
                                
                //add the quickbooks related values
                $qb_itemid = db_query("select field_quickbooks_id_value from {field_data_field_quickbooks_id} where bundle = 'product' and entity_id = :v", array(":v" => $product->nid))->fetchField();
                
                if($qb_itemid != '')
                    $prod->addChild("ItemID", $qb_itemid);                                    
            }
        }
    }
    
    //write the request to the request table
    db_insert('courier_requests') // Table name no longer needs {}
            ->fields(array(
              'request_xml' => trim(str_replace('<?xml version="1.0"?>', '', $doc->asXML()))
            ))
            ->execute();
}

?>