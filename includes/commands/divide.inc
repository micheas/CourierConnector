<?php

function courier_quickbooks_pos_command_divide($value, $field_name, $parameters, $record) {
  $new_value = $value / $parameters;
    
  return $new_value;
}

?>
