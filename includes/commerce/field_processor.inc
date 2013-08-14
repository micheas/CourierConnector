<?php
/**
 * @file
 * Process the individual fields.
 */

/**
 * @param $value
 * @param $field_name
 * @param $field_type
 * @return array|null  Should this return array() instead of NULL?
 */
function courier_connector_process_field($value, $field_name, $field_type) {
  $debug = FALSE;
  $return_field = array();

  set_time_limit(30);

  switch ($field_type) {

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
      break;

    case 'list':

      break;

    case 'taxonomy_term_reference':

      $vocabulary = courier_connector_get_vocabulary($field_name);
      $original_value = $value;
      if ($debug === TRUE) {
        watchdog('Quickbooks import', print_r($vocabulary, TRUE) . '$value = ' . $value, array(), WATCHDOG_INFO);
      }
      $vid = $vocabulary['vid'];
      $machine_name = $vocabulary['machine_name'];
      if (trim($original_value) == '') {
        $return_field = NULL;
      }
      else {
        if ($machine_name == 'department') {
          $query = db_query('SELECT tid FROM {courier_department_cache} WHERE qb_id = :qbid', array(':qbid' => $value));
          $result = $query->fetchObject();
          if ($debug === TRUE) {
            watchdog('Quickbooks import', '$value = ' . $original_value . ' resulted in: ' . print_r($result, TRUE), array(), WATCHDOG_DEBUG);
          }
          $value = $result->tid;
        }
        else {
          $term = taxonomy_get_term_by_name($value, $machine_name);

          if (count($term) === 0) {
            $term = new stdClass();
            $term->name = $value;
            $term->vid = $vid;
            taxonomy_term_save($term);
            $value = $term->tid;
          }
          else {
            foreach ($term as $tid) {
              watchdog('Quickbooks import', '$value = ' . $original_value . ' resulted in: ' . print_r($tid, TRUE), array(), WATCHDOG_NOTICE);
              $value = $tid->tid;
            }
          }
        }

        $return_field[LANGUAGE_NONE][0]['tid'] = $value;
      }
      break;

    case 'number':
      $return_field[LANGUAGE_NONE][0]['value'] = $value;
      break;

    case 'number_decimal':
      $return_field[LANGUAGE_NONE][0]['value'] = $value;
      break;

    case 'commerce_price':
      $return_field[LANGUAGE_NONE][0]['amount'] = ($value * 100);
      $return_field[LANGUAGE_NONE][0]['currency_code'] = 'USD';
      $return_field[LANGUAGE_NONE][0]['data']['components'] = array();
      break;

    case "commerce_product_reference":
      $commerce_product_ids = explode(",", $value);
      if (is_array($commerce_product_ids)) {
        foreach ($commerce_product_ids as $index => $product_id) {
          $return_field[LANGUAGE_NONE][$index] = array("product_id" => $product_id);
        }
      }
      break;
  }

  return $return_field;
}



