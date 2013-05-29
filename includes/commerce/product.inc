<?php

function courier_quickbooks_pos_process_product($objectType, $responseXml)
{
  require_once 'field_processor.inc';
  
  //should be set somewhere else
  $product_type = "product";

  $field_config_data = db_query("select field_config_instance.field_name, field_config.type,
              field_config.module from field_config_instance inner join field_config on field_config.id = field_config_instance.field_id
              where field_config_instance.bundle = '{$product_type}' and field_config_instance.entity_type = 'commerce_product'")->fetchAllAssoc("field_name");

   //write the error to some log if there was one
  if($responseXml->ErrorMessage != '')
  {
      //handle error
  }

  $plural = $objectType . 's';
  $id = $objectType . 'ID';

  foreach($responseXml->$plural->$objectType as $quickbooks_item_record)
  {      
      //check the processed tag, to see what we need to do with this response
     //4 possibles
     //error 
     //added
     //modded
     //read 

     if($quickbooks_item_record->Processed == "error") 
     {
         //was already logged

     }
     else if($quickbooks_item_record->Processed == "added") 
     {
         //this is just a response that the record in qb was added

         //update the record locally to attach the qb id from this add

         //try and find a node that has this quickbooks id            
         $itemid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'product' and field_quickbooks_id_value = :v", array(":v" => $quickbooks_item_record->$id))->fetchField();

         //check if there is a hard node realtion
         if($quickbooks_item_record->RelatedID && $quickbooks_item_record->RelatedID != '')
         {
             $node = node_load((string)$quickbooks_item_record->RelatedID);
         }         
         else if($nid != '')
         {
             //load from the quickbooks related id
             $node = node_load($nid);
         }

         $node->language = LANGUAGE_NONE;

         //set the quickbooks related id
         if($quickbooks_item_record->$id != '')
         {
             $node->field_quickbooks_id[$itm->language][0]['value'] = $quickbooks_item_record->$id;                           
         }   

         node_save($node);

         //log the add            
         db_insert('courier_transaction_log') 
             ->fields(array(
               'type' => 1,
               'message' => "Product Sent to QuickBooks, Product ID: " . $node->nid,
               'status' => "OK"
             ))
             ->execute();
     }    
     else if($quickbooks_item_record->Processed == "modded") 
     {
         //this is a response that the record has been modified, really nothing needs to be done
         //            
         //check if there is a hard node realtion
         if($quickbooks_item_record->RelatedID && $quickbooks_item_record->RelatedID != '')
         {
             $itemid = $quickbooks_item_record->RelatedID; 
         }
         else
         {
             $itemid = db_query("select entity_id from {field_data_field_quickbooks_id} where bundle = 'product' and field_quickbooks_id_value = :v", array(":v" => $quickbooks_item_record->$id))->fetchField();  
         }

         //log the modification
         db_insert('courier_transaction_log') 
             ->fields(array(
               'type' => 1,
               'message' => "Product update sent to QuickBooks, Product ID: " . $itemid,
               'status' => "OK"
             ))
             ->execute();
     }
     //these are the result of a query, we will update / insert these
     else if($quickbooks_item_record->Processed == "read")        
     {                    
         try
         {	                            
             //try and find a node that has this quickbooks id            
             $entity_id = db_query("select entity_id from {field_data_field_quickbooks_id} where entity_type = 'commerce_product' and field_quickbooks_id_value = :v", array(":v" => (string) $quickbooks_item_record->ItemInventoryID))->fetchField();                

             if(isset($quickbooks_item_record->RelatedID) && trim((string)$quickbooks_item_record->RelatedID) != '') {              
              $commerce_product_record = commerce_product_load($quickbooks_item_record->RelatedID);            

              if(!$commerce_product_record){
                //log an error
              }
              else {
               //log the read
               db_insert('courier_transaction_log') 
                   ->fields(array(
                     'type' => 1,
                     'message' => "Received {$objectType} from QuickBooks POS. Name: {$commerce_product_record->Name}, Updated entity_id {$entity_id}.",
                     'status' => "True"
                   ))
                   ->execute();
              }
                   
             }
             else if($entity_id !== false)
             {
                $commerce_product_record = commerce_product_load($entity_id);            

                if(!$commerce_product_record){
                  //log an error
                }
                else {
                 //log the read
                 db_insert('courier_transaction_log') 
                     ->fields(array(
                       'type' => 1,
                       'message' => "Received {$objectType} from QuickBooks POS. Name: {$quickbooks_item_record->Name}, Updated entity_id {$entity_id}.",
                       'status' => "True"
                     ))
                     ->execute();
                }
             }
             else if($sku_field = courier_quickbooks_pos_map_value("product_response", "sku", $quickbooks_item_record))
             {                   
               //try and do an auto relation base on the sku value

               $commerce_product_record = commerce_product_load_by_sku($sku_field);            

                if(!$commerce_product_record) {
                  //log an error
                }
                else {
                 //log the read
                 db_insert('courier_transaction_log') 
                     ->fields(array(
                       'type' => 1,
                       'message' => "Received {$objectType} from QuickBooks POS. Name: {$quickbooks_item_record->Name}, Updated entity_id {$entity_id}.",
                       'status' => "True"
                     ))
                     ->execute();
                }
             }                

             if(!$commerce_product_record)
             {                    
                 $commerce_product_record = commerce_product_new($product_type);                                          

                 $commerce_product_record->is_new = true;

                 //log the read
                 db_insert('courier_transaction_log') 
                     ->fields(array(
                       'type' => 1,
                       'message' => "Received " . $objectType . " from QuickBooks. Name: " . $quickbooks_item_record->Name . ", Adding to products.",
                       'status' => "OK"
                     ))
                     ->execute();
             }         

             foreach ($field_config_data as $field => $value)
             {                    
               $new_field_value = courier_quickbooks_pos_map_value("product_response", $field, $quickbooks_item_record);
               
               $commerce_product_record->$field = courier_quickbooks_pos_process_field($new_field_value, $field, $value->type);
             }
             
             //set the sku this is not field either
             $sku_value = courier_quickbooks_pos_map_value("product_response", "sku", $quickbooks_item_record);               
             $commerce_product_record->sku = $sku_value;
                         
             //set the quickbooks related id
             if($quickbooks_item_record->ItemInventoryID != '') {
                 $commerce_product_record->field_quickbooks_id['und'][0]['value'] = (string) $quickbooks_item_record->ItemInventoryID;                           
             }     
             $commerce_product_record->uid = 0;
             $result = commerce_product_save($commerce_product_record);
             
             //echo $result;
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
//objectType - The request type, valid: InventoryItem, NonInventoryItem, ServiceItem, DiscountItem
//args - array of the query arguments, refer to xml documents for valid list, the default will give all records
//itemIDs - array of the ids of the products that you whish to push down 
function courier_ubercart_request_product($objectType, $args = array("FromModifiedDate" => "1-1-1990"), $itemIDs = array())
{
    $item_mapping = courier_ubercart_get_product_mapping();
   
    $items = array();
    
    //get the orders that are defined in the orderIDs list
    if(count($itemIDs) > 0)
    {
        foreach($itemIDs as $itemid)
        {
            $items[] = node_load($itemid);
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
    
    if(count($items) > 0)
    {
        //add the object we are pushing down
        $objects = $doc->addChild($objectType . "s");
    
        //loop through each of the orders
        foreach($items as $itm)
        {
            $object = $objects->addChild($objectType);
            
            //add the keys that are mapped from the order item to the xml
            foreach($item_mapping as $uc_key => $field)
            {
                if($field != '')
                {
                    //make sure the value is not a default value                                
                    if(preg_match('/^{.*?}$/', $field) == 0)
                    {                         
                        if($uc_key == "body")
                        {
                            //this is a special case, have to handle diferently
                            $object->addChild($field, $itm->body['und'][0]['value']);
                        }
                        else if(isset($itm->$uc_key))     
                        {
                            $object->addChild($field, (string) $itm->$uc_key);
                        }
                    }
                }
            }     
            
            //add the related id data, these are not mappable so wouldnt appear in that list
            $object->addChild("RelatedID", $itm->nid);                       
                        
            //add the quickbooks related values
            $qb_itemid = db_query("select field_quickbooks_id_value from {field_data_field_quickbooks_id} where bundle = 'product' and entity_id = :v", array(":v" => $itm->nid))->fetchField();
            
            if($qb_itemid != '')
                $object->addChild($objectType . "ID", $qb_itemid);                     
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