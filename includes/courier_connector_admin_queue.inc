<?php

function courier_connector_queue_form_6(&$form_state) {
  courier_connector_queue_form($form_state, null);
}
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

  $result = db_query("select * from courier_requests where processed != 1");

  foreach ($result as $row) 
  {         
      $form['current_requests']['job_date'][$row->job_id] = array('#markup' => check_plain($row->job_date));

      $form['current_requests']['job_id'][$row->job_id] = array('#markup' => "{$row->job_id}");
      $form['current_requests']['request_method'][$row->job_id] = array('#markup' => "{$row->request_method}");
      
      $request_data = var_export(unserialize($row->request_data), true);
      
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

//    $form['submit_request']['request_type'] = array(
//      '#type' => 'select',
//      '#title' => t('Request Type'),
//      '#default_value' => t('read'),
//      '#options' => array("read" => "Read Request", "write" => "Write Request"),       
//    ); 

  $form['submit_read_request']['request_method'] = array(
    '#type' => 'select',
    '#title' => t('Request Method'),
    '#default_value' => t('AssemblyItem'),
    '#options' => array("AssemblyItem" => "Assembly Item Request",
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

//  $form['submit_read_request']['to_date'] = array(
//    '#type' => 'textfield',
//    '#title' => t('To Time Modified'),
//    '#default_value' => t(''), 
//    '#size' => 20,
//    '#description' => t('Format the date as 1/1/2001 HH:MM:SS or just 1/1/2001'),
//  ); 

//  $form['submit_read_request']['active'] = array(
//    '#type' => 'select',
//    '#title' => t('Active Status'),
//    '#default_value' => t(''),
//    '#options' => array("" => "Any",
//                        "true" => "Active Only",
//                        "false" => "Deactive Only",                                            
//                       ),      
//  ); 

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
    '#options' => array("Invoice" => "Invoice",                          
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

  //$result = db_query("select order_id, order_total, created, order_status, concat(billing_first_name, ' ', billing_last_name) as customer from uc_orders");

//  $d = array();
//
//  foreach ($result as $row) 
//  {                   
//      $form['submit_write_order_request']['orders']['add'][$row->order_id] = array(    
//                                                                                      '#type' => 'checkbox',                                                                                            
//                                                                                  );
//      $form['submit_write_order_request']['orders']['order_id'][$row->order_id] = array('#markup' => check_plain($row->order_id));
//      $form['submit_write_order_request']['orders']['customer'][$row->order_id] = array('#markup' => check_plain($row->customer));                                                        
//      $form['submit_write_order_request']['orders']['total'][$row->order_id] = array('#markup' => check_plain($row->order_total));
//      $form['submit_write_order_request']['orders']['purchase_date'][$row->order_id] = array('#markup' => check_plain(gmdate("M d Y H:i:s", $row->created)));
//      $form['submit_write_order_request']['orders']['status'][$row->order_id] = array('#markup' => check_plain($row->order_status));
//  }

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
    '#options' => array("AssemblyItem" => "Assembly Item Request",                          
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

//  $result = db_query("select uc_products.nid, title, `sell_price`, `model` from uc_products inner join node on node.nid = uc_products.nid");
//
//  $d = array();
//
//  foreach ($result as $row) 
//  {                   
//      $form['submit_write_product_request']['products']['add'][$row->nid] = array(    
//                                                                                      '#type' => 'checkbox',                                                                                            
//                                                                                  );
//      $form['submit_write_product_request']['products']['nid'][$row->nid] = array('#markup' => check_plain($row->nid));
//      $form['submit_write_product_request']['products']['title'][$row->nid] = array('#markup' => check_plain($row->title));                                                        
//      $form['submit_write_product_request']['products']['sell_price'][$row->nid] = array('#markup' => check_plain($row->sell_price));
//      $form['submit_write_product_request']['products']['model'][$row->nid] = array('#markup' => check_plain($row->model));       
//  }

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

//  $result = db_query("select distinct users.uid, concat(uc_orders.billing_first_name, ' ', uc_orders.billing_last_name) as name, mail, concat(billing_city, ', ', uc_zones.zone_name) as location from users inner join uc_orders on uc_orders.uid = users.uid inner join uc_zones on uc_orders.billing_zone = uc_zones.zone_id");
//
//  $d = array();
//
//  foreach ($result as $row) 
//  {                   
//      $form['submit_write_customer_request']['customers']['add'][$row->uid] = array(    
//                                                                                      '#type' => 'checkbox',                                                                                            
//                                                                                  );
//      $form['submit_write_customer_request']['customers']['uid'][$row->uid] = array('#markup' => check_plain($row->uid));
//      $form['submit_write_customer_request']['customers']['name'][$row->uid] = array('#markup' => check_plain($row->name));                                                        
//      $form['submit_write_customer_request']['customers']['email'][$row->uid] = array('#markup' => check_plain($row->mail));
//      $form['submit_write_customer_request']['customers']['location'][$row->uid] = array('#markup' => check_plain($row->location));        
//  }

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

function theme_courier_connector_order_table($variables) 
{     
    $form_state = $variables['form'];

    $header = array(t('Select'), t('Order ID'), t('Customer'), t('TOTAL'), t('PURCHASE DATE'), t('STATUS'));

    $rows = array();

    foreach (element_children($form_state['order_id']) as $key) 
    {
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

function theme_courier_connector_product_table($variables) 
{     
    $form_state = $variables['form'];

    $header = array(t('Select'), t('Product ID'), t('Title'), t('price'), t('model'));

    $rows = array();

    foreach (element_children($form_state['nid']) as $key) 
    {
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

function theme_courier_connector_customer_table($variables) 
{     
    $form_state = $variables['form'];

    $header = array(t('Select'), t('User ID'), t('Name'), t('Email'), t('Location'));

    $rows = array();

    foreach (element_children($form_state['uid']) as $key) 
    {
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

function theme_courier_connector_request_table($variables) 
{     
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
//
//function theme_courier_connector_request_log_table($variables) 
//{     
//   $form_state = $variables['form'];
//
//   $header = array(t('Request Date'), t('Request XML'), t('Processed Date'), t('Response XML'), t('Status'));
//
//   $rows = array();
//
//   foreach (element_children($form_state['job_date']) as $key) {
//      $rows[] = array(
//        drupal_render($form_state['job_date'][$key]),
//        drupal_render($form_state['request_xml'][$key]),               
//        drupal_render($form_state['processed_date'][$key]),
//        drupal_render($form_state['response_xml'][$key]),        
//        drupal_render($form_state['status'][$key])
//
//      );       
//    } 
//
//    $output = theme('table', array(
//        'header' => $header,
//        'rows' => $rows
//    ));
//
//    return $output;
//}

function courier_connector_queue_form_submit(&$form_state) 
{   
    db_insert('courier_requests') 
              ->fields(array(
                'request_type' => 'add',
                'request_method' => 'ItemInventory',
                'request_data' => serialize(array("ItemInventory_ids"=>array("all"))) 
              ))
              ->execute();
  
    switch($form_state_state['values']['courier-requests__active_tab'])
    {
        case "edit-submit-read-request":
        {            
//            drupal_goto("courier/generate_request", array('query'=> array(
//                                                            "request_type" => "read",
//                                                            "request_method" => $form_state_state['values']['request_method'],
//                                                            "parameters" => "{$form_state_state['values']['from_date']}~{$form_state_state['values']['to_date']}~{$form_state_state['values']['active']}~{$form_state_state['values']['ids']}~{$form_state_state['values']['fullnames']}"
//                                                         )));
          //courier_connector_generate_read_request($form_state_state['values']['request_method'], "{$form_state_state['values']['from_date']}~{$form_state_state['values']['to_date']}~{$form_state_state['values']['active']}~{$form_state_state['values']['ids']}~{$form_state_state['values']['fullnames']}");
          $request_data = array();
                      
          $request_data['TimeModified'] = $form_state_state['values']['from_date'];
          //$request_data['ToTimeModified'] = $form_state_state['values']['to_date'];
          $request_data['Desc1'] = $form_state_state['values']['desc1'];
          $request_data[$form_state_state['values']['request_method'] . 'ID'] = $form_state_state['values']['ids'];
          //$request_data['ActiveStatus'] = $form_state_state['values']['active'];                                                  
          //$request_data[$form_state_state['values']['request_method'] . 'IDs']['key'] =$form_state_state['values']['request_method'] . 'ID';
          //$request_data[$form_state_state['values']['request_method'] . 'IDs'] = array_merge($request_data[$form_state_state['values']['request_method'] . 'IDs'], explode(',', $form_state_state['values']['ids']));                  
          //$request_data['FullNames']['key'] = 'FullName';                    
          //$request_data['FullNames'] = array_merge($request_data['FullNames'], explode(',', $form_state_state['values']['fullnames']));
          
          
          db_insert('courier_requests') 
              ->fields(array(
                'request_type' => 'read',
                'request_method' => $form_state_state['values']['request_method'],
                'request_data' => serialize($request_data) 
              ))
              ->execute();
          break;
        }
        case "edit-submit-write-order-request":            
        {
            $orderids = '';

            foreach($form_state_state['values']['orders']['add'] as $orderid => $add)
            {
                if($add == 1)
                {
                    $orderids .= $orderid . ",";
                }
            }

//            drupal_goto("courier/generate_request", array('query'=> array(
//                                                            "request_type" => "write",
//                                                            "request_method" => $form_state_state['values']['order_type'],
//                                                            "parameters" => $orderids
//                                                         )));
            break;
        }
        case "edit-submit-write-payment-request":
        {

            break;
        }
        case "edit-submit-write-product-request":
        {
            $productids = '';

            foreach($form_state_state['values']['products']['add'] as $productid => $add)
            {
                if($add == 1)
                {
                    $productids .= $productid . ",";
                }
            }

//            drupal_goto("courier/generate_request", array('query'=> array(
//                                                            "request_type" => "write",
//                                                            "request_method" => $form_state_state['values']['product_type'],
//                                                            "parameters" => $productids
//                                                         )));
            break;
        }        
        case "edit-submit-write-customer-request":
        {
            $customerids = '';

            foreach($form_state_state['values']['customers']['add'] as $customerid => $add)
            {
                if($add == 1)
                {
                    $customerids .= $customerid . ",";
                }
            }

//            drupal_goto("courier/generate_request", array('query'=> array(
//                                                            "request_type" => "write",
//                                                            "request_method" => "Customer",
//                                                            "parameters" => $customerids
//                                                         )));
            break;
        }
    }
}