<?php

/**
 * @file
 * The admin log -- probably should be rewritten to use watchdog().
 */

/**
 * Builds the log form.
 * @param $form
 * @param $form_state
 * @return mixed system_settings_form($form)
 */
function courier_connector_logs_form($form, &$form_state) {

  //TODO, add a query filter for this logged data

  $form['request_log'] = array(
    '#type' => 'fieldset',
    '#title' => t('Request Log'),
    '#group' => 'courier-requests',
    '#weight' => 4,
    '#theme' => 'courier_connector_request_log_table',
    '#tree' => TRUE,
  );

  $result = db_query("SELECT *, courier_requests.job_id AS 'jobid', courier_requests.job_date AS 'request_date' FROM {courier_requests} courier_requests LEFT JOIN {courier_responses} courier_responses ON courier_requests.job_id = courier_responses.job_id ORDER BY courier_requests.job_date DESC");

  foreach ($result as $row) {
    //convert the database record from obj to array if need be
    if (!is_array($row)) {
      $row = (array) $row;
    }

    $status = 'Error';
    if ($row['processed'] == 1 && $row['status'] == 'True') {
      $status = 'Processed';
    }
    else {
      if ($row['processed'] == 0) {
        $status = 'Queued';
      }
      else {
        if ($row['processed'] == 1) {
          $status = 'Awaiting Response';
        }
      }
    }

    $value_type = "#markup";

    $form['request_log']['job_date'][$row['jobid']] = array($value_type => check_plain($row['request_date']));
    $form['request_log']['request_type'][$row['jobid']] = array($value_type => check_plain($row['request_type']));
    $form['request_log']['job_date'][$row['jobid']] = array($value_type => check_plain($row['request_date']));
    $form['request_log']['request_type'][$row['jobid']] = array($value_type => check_plain($row['request_type']));
    $form['request_log']['status'][$row['jobid']] = array($value_type => check_plain($status));
    $form['request_log']['processed_date'][$row['jobid']] = array($value_type => check_plain($row['processed_date']));

    $request_data = var_export(unserialize($row['request_data']), TRUE);
    $form['request_log']['request_data'][$row['jobid']] = array(
      '#type' => 'textarea',
      '#value' => "{$request_data}",
    );
    $form['request_log']['response_xml'][$row['jobid']] = array(
      '#type' => 'textarea',
      '#value' => "{$row['response_xml']}",
    );
  }

  //return system_settings_form($form);
  return $form;
}

/**
 * @param $form
 * @return string
 */
function theme_courier_connector_request_log_table($form) {
  $form = $form['form'];

  $rows = array();

  $header = array(
    t('Request Date'),
    t('Request Type'),
    t('Request Data'),
    t('Processed Date'),
    t('Response XML'),
    t('Status')
  );

  foreach (element_children($form['job_date']) as $key) {
    $rows[] = array(
      drupal_render($form['job_date'][$key]),
      drupal_render($form['request_type'][$key]),
      drupal_render($form['request_data'][$key]),
      drupal_render($form['processed_date'][$key]),
      drupal_render($form['response_xml'][$key]),
      drupal_render($form['status'][$key])
    );
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows
  ));

  return $output;
}
