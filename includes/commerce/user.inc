<?php

function courier_ubercart_get_user_mapping()
{
    //define the default mapping, and check the settings to see if there is new mapping defined
    $mapping = array();
    $mapping['name'] = 'CustomerRecordName';            
    $mapping['pass'] = 'Notes';            
    $mapping['mail'] = 'Email';            
    $mapping['status'] = '{1}';    

    return $mapping;            
}

function courier_ubercart_get_user_order_mapping()
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

    return $mapping;            
}

function courier_quickbooks_pos_process_user($objectType, $responseXml)
{
    //write the error to some log if there was one
    if($responseXml->ErrorMessage != '')
    {
        //handle error
    }
    
    $plural = $objectType . 's';
    $id = $objectType . 'ID';
    
    foreach($responseXml->$plural->$objectType as $user)
    {          
         //check the processed tag, to see what we need to do with this response
        //4 possibles
        //error 
        //added
        //modded
        //read 
        
        if($user->Processed == "error") 
        {
            //was already logged
            
        }
        else if($user->Processed == "added") 
        {
            //this is just a response that the record in qb was added
            
            //update the record locally to attach the qb id from this add
            
            //try and find a node that has this quickbooks id            
            $uid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'user' and field_quickbooks_id_value = :v", array(":v" => $user->$id))->fetchField();

            //check if there is a hard node realtion
            if($user->RelatedID && $user->RelatedID != '')
            {
                $usr = user_load((string)$user->RelatedID);
            }         
            else if($uid != '')
            {
                //load from the quickbooks related id
                $usr = user_load($uid);
            }
            
            $usr->language = LANGUAGE_NONE; 
            
            //set the quickbooks related id
            if($user->$id != '')
            {
                $usr->field_quickbooks_id[$usr->language][0]['value'] = $user->$id;                           
                //$edit['field_quickbooks_id'][$user->language][0]['value'] = $user->$id;
            }   

            //user_save($usr, $edit); 
            user_save($usr);
            
            //log the add            
            db_insert('courier_transaction_log') 
                ->fields(array(
                  'type' => 1,
                  'message' => "Customer Sent to QuickBooks, Customer ID: " . $user->uid,
                  'status' => "OK"
                ))
                ->execute();
        }    
        else if($user->Processed == "modded") 
        {
            //this is a response that the record has been modified, really nothing needs to be done
            //            
            //check if there is a hard node realtion
            if($user->RelatedID && $user->RelatedID != '')
            {
                $userid = $user->RelatedID; 
            }
            else
            {
                $userid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'user' and field_quickbooks_id_value = :v", array(":v" => $user->$id))->fetchField();  
            }
            
            //log the modification
            db_insert('courier_transaction_log') 
                ->fields(array(
                  'type' => 1,
                  'message' => "Customer update sent to QuickBooks, Customer ID: " . $userid,
                  'status' => "OK"
                ))
                ->execute();
        }
        //these are the result of a query, we will update / insert these
        else if($user->Processed == "read")        
        {
            //log the read
            db_insert('courier_transaction_log') 
                ->fields(array(
                  'type' => 1,
                  'message' => "Received Customer from QuickBooks. Name: " . $user->FullName . ", Adding to Customers.",
                  'status' => "OK"
                ))
                ->execute();
        
            $customer = new stdClass();   
            $edit = array();

            try
            {	
                //try and find a node that has this quickbooks id            
                $userid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'user' and field_quickbooks_id_value = :v", array(":v" => $user->$id))->fetchField();

                //check if there is a hard node realtion
                if($user->RelatedID && $user->RelatedID != '')
                {
                    //get the record from the related id
                    $customer = user_load((string)$user->RelatedID);
                }         
                else if($userid != '')
                {
                    //use the paymentid
                    $customer = user_load($userid);
                }
                else
                {
                    //new record
                    //$customer->init = 'email address';
                    $customer->is_new = true;
                }                 

                $customer->language = LANGUAGE_NONE;

                //load data based on the mapping
                //$mapping = courier_ubercart_get_user_mapping();
                $mapping = db_query("select * from {courier_customer_mapping}");

                //get some value from the database
                //be sure to pass the object type value in, so can use mapping based on type from qb

                //if not empty, parse mapping into the array

                //if it is empty
                //use the default

                foreach ($mapping as $mapping)
                {
                    if($map->qb_field != "Not_Mapped")
                    {
                        if($map->qb_field == "Custom")
                        {                   
                            $customer->$map->uc_field = (string) $user->$map->custom_value;
                            $edit[$map->uc_field] = (string) $user->$map->custom_value;
                        }
                        else
                        {
                           $customer->$map->uc_field = (string) $user->$map->qb_field;
                            $edit[$map->uc_field] = (string) $user->$map->qb_field;                          
                        }
                    }
                }

                //set the quickbooks related id
                if($user->$id != '')
                {
                    $customer->field_quickbooks_id[$customer->language][0]['value'] = $user->$id;      
                    $edit['field_quickbooks_id'][$customer->language][0]['value'] = $user->$id;
                }                           

                user_save($customer, $edit);                      
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
//objectType - The request type, valid: Customer
//args - array of the query arguments, refer to xml documents for valid list, the default will give all records
//ids - array of the ids of the customers that you whish to push down, if fromorders is true, then these are order ids
//fromoders - mark as yes and it will pull the data from the order record, the $ids will be treated as order ids
function courier_ubercart_request_user($objectType, $args = array("FromModifiedDate" => "1-1-1990"), $ids = array(), $fromorders = false)
{
    $mapping = db_query("select * from {courier_customer_mapping}");
    $order_mapping = db_query("select * from {courier_order_mapping}");
    
    $records = array();
    
    //get the orders that are defined in the orderIDs list
    if(count($ids) > 0)
    {
        foreach($ids as $id)
        {
            if($fromorders)
            {
                $order = uc_order_load($id);
                $user = user_load($order->uid);            
                $records[] = array("order" => $order, "user" => $user);                
            }
            else
            {
                $records[] = user_load($id);            
            }
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
        
    if(count($records) > 0)
    {
        //add the object we are pushing down
        $objects = $doc->addChild($objectType . "s");
    
        //loop through each of the orders
        foreach($records as $usr)
        {
            $object = $objects->addChild($objectType);
            
            if($fromorders)
            {                 
                foreach($order_mapping as $map)
                {                   
                    if($map->qb_field != "Not_Mapped")
                    {
                        if($map->uc_field == 'delivery_zone' || $map->uc_field == 'billing_zone')
                        {                            
                            $country_data = uc_get_country_data(array("country_id" => $usr['order']->$map->uc_field));
                            $object->addChild($map->qb_field, $country_data[0]['country_name']);
                        }
                        else if($local == 'delivery_country' || $local == 'billing_country')
                        {
                            $object->addChild($map->qb_field, uc_get_zone_code($usr['order']->$map->uc_field));                            
                        }                        
                        else if($order->$map->qb_field)
                        {                   
                            $object->addChild($map->qb_field, $usr['order']->$map->uc_field);
                        }        
                    }
                }    
                
                $usr = $usr['user'];
            } 
            
            //add the keys that are mapped from the order item to the xml
            foreach($mapping as $map)
            {
                if($map->qb_field != "Not_Mapped" && $map->qb_field != "Custom")                    
                {                   
                    $object->addChild($map->qb_field, $usr->$map->uc_field);
                }
            }     

            //add the related id data, these are not mappable so wouldnt appear in that list
            $object->addChild("RelatedID", $usr->uid);                       

            //add the quickbooks related values
            $qb_userid = db_query("select field_quickbooks_id_value from {field_data_field_quickbooks_id} where bundle = 'user' and entity_id = :v", array(":v" => $usr->nid))->fetchField();

            if($qb_userid != '')
                $object->addChild($objectType . "ID", $qb_userid);                                     
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