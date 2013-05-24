<?php

function courier_ubercart_generate_Customer_Request($cs)
{
    if(!is_array($cs))
    {                
        $cs = explode(',', $cs);
    }
  
    if(is_array($cs))
    {         
        foreach($cs as $c)
        {
            if(gettype($c) != "object")
            {                                
                if($c != '')
                {
                    $user = user_load($c);
                    $order = db_query("select uc_orders.*, z.zone_name as 'billing_state_name', c.country_name as billing_country_name, zs.zone_name as 'delivery_state_name', 
                                        cs.country_name as 'delivery_country_name' from uc_orders inner join uc_zones z on 
                                        uc_orders.billing_zone = z.zone_id inner join uc_countries c on c.country_id = uc_orders.billing_country
                                        inner join uc_zones zs on 
                                        uc_orders.billing_zone = zs.zone_id inner join uc_countries cs on cs.country_id = uc_orders.billing_country
                                        where uid = :uid limit 1", array(":uid" => $user->uid))->fetchObject();
                }
                else
                    continue;             
            }
                        
            $customer_mapping = db_query("Select * from courier_customer_mapping");
            
            //init the xml document           
            $customer_xml = new DOMDocument();
            $customer_root = $customer_xml->createElement("Customer_Request");
            $customer_xml->appendChild($customer_root);
            $customer_xml->formatOutput = true;

            $customers = $customer_xml->createElement("Customers");
            $customer = $customer_xml->createElement("Customer");

            $customer_root->appendChild($customers); 

            foreach($customer_mapping as $field)
            {
                if($field->custom_value != '')
                {
                    $customer->appendChild($customer_xml->createElement ($field->uc_field, $field->custom_value));                    
                }
                else if($field->qb_field != "Not_Mapped")
                {                                        
                    $x = $field->uc_field;                    

                    if(isset($user->$x))
                        $customer->appendChild($customer_xml->createElement ($field->qb_field, $user->$x));
                    else
                       $customer->appendChild($customer_xml->createElement ($field->qb_field, $order->$x));
                }                                
            }

            //check for a related id on the customer record

            //add the customers uid so it relates back to the order in the download            
            $customer->appendChild($customer_xml->createElement ("RelatedID", $user->uid));

            $customers->appendChild($customer);

            db_query("Insert into courier_requests (request_xml) values(:xml)", array(":xml" => trim(str_replace('<?xml version="1.0"?>', "", $customer_xml->saveXML())))); 
        }
    }
}

?>
