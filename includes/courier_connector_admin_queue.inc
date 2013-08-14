<?php
/**
 * @file
 * The admin section for courier.
 */


/**
 * Builds the conifg form.
 */
function courier_connector_queue_form($form, &$form_state) {

  $form['courier-requests'] = array('#type' => 'vertical_tabs');

  $form['current_requests'] = array(
    '#type' => 'fieldset',
    '#title' => t('Current Requests'),
    '#group' => 'courier-requests',
    '#weight' => -2,
    '#theme' => 'courier_connector_request_table',
    '#tree' => TRUE,
  );

  $result = db_query("SELECT * FROM {courier_requests} WHERE processed <> 1");

  foreach ($result as $row) {
    $form['current_requests']['job_date'][$row->job_id] = array('#markup' => check_plain($row->job_date));

    $form['current_requests']['job_id'][$row->job_id] = array('#markup' => "{$row->job_id}");
    $form['current_requests']['request_method'][$row->job_id] = array('#markup' => "{$row->request_method}");

    $request_data = var_export(unserialize($row->request_data), TRUE);

    $form['current_requests']['request_data'][$row->job_id] = array('#markup' => "{$request_data}");

    $form['current_requests']['remove_job'][$row->job_id] = array(
      '#type' => 'button',
      //'#default_value' => ($row->qb_field == "Custom" ? $row->custom_value : ''),
      '#value' => "Remove"
    );
  }

  $form['submit_read_request'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submit Read Request'),
    '#group' => 'courier-requests',
    '#weight' => -1,
  );

  $form['submit_read_request']['request_method'] = array(
    '#type' => 'select',
    '#title' => t('Request Method'),
    '#default_value' => t('AssemblyItem'),
    '#options' => array(
      "AssemblyItem" => "Assembly Item Request",
      "Customer" => "Customer Request",
      "DiscountItem" => "Discount Item Request",
      "ItemInventory" => "Inventory Item Request",
      "Invoice" => "Invoice Request",
      "NonInventoryItem" => "Non Inventory Item Request",
      "PurchaseOrder" => "Purchase Order Request",
      "ReceivePayment" => "Receive Payment Request",
      "SalesOrder" => "Sales Order Request",
      "SalesReceipt" => "Sales Receipt Request",
      "ServiceItem" => "Service Item Request",
    ),
  );

  $form['submit_read_request']['from_date'] = array(
    '#type' => 'textfield',
    '#title' => t('From Time Modified'),
    '#default_value' => t('1/1/2001'),
    '#size' => 20,
    '#description' => t('Format the date as 1/1/2001 HH:MM:SS or just 1/1/2001'),
  );

  $form['submit_read_request']['ids'] = array(
    '#type' => 'textfield',
    '#title' => t('Specific ID'),
    '#default_value' => t(''),
    '#size' => 100,
    '#description' => t('Specific Item ID to Request'),
  );

  $form['submit_read_request']['desc1'] = array(
    '#type' => 'textfield',
    '#title' => t('Description 1'),
    '#default_value' => t(''),
    '#size' => 100,
    '#description' => t('The description 1 value of the item'),
  );

  $form['submit_read_request']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Request to Queue'),
  );

  $form['submit_write_order_request'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submit Write Order Request'),
    '#group' => 'courier-requests',
    '#weight' => 0,
  );

  $form['submit_write_order_request']['order_type'] = array(
    '#type' => 'select',
    '#title' => t('Order Type'),
    '#default_value' => t('SalesReceipt'),
    '#options' => array(
      "Invoice" => "Invoice",
      "PurchaseOrder" => "Purchase Order",
      "SalesOrder" => "Sales Order",
      "SalesReceipt" => "Sales Receipt",
    ),
  );

  $form['submit_write_order_request']['orders'] = array(
    '#type' => 'fieldset',
    '#group' => 'submit_write_order_request',
    '#title' => t('Select Order(s) to Download'),
    '#theme' => 'courier_connector_order_table',
    '#tree' => TRUE,
  );

  $form['submit_write_order_request']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Request to Queue'),
  );

  $form['submit_write_payment_request'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submit Payment Request'),
    '#group' => 'courier-requests',
    '#weight' => 1,
  );

  $form['submit_write_payment_request']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Request to Queue'),
  );

  $form['submit_write_product_request'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submit Write Product Request'),
    '#group' => 'courier-requests',
    '#weight' => 2,
  );

  $form['submit_write_product_request']['product_type'] = array(
    '#type' => 'select',
    '#title' => t('Product Type'),
    '#default_value' => t('SalesReceipt'),
    '#options' => array(
      "AssemblyItem" => "Assembly Item Request",
      "DiscountItem" => "Discount Item Request",
      "InventoryItem" => "Inventory Item Request",
      "NonInventoryItem" => "Non Inventory Item Request",
      "ServiceItem" => "Service Item Request",
    ),
  );

  $form['submit_write_product_request']['products'] = array(
    '#type' => 'fieldset',
    '#group' => 'submit_write_product_request',
    '#title' => t('Select Products(s) to Download'),
    '#theme' => 'courier_connector_product_table',
    '#tree' => TRUE,
  );

  $form['submit_write_product_request']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Request to Queue'),
  );

  $form['submit_write_customer_request'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submit Write Customer Request'),
    '#group' => 'courier-requests',
    '#weight' => 3,
  );

  $form['submit_write_customer_request']['customers'] = array(
    '#type' => 'fieldset',
    '#group' => 'submit_write_customer_request',
    '#title' => t('Select Customer(s) to Download'),
    '#theme' => 'courier_connector_customer_table',
    '#tree' => TRUE,
  );

  $form['submit_write_customer_request']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Request to Queue'),
  );

  //$form['#validate'][] = 'courier_connector_queue_validate';

  return $form;
}

