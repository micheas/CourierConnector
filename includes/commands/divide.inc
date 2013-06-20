<?php

function courier_connector_command_divide($value, $field_name, $parameters, $record) {
  $new_value = $value / $parameters;    
  return $new_value;
}


