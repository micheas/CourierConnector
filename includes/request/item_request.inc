<?php

function courier_ubercart_generate_item_request($method, $items)
{  
    $item_data = array();
        
    if(!is_array($items))
    {                
        $items = explode(',', $items);
    }
     
    if(is_array($items))
    {                
        foreach($items as $item)
        {            
            if(gettype($item) != "object")
            {                                
                if($item != '')
                {
                    $node = node_load($item);
                    $item = db_query("select * from uc_products where nid = :nid", array(":nid" => $node->nid));
                }
                else
                    continue;             
            }
                            
            //get the mapping data
            //get a list of the order mapping fields
            $product_mapping = db_query("select * from courier_product_mapping");
            
            //init the xml document           
            $item_xml = new DOMDocument();
            $item_root = $item_xml->createElement($method . "_Request");
            $item_xml->appendChild($item_root);
            $item_xml->formatOutput = true;
                        
            $items = $item_xml->createElement($method . "s");
            $item = $item_xml->createElement($method);
            
            $item_root->appendChild($items); 

            foreach($product_mapping as $field)
            {
                if($field->custom_value != '')
                {
                    $item->appendChild($item_xml->createElement ($field->uc_field, $field->custom_value));                    
                }
                else if($field->qb_field != "Not_Mapped")
                {                                        
                    $x = $field->uc_field;   
                    
                    if(isset($item->$x))
                        $item->appendChild($item_xml->createElement ($field->qb_field, $item->$x));
                    else
                    {
                        
                        $item->appendChild($item_xml->createElement ($field->qb_field, $node->$x));
                    }
                }                                
            }

            //check for a related id on the customer record

            //add the customers uid so it relates back to the order in the download            
            $item->appendChild($item_xml->createElement ("RelatedID", $node->nid));
                 
            $items->appendChild($item);
                                    
            $item_data[] = $item_xml;
        }
    }
    
    return $item_data;
}

?>
