<?php

function courier_connector_process_field($value, $field_name, $field_type) {

  $return_field = array();

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
      watchdog('Quickbooks import', print_r($vocabulary, TRUE) . '$value = ' . $value, array(), WATCHDOG_INFO);
      $vid = $vocabulary['vid'];
      $machine_name = $vocabulary['machine_name'];
      if ($machine_name == 'department') {
        $query = db_query('select tid from courier_department_cache where qb_id = :qbid', array(':qbid' => trim($value)));
        $result = $query->fetchObject();
        watchdog('Quickbooks import', print_r($result, TRUE) . '$value = ' . $value, array(), WATCHDOG_CRITICAL);
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
            $value = $tid->tid;
          }
        }
      }

      $return_field[LANGUAGE_NONE][0]['tid'] = (int) $value;
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



