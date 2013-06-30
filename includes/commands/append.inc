<?php

function courier_connector_command_append($value, $field_name, $parameters, $record) {
  $new_value .= $parameters;
  return $new_value;
}

