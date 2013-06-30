<?php

/**
 * Wrapper function for D6
 * @param $form_state
 * @return mixed
 */
function courier_connector_logs_form_6(&$form_state) {
  return courier_connector_logs_form($form_state, NULL);
}

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

  $result = db_query("select *, courier_requests.job_id as 'jobid', courier_requests.job_date as 'request_date' from courier_requests left join courier_responses on courier_requests.job_id = courier_responses.job_id order by courier_requests.job_date desc");

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
    if (courier_connector_get_drupal_version() == 6) {
      $value_type = "#value";
    }

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
  if (courier_connector_get_drupal_version() == '7') {
    $form = $form['form'];
  }

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

  if (courier_connector_get_drupal_version() == '6') {
    $output = theme('table', $header, $rows);
  }
  else {
    $output = theme('table', array(
      'header' => $header,
      'rows' => $rows
    ));
  }

  return $output;
}