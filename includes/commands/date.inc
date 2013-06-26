<?php

function courier_connector_command_date($value, $field_name, $parameters, $record) {
 
  $new_value = date($parameters, $value);
  
  return $new_value;
}
?>
