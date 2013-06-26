<?php

function courier_connector_process_field($value, $field_name, $field_type) {
  
  $return_field = array();
  
  switch($field_type) {
  
    case 'entityreference':
      $return_field[LANGUAGE_NONE][0]['target_id'] = $value;
      break;
  
    case 'text':    
      $return_field[LANGUAGE_NONE][0]['value'] = $value;
      $return_field[LANGUAGE_NONE][0]['format'] = 'full_html';
      $return_field[LANGUAGE_NONE][0]['safe_value'] = $value;      
      break;
     
     case "text_long":
       $return_field[LANGUAGE_NONE][0]['value'] = $value;
       $return_field[LANGUAGE_NONE][0]['format'] = 'full_html';
       $return_field[LANGUAGE_NONE][0]['safe_value'] = $value;
       break;
      
     case "text_with_summary":
       $return_field[LANGUAGE_NONE][0]['value'] = $value;
       $return_field[LANGUAGE_NONE][0]['format'] = 'full_html';
       $return_field[LANGUAGE_NONE][0]['safe_value'] = $value;
       $return_field[LANGUAGE_NONE][0]['summary'] = substr($value, 0, 500);
       break;
   
      case 'list':           
      
       break;
       
      case 'taxonomy':
             
        break;         
     
      case 'number':        
        $return_field[LANGUAGE_NONE][0]['value'] = $v;
        break;
     
      case 'commerce_price':            
        $return_field[LANGUAGE_NONE][0]['amount'] = ($value * 100);
        $return_field[LANGUAGE_NONE][0]['currency_code'] = 'USD';
        $return_field[LANGUAGE_NONE][0]['data']['components'] = array();
        break;
      
      case "commerce_product_reference":
        $commerce_product_ids = explode(",", $value);
        if(is_array($commerce_product_ids))
        {
          foreach($commerce_product_ids as $index => $product_id)
          {                        
            $return_field[LANGUAGE_NONE][$index] = array("product_id" => $product_id);                        
          }        
        }
        break;       
  }   
  
  return $return_field;
}


?>