/**
 * Form validate handler for the courier_quickbooks_config_form.
 */
function courier_connector_queue_validate(&$form_state) {

}

/**
 * Should probably be renamed courier_connector_order_table().
 * @param $variables
 * @return string
 */
function theme_courier_connector_order_table($variables) {
  $form_state = $variables['form'];

  $header = array(t('Select'), t('Order ID'), t('Customer'), t('TOTAL'), t('PURCHASE DATE'), t('STATUS'));

  $rows = array();

  foreach (element_children($form_state['order_id']) as $key) {
    $rows[] = array(
      drupal_render($form_state['add'][$key]),
      drupal_render($form_state['order_id'][$key]),
      drupal_render($form_state['customer'][$key]),
      drupal_render($form_state['total'][$key]),
      drupal_render($form_state['purchase_date'][$key]),
      drupal_render($form_state['status'][$key]),
    );
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows
  ));
  //$output .= drupal_render_children($form_state);

  return $output;
}

/**
 * @param $variables
 * @return string
 */
function theme_courier_connector_product_table($variables) {
  $form_state = $variables['form'];

  $header = array(t('Select'), t('Product ID'), t('Title'), t('price'), t('model'));

  $rows = array();

  foreach (element_children($form_state['nid']) as $key) {
    $rows[] = array(
      drupal_render($form_state['add'][$key]),
      drupal_render($form_state['nid'][$key]),
      drupal_render($form_state['title'][$key]),
      drupal_render($form_state['sell_price'][$key]),
      drupal_render($form_state['model'][$key]),
    );
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows
  ));
  //$output .= drupal_render_children($form_state);

  return $output;
}

/**
 * @param $variables
 * @return string
 */
function theme_courier_connector_customer_table($variables) {
  $form_state = $variables['form'];

  $header = array(t('Select'), t('User ID'), t('Name'), t('Email'), t('Location'));

  $rows = array();

  foreach (element_children($form_state['uid']) as $key) {
    $rows[] = array(
      drupal_render($form_state['add'][$key]),
      drupal_render($form_state['uid'][$key]),
      drupal_render($form_state['name'][$key]),
      drupal_render($form_state['email'][$key]),
      drupal_render($form_state['location'][$key]),
    );
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows
  ));
  //$output .= drupal_render_children($form_state);

  return $output;
}

/**
 * @param $variables
 * @return string
 */
function theme_courier_connector_request_table($variables) {
  $form_state = $variables['form'];

  $header = array(t('Job ID'), t('Request Date'), t('Request Method'), t('Request Data'), t(''));

  $rows = array();

  foreach (element_children($form_state['job_id']) as $key) {
    $rows[] = array(
      drupal_render($form_state['job_id'][$key]),
      drupal_render($form_state['job_date'][$key]),
      drupal_render($form_state['request_method'][$key]),
      drupal_render($form_state['request_data'][$key]),
      drupal_render($form_state['remove_job'][$key]),
    );
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows
  ));

  return $output;
}

/**
 * Implements hook_form_submit().
 */
function courier_connector_queue_form_submit(&$form_state) {
  db_insert('courier_requests')
    ->fields(array(
      'request_type' => 'add',
      'request_method' => 'ItemInventory',
      'request_data' => serialize(array("ItemInventory_ids" => array("all")))
    ))
    ->execute();

  switch ($form_state_state['values']['courier-requests__active_tab']) {
    case "edit-submit-read-request": {

      $request_data = array();

      $request_data['TimeModified'] = $form_state_state['values']['from_date'];
      //$request_data['ToTimeModified'] = $form_state_state['values']['to_date'];
      $request_data['Desc1'] = $form_state_state['values']['desc1'];
      $request_data[$form_state_state['values']['request_method'] . 'ID'] = $form_state_state['values']['ids'];

      db_insert('courier_requests')
        ->fields(array(
          'request_type' => 'read',
          'request_method' => $form_state_state['values']['request_method'],
          'request_data' => serialize($request_data)
        ))
        ->execute();
      break;
    }
    case "edit-submit-write-order-request": {
      $orderids = '';

      foreach ($form_state_state['values']['orders']['add'] as $orderid => $add) {
        if ($add == 1) {
          $orderids .= $orderid . ",";
        }
      }

      break;
    }
    case "edit-submit-write-payment-request": {
      break;
    }
    case "edit-submit-write-product-request": {
      $productids = '';

      foreach ($form_state_state['values']['products']['add'] as $productid => $add) {
        if ($add == 1) {
          $productids .= $productid . ",";
        }
      }

      break;
    }
    case "edit-submit-write-customer-request": {
      $customerids = '';

      foreach ($form_state_state['values']['customers']['add'] as $customerid => $add) {
        if ($add == 1) {
          $customerids .= $customerid . ",";
        }
      }

      break;
    }
  }
}
