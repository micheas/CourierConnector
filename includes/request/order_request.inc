<?php

function courier_ubercart_generate_order_request($method, $orders)
{    
    $order_data = array();
    
    if(!is_array($orders))
    {                
        $orders = explode(',', $orders);
    }
    
    if(is_array($orders))
    {                
        foreach($orders as $order)
        {
            if(gettype($order) != "object")
            {
                if($order != '')
                    $order = uc_order_load($order);
                else
                    continue;
            }
            
            $user = user_load($order->uid);
                        
            //get the mapping data
            //get a list of the order mapping fields
            $order_mapping = db_query("select * from courier_order_mapping");
            $customer_mapping = db_query("Select * from courier_customer_mapping");
            $item_mapping = db_query("select * from courier_order_product_mapping");

            //init the xml document           
            $customer_xml = new DOMDocument();
            $customer_root = $customer_xml->createElement("Customer_Request");
            $customer_xml->appendChild($customer_root);
            $customer_xml->formatOutput = true;
            
            $salesreceipt_xml = new DOMDocument();
            $salesreceipt_root = $salesreceipt_xml->createElement($method . "_Request");
            $salesreceipt_xml->appendChild($salesreceipt_root);
            $salesreceipt_xml->formatOutput = true;
            
            //handle the customer download
            $customers = $customer_xml->createElement("Customers");
            $customer = $customer_xml->createElement("Customer");
            
            $customer_root->appendChild($customers); 

            while($field = db_fetch_object($customer_mapping))
            {
                if($field->custom_value != '')
                {
                    $customer->appendChild($customer_xml->createElement ($field->uc_field, $field->custom_value));                    
                }
                else if($field->qb_field != "Not_Mapped")
                {                                        
                    $x = $field->uc_field;                    
                    
                    if(isset($order->$x))
                        $customer->appendChild($customer_xml->createElement ($field->qb_field, $order->$x));
                    else
                       $customer->appendChild($customer_xml->createElement ($field->qb_field, $user->$x));
                }                                
            }

            //check for a related id on the customer record

            //add the customers uid so it relates back to the order in the download            
            $customer->appendChild($customer_xml->createElement ("RelatedID", $order->uid));
                 
            $customers->appendChild($customer);
            
            $orders = $salesreceipt_xml->createElement($method . "s");
            $ord = $salesreceipt_xml->createElement($method);
            
            $salesreceipt_root->appendChild($orders); 
                        
            while($field = db_fetch_object($order_mapping))
            {
                if($field->custom_value != '')
                {                    
                    $ord->appendChild($salesreceipt_xml->createElement ($field->uc_field, $field->custom_value));
                }
                else if($field->qb_field != "Not_Mapped")
                {
                    $x = $field->uc_field;                    
                    $ord->appendChild($salesreceipt_xml->createElement ($field->qb_field, $order->$x));
                }
            }
            
            $items = $salesreceipt_xml->createElement($method . "Items");

            foreach($order->products as $product)
            {
                $item = $salesreceipt_xml->createElement($method . "Item");
                                
                while($field = db_fetch_object($item_mapping))
                {               
                    if($field->custom_value != '')
                    {                        
                        $item->appendChild($salesreceipt_xml->createElement ($field->uc_field, $field->custom_value));
                    }
                    else if($field->qb_field != "Not_Mapped")
                    {
                        $x = $field->uc_field;
                        $item->appendChild($salesreceipt_xml->createElement ($field->qb_field, $product->$x));
                    }
                }   
                
                //get the item id from the relations
                $related_id = db_result(db_query("select `relation_id` from `courier_product_relations` where nid = {$product->nid} and model = {$product->model}"));
                
                $item->appendChild($salesreceipt_xml->createElement ('ItemID', $related_id));
                
                $items->appendChild($item);
            }
                        
            //map the item data
            
            $ord->appendChild($salesreceipt_xml->createElement ("RelatedID", $order->order_id));
            
            $ord->appendChild($salesreceipt_xml->createElement ("RelatedCustomerID", $order->uid));

            $ord->appendChild($items);
            
            $orders->appendChild($ord);
            
            $order_d = array("customer" => $customer_xml,
                             "order" => $salesreceipt_xml
                            );
            
            $order_data[] = $order_d;
            
        }
    }
    
    return $order_data;
}

?>
