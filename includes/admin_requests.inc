<?php

function courier_ubercart_admin_requests($form, &$form_state)
{
    $form['courier-requests'] = array('#type' => 'vertical_tabs');

    $form['current_requests'] = array(
        '#type' => 'fieldset',
        '#title' => t('Current Requests'),
        '#group' => 'courier-requests',
        '#weight' => -2,
        '#theme' => 'courier_ubercart_request_table',
        '#tree' => TRUE,  
    );

    $result = db_query("select * from courier_requests");

    foreach ($result as $row) 
    {         
        $form['current_requests']['job_date'][$row->job_id] = array('#markup' => check_plain($row->job_date));

        $form['current_requests']['request_xml'][$row->job_id] = array(    
                                                        '#type' => 'textarea',    
                                                        '#value' => "{$row->request_xml}",
                                                        );
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
                          "InventoryItem" => "Inventory Item Request",
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
      '#title' => t('From Modified Date'),
      '#default_value' => t('1/1/2001'), 
      '#size' => 20,
      '#description' => t('Format the date as 1/1/2001 HH:MM:SS or just 1/1/2001'),
    ); 
    
    $form['submit_read_request']['to_date'] = array(
      '#type' => 'textfield',
      '#title' => t('To Modified Date'),
      '#default_value' => t(''), 
      '#size' => 20,
      '#description' => t('Format the date as 1/1/2001 HH:MM:SS or just 1/1/2001'),
    ); 
    
    $form['submit_read_request']['active'] = array(
      '#type' => 'select',
      '#title' => t('Active Status'),
      '#default_value' => t(''),
      '#options' => array("" => "Any",
                          "true" => "Active Only",
                          "false" => "Deactive Only",                                            
                         ),      
    ); 
    
    $form['submit_read_request']['ids'] = array(
      '#type' => 'textfield',
      '#title' => t('Specific IDs'),
      '#default_value' => t(''), 
      '#size' => 100,
      '#description' => t('Comma seperated list of specific object ids to request'),
    ); 
    
    $form['submit_read_request']['fullnames'] = array(
      '#type' => 'textfield',
      '#title' => t('Full Names'),
      '#default_value' => t(''), 
      '#size' => 100,
      '#description' => t('Comma seperated list of specific object full names to request'),
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
        '#theme' => 'courier_ubercart_order_table',
        '#tree' => TRUE,
    );

    $result = db_query("select order_id, order_total, created, order_status, concat(billing_first_name, ' ', billing_last_name) as customer from uc_orders");

    $d = array();

    foreach ($result as $row) 
    {                   
        $form['submit_write_order_request']['orders']['add'][$row->order_id] = array(    
                                                                                        '#type' => 'checkbox',                                                                                            
                                                                                    );
        $form['submit_write_order_request']['orders']['order_id'][$row->order_id] = array('#markup' => check_plain($row->order_id));
        $form['submit_write_order_request']['orders']['customer'][$row->order_id] = array('#markup' => check_plain($row->customer));                                                        
        $form['submit_write_order_request']['orders']['total'][$row->order_id] = array('#markup' => check_plain($row->order_total));
        $form['submit_write_order_request']['orders']['purchase_date'][$row->order_id] = array('#markup' => check_plain(gmdate("M d Y H:i:s", $row->created)));
        $form['submit_write_order_request']['orders']['status'][$row->order_id] = array('#markup' => check_plain($row->order_status));
    }

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
        '#theme' => 'courier_ubercart_product_table',
        '#tree' => TRUE,
    );

    $result = db_query("select uc_products.nid, title, `sell_price`, `model` from uc_products inner join node on node.nid = uc_products.nid");

    $d = array();

    foreach ($result as $row) 
    {                   
        $form['submit_write_product_request']['products']['add'][$row->nid] = array(    
                                                                                        '#type' => 'checkbox',                                                                                            
                                                                                    );
        $form['submit_write_product_request']['products']['nid'][$row->nid] = array('#markup' => check_plain($row->nid));
        $form['submit_write_product_request']['products']['title'][$row->nid] = array('#markup' => check_plain($row->title));                                                        
        $form['submit_write_product_request']['products']['sell_price'][$row->nid] = array('#markup' => check_plain($row->sell_price));
        $form['submit_write_product_request']['products']['model'][$row->nid] = array('#markup' => check_plain($row->model));       
    }
    
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
        '#theme' => 'courier_ubercart_customer_table',
        '#tree' => TRUE,
    );

    $result = db_query("select distinct users.uid, concat(uc_orders.billing_first_name, ' ', uc_orders.billing_last_name) as name, mail, concat(billing_city, ', ', uc_zones.zone_name) as location from users inner join uc_orders on uc_orders.uid = users.uid inner join uc_zones on uc_orders.billing_zone = uc_zones.zone_id");

    $d = array();

    foreach ($result as $row) 
    {                   
        $form['submit_write_customer_request']['customers']['add'][$row->uid] = array(    
                                                                                        '#type' => 'checkbox',                                                                                            
                                                                                    );
        $form['submit_write_customer_request']['customers']['uid'][$row->uid] = array('#markup' => check_plain($row->uid));
        $form['submit_write_customer_request']['customers']['name'][$row->uid] = array('#markup' => check_plain($row->name));                                                        
        $form['submit_write_customer_request']['customers']['email'][$row->uid] = array('#markup' => check_plain($row->mail));
        $form['submit_write_customer_request']['customers']['location'][$row->uid] = array('#markup' => check_plain($row->location));        
    }
   
   $form['submit_write_customer_request']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit Request to Queue'),
    );
    

    $form['request_log'] = array(
        '#type' => 'fieldset',
        '#title' => t('Request Log'),
        '#group' => 'courier-requests',
        '#weight' => 4,
          '#theme' => 'courier_ubercart_request_log_table',
        '#tree' => TRUE,  
    ); 

    $result = db_query("select *, courier_responses_log.log_date as response_date from courier_requests_log inner join courier_responses_log on courier_responses_log.job_id = courier_requests_log.job_id");

    foreach ($result as $row) 
    {         
        $form['request_log']['job_date'][$row->job_id] = array('#markup' => check_plain($row->job_date));

        $form['request_log']['request_xml'][$row->job_id] = array(    
                                                        '#type' => 'textarea',    
                                                        '#value' => "{$row->request_xml}",
                                                        );
        $form['request_log']['processed_date'][$row->job_id] = array('#markup' => check_plain($row->log_date));
        $form['request_log']['response_xml'][$row->job_id] = array(    
                                                        '#type' => 'textarea',    
                                                        '#value' => "{$row->response_xml}",
                                                        );        
        $form['request_log']['status'][$row->job_id] = array('#markup' => check_plain($row->status));                                                        

    }

  
   return $form;
}

