<?php

function courier_connector_command_explode($value, $field_name, $parameters) {
  $parameters = explode(',', $parameters);

  $new_value = explode($parameters[0], $value);

  if (isset($parameters[1])) {
    return $new_value[$parameters[1]];
  }

  return $new_value;
}
