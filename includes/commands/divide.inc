<?php
/**
 * @file
 * Division function
 */

/**
 * @param $value
 * @param $field_name
 * @param $parameters
 * @param $record
 * @return float
 */
function courier_connector_command_divide($value, $field_name, $parameters, $record) {
  $new_value = $value / $parameters;
  return $new_value;
}