function theme_courier_ubercart_order_table($variables) 
{     
    $form = $variables['form'];
  
    $header = array(t('Select'), t('Order ID'), t('Customer'), t('TOTAL'), t('PURCHASE DATE'), t('STATUS'));
  
    $rows = array();
  
    foreach (element_children($form['order_id']) as $key) 
    {
        $rows[] = array(
                        drupal_render($form['add'][$key]),
                        drupal_render($form['order_id'][$key]),
                        drupal_render($form['customer'][$key]),
                        drupal_render($form['total'][$key]),        
                        drupal_render($form['purchase_date'][$key]),        
                        drupal_render($form['status'][$key]),                                  
                       );       
    } 

    $output = theme('table', array(
        'header' => $header,
        'rows' => $rows
    ));
    //$output .= drupal_render_children($form);

    return $output;
}

function theme_courier_ubercart_product_table($variables) 
{     
    $form = $variables['form'];
  
    $header = array(t('Select'), t('Product ID'), t('Title'), t('price'), t('model'));
  
    $rows = array();
      
    foreach (element_children($form['nid']) as $key) 
    {
        $rows[] = array(
                        drupal_render($form['add'][$key]),
                        drupal_render($form['nid'][$key]),
                        drupal_render($form['title'][$key]),
                        drupal_render($form['sell_price'][$key]),        
                        drupal_render($form['model'][$key]),                                                                
                       );       
    } 

    $output = theme('table', array(
        'header' => $header,
        'rows' => $rows
    ));
    //$output .= drupal_render_children($form);

    return $output;
}

