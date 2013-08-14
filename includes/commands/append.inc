<?php
/**
 * @file
 * Strangely, seems to just set the value to the parameters.
 */

/**
 * @param $value
 * @param $field_name
 * @param $parameters
 * @param $record
 * @return string
 */
function courier_connector_command_append($value, $field_name, $parameters, $record) {
  $new_value .= $parameters;
  return $new_value;
}

