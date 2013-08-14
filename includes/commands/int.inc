<?php
/**
 * @file
 * Converts a value to an int, only useful in QB->drupal
 */

/**
 * @param $value
 * @return string
 */
function courier_connector_command_int($value) {
  return (int) $value;
}