function theme_courier_ubercart_customer_table($variables) 
{     
    $form = $variables['form'];
  
    $header = array(t('Select'), t('User ID'), t('Name'), t('Email'), t('Location'));
  
    $rows = array();
  
    foreach (element_children($form['uid']) as $key) 
    {
        $rows[] = array(
                        drupal_render($form['add'][$key]),
                        drupal_render($form['uid'][$key]),
                        drupal_render($form['name'][$key]),
                        drupal_render($form['email'][$key]),        
                        drupal_render($form['location'][$key]),                                
                       );       
    } 

    $output = theme('table', array(
        'header' => $header,
        'rows' => $rows
    ));
    //$output .= drupal_render_children($form);

    return $output;
}

function theme_courier_ubercart_request_table($variables) 
{     
   $form = $variables['form'];

   $header = array(t('Request Date'), t('Request XML'), t(''));

   $rows = array();
  
   foreach (element_children($form['job_date']) as $key) {
      $rows[] = array(
        drupal_render($form['job_date'][$key]),
        drupal_render($form['request_xml'][$key]),
        drupal_render($form['remove_job'][$key]),        
      );       
    } 
    
    $output = theme('table', array(
        'header' => $header,
        'rows' => $rows
    ));
   
    return $output;
}

function theme_courier_ubercart_request_log_table($variables) 
{     
   $form = $variables['form'];

   $header = array(t('Request Date'), t('Request XML'), t('Processed Date'), t('Response XML'), t('Status'));

   $rows = array();
  
   foreach (element_children($form['job_date']) as $key) {
      $rows[] = array(
        drupal_render($form['job_date'][$key]),
        drupal_render($form['request_xml'][$key]),               
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

function courier_ubercart_admin_requests_submit($form, &$form_state) 
{   
    switch($form_state['values']['courier-requests__active_tab'])
    {
        case "edit-submit-read-request":
        {            
            drupal_goto("courier/generate_request", array('query'=> array(
                                                            "request_type" => "read",
                                                            "request_method" => $form_state['values']['request_method'],
                                                            "parameters" => "{$form_state['values']['from_date']}~{$form_state['values']['to_date']}~{$form_state['values']['active']}~{$form_state['values']['ids']}~{$form_state['values']['fullnames']}"
                                                         )));
            break;
        }
        case "edit-submit-write-order-request":            
        {
            $orderids = '';
            
            foreach($form_state['values']['orders']['add'] as $orderid => $add)
            {
                if($add == 1)
                {
                    $orderids .= $orderid . ",";
                }
            }
            
            drupal_goto("courier/generate_request", array('query'=> array(
                                                            "request_type" => "write",
                                                            "request_method" => $form_state['values']['order_type'],
                                                            "parameters" => $orderids
                                                         )));
            break;
        }
        case "edit-submit-write-payment-request":
        {
            
            break;
        }
        case "edit-submit-write-product-request":
        {
            $productids = '';
            
            foreach($form_state['values']['products']['add'] as $productid => $add)
            {
                if($add == 1)
                {
                    $productids .= $productid . ",";
                }
            }
            
            drupal_goto("courier/generate_request", array('query'=> array(
                                                            "request_type" => "write",
                                                            "request_method" => $form_state['values']['product_type'],
                                                            "parameters" => $productids
                                                         )));
            break;
        }        
        case "edit-submit-write-customer-request":
        {
            $customerids = '';
            
            foreach($form_state['values']['customers']['add'] as $customerid => $add)
            {
                if($add == 1)
                {
                    $customerids .= $customerid . ",";
                }
            }
            
            drupal_goto("courier/generate_request", array('query'=> array(
                                                            "request_type" => "write",
                                                            "request_method" => "Customer",
                                                            "parameters" => $customerids
                                                         )));
            break;
        }
    }
}

?>