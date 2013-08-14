<?php

/**
 * @file
 * converts the date.
 */

/**
 * @param $value
 * @param $field_name
 * @param $parameters
 * @param $record
 * @return bool|string
 */
function courier_connector_command_date($value, $field_name, $parameters, $record) {

  $new_value = date($parameters, $value);

  return $new_value;
}
