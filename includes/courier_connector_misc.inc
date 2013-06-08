<?php

require_once 'courier_connector_rest_server.inc';

function get_courier_menu() {  
  $items = array();

//  $items['courier/update'] = array(    
//    'page callback' => 'courier_update', 
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/update.inc',
//  );
//  
//   $items['courier/update.php'] = array(    
//    'page callback' => 'courier_update', 
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/update.inc',
//  );
//  
//  
//  $items['courier/generate_request'] = array(    
//    'page callback' => 'generate_request', 
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/request_handler.inc',
//  );   
//  
//  $items['admin/config/courier'] = array(    
//    'title' => 'Courier Settings',
//    'page callback' => 'drupal_get_form', 
//    'page arguments' => array('courier_connector_admin'),
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/admin.inc',
//  );
//  
//  $items['admin/config/courier/mapping/product'] = array( 
//    'title' => 'Courier Product Mapping',
//    'page callback' => 'courier_connector_admin_mapping_product', 
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/admin_mapping.inc',
//  );
//  
//  $items['admin/config/courier/mapping/customer'] = array(    
//    'title' => 'Courier User Mapping',
//    'page callback' => 'courier_connector_admin_mapping_user', 
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/admin_mapping.inc',
//  );
//  
//  $items['admin/config/courier/request'] = array(    
//    'title' => 'Courier Requests',
//    'page callback' => 'drupal_get_form', 
//    'page arguments' => array('courier_connector_admin_requests'),
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/admin_requests.inc',
//  );
//  
//   $items['courier'] = array(    
//    'title' => '',
//    'page callback' => 'courier_connector_courier_ws',     
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/webservice/courier_ws.php',
//  );
  $items['admin/store/integration'] = array(
    'title' => 'QuickBooks Integration',
    'description' => 'Administer your QuickBooks Integration.',
    'page callback' => 'system_admin_menu_block_page',
    'access arguments' => array('access administration pages'),
    'file path' => drupal_get_path('module', 'system'),
    'file' => 'system.admin.inc',
    'weight' => -7,
  );
  
  $items['admin/store/integration/config'] = array(
    'title' => 'Configuration',
    'description' => 'Configure settings for your QuickBooks Integration.',
    'page callback' => 'drupal_get_form',
    'access arguments' => array('access administration pages'),    
    'type' => MENU_NORMAL_ITEM,
    'weight' => 50,
    'file' => 'includes/courier_connector_admin_config.inc',
  );
  
  if(courier_connector_get_drupal_version() == 6) {
    $items['admin/store/integration/config']['page arguments'] = array('courier_connector_config_form_6');
  }
  else {
    $items['admin/store/integration/config']['page arguments'] = array('courier_connector_config_form');
  }
  
  
  $items['admin/store/integration/log'] = array(
    'title' => 'Logs',
    'description' => 'Communication logs.',
    'page callback' => 'drupal_get_form',
    'access arguments' => array('access administration pages'),    
    'type' => MENU_NORMAL_ITEM,
    'weight' => 50,    
    'file' => 'includes/courier_connector_admin_log.inc',
  );
  
  if(courier_connector_get_drupal_version() == 6) {
    $items['admin/store/integration/log']['page arguments'] = array('courier_connector_logs_form_6');
  }
  else {
    $items['admin/store/integration/log']['page arguments'] = array('courier_connector_logs_form');
  }
  
//  $items['admin/store/integration/mapping'] = array(
//    'title' => 'Field Mappings for the integration.',
//    'description' => 'Integration Mappings.',
//    'page callback' => 'drupal_get_form',
//    'access arguments' => array('access administration pages'),
//    'page arguments' => array('courier_connector_mapping_form'),
//    'type' => MENU_NORMAL_ITEM,
//    'weight' => 50,
//    'file' => 'includes/courier_connector_admin_mapping.inc',
//  );
  
  $items['admin/store/integration/command_queue'] = array(
    'title' => 'Queue Command',
    'description' => 'Queue commands to be processed by the integration.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('courier_connector_queue_form'),
    'access arguments' => array('access administration pages'),
    'type' => MENU_NORMAL_ITEM,
    'weight' => 50,
    'file' => 'includes/courier_connector_admin_queue.inc',
  );
  
//  $items['courier_connector/update'] = array(    
//    'page callback' => 'courier_connector_courier_update', 
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/courier_connector_update.inc',
//  );
//  
//  $items['courier_connector/generate_request'] = array(    
//    'page callback' => 'courier_connector_generate_request', 
//    'access arguments' => array('access content'),
//    'type' => MENU_CALLBACK,
//    'file' => 'includes/courier_connector_request_handler.inc',
//  );   
  return $items;
}

//custom rules exposed
function get_courier_connector_rules_action_info() {
  $actions['download_item'] = array(
    'arguments' => array(          
      'date_modified' => array(
        'type' => 'date',
        'label' => t('From Modified Date'),
        'save' => true
      ),
    ),
    'group' => t('QuickBooks'),     
    'module' => t('QuickBooks'),
    'label' => t('Download item(s) into Quickbooks'),    
    'base' => 'courier_connector_download_items',  
  );
  
  $actions['read_items'] = array(
    'arguments' => array(          
      'date_modified' => array(
        'type' => 'date',
        'label' => t('From Modified Date'),
        'save' => true
      ),
    ),
    'group' => t('QuickBooks'),     
    'module' => t('QuickBooks'),
    'label' => t('Read item(s) from Quickbooks'),    
    'base' => 'courier_connector_read_items',  
  );
  
  $actions['read_customers'] = array(
    'arguments' => array(          
      'date_modified' => array(
        'type' => 'date',
        'label' => t('From Modified Date'),
        'save' => true
      ),
    ),
    'group' => t('QuickBooks'),     
    'module' => t('QuickBooks'),
    'label' => t('Read customers from Quickbooks'),    
    'base' => 'courier_connector_read_customers',  
  );
    
  return $actions;
}

function get_courier_connector_services_resources() {
 return array(
  'CourierConnector' => array(          
    'index' => array(
      'help' => 'Retrieves open requests',      
      'file' => array('type' => 'inc', 'module' => 'courier_connector_uber_7', 'name' => 'includes/courier_connector_retrieve_requests'),      
      'callback' => 'courier_connector_retrieve_requests',
      'access callback' => 'courier_connector_access',
      'access arguments' => array('view'),
      'access arguments append' => TRUE,
      'args' => array(              
      ),
    ),
    'update' => array(
      'help' => 'Calls the processing of a pending response',      
      'file' => array('type' => 'inc', 'module' => 'courier_connector_uber_7', 'name' => 'includes/courier_connector_process_responses'),      
      'callback' => 'courier_connector_process_response',
      'access callback' => 'courier_connector_access',
      'access arguments' => array('view'),
      'access arguments append' => TRUE,
      'args' => array(   
          array(
               'name' => 'job_id',
               'optional' => FALSE,
               'source' => array('path' => '0'),
               'description' => '',
               'type' => 'int',
             ), 
      ),
    ),  
    'create' => array(
      'help' => 'Create a response to a request',      
      'file' => array('type' => 'inc', 'module' => 'courier_connector_uber_7', 'name' => 'includes/courier_connector_create_responses'),      
      'callback' => 'courier_connector_create_responses',
      'access callback' => 'courier_connector_access',
      'access arguments' => array('create'),
      'access arguments append' => TRUE,
      'args' => array( 
        array(
               'name' => 'response_xml',
               'optional' => FALSE,
               'source' => 'data',
               'description' => '',
               'type' => 'text',
             ), 
        array(
               'name' => 'job_id',
               'optional' => FALSE,
               'source' => array('path' => '0'),
               'description' => '',
               'type' => 'int',
             ),
        array(
               'name' => 'job_date',
               'optional' => FALSE,
               'source' => array('path' => '1'),
               'description' => '',
               'type' => 'text',
             ),          
        array(
               'name' => 'status',
               'optional' => FALSE,
               'source' => array('path' => '2'),
               'description' => '',
               'type' => 'text',
             ),
        array(
               'name' => 'record_count',
               'optional' => FALSE,
               'source' => array('path' => '3'),
               'description' => '',
               'type' => 'text',
             ),
        array(
               'name' => 'number_of_chunks',
               'optional' => FALSE,
               'source' => array('path' => '4'),
               'description' => '',
               'type' => 'int',
             ),
        array(
               'name' => 'chunk_number',
               'optional' => FALSE,
               'source' => array('path' => '5'),
               'description' => '',
               'type' => 'int',
             ),
      ),
    ),            
  ),
 );
}

function get_courier_connector_rest_server_request_parsers_alter(&$parsers) {
  $parsers['text/xml'] = "CourierXMLResponseParser::parseXML";
}
 

function get_courier_connector_rest_server_response_formatters_alter(&$formatters) {
  $formatters['xml']['view'] = 'RESTServerViewCourier';
}

/**
  * Implements hook_permission().
  */
function get_courier_connector_permission() {
 return array(
     'get courier connector requests' => array(
      'title' => t('Get Open courier connector Requests'), 
      'description' => t('Get any requests that have yet to be processed by courier connector'),
     ),
     'create courier connector responses' => array(
      'title' => t('Create the response from courier connector requests'), 
      'description' => t('Create the response record from the courier connector request')     
     ),    
 );
}

/**
* Access callback for the get requests resource.
*
* @param string $op
*  The operation that's going to be performed.
* @param array $args
*  The arguments that will be passed to the callback.
* @return bool
*  Whether access is given or not.
*/
function courier_connector_access($op, $args) {
 global $user;
 $access = true;

 switch ($op) {
   case 'view':     
     $access = user_access('get courier connector requests');     
     break;
   case 'create':     
     $access = user_access('create courier connector responses');     
     break;
 }
 $access = true;
 return $access;
}

function get_courier_connector_theme($existing, $type, $theme, $path) {
    $theme_hooks = array(
       'courier_connector_request_table' => array(
       'render element' => 'form',
       'file' => 'includes/courier_connector_admin_queue.inc',
        ), 
//       'courier_connector_product_mapping_table' => array(
//          'render element' => 'form',
//          'file' => 'includes/admin.inc',
//        ), 
//        'courier_connector_order_table' => array(
//           'render element' => 'form',
//           'file' => 'includes/admin_requests.inc',
//        ),
//        'courier_connector_product_table' => array(
//           'render element' => 'form',
//           'file' => 'includes/admin_requests.inc',
//        ),
//        'courier_connector_customer_table' => array(
//           'render element' => 'form',
//           'file' => 'includes/admin_requests.inc',
//        ),
//         'courier_connector_order_mapping_table' => array(
//       'render element' => 'form',
//       'file' => 'includes/admin.inc',
//        ),
//        'courier_connector_request_table' => array(
//       'render element' => 'form',
//       'file' => 'includes/admin_requests.inc',
//        ),
       'courier_connector_request_log_table' => array(
         'render element' => 'form',
         'file' => 'includes/courier_connector_admin_log.inc',
        ),
//        'courier_connector_customer_mapping_table' => array(
//       'render element' => 'form',
//       'file' => 'includes/admin.inc',
//        )  
        
//        'courier_connector_order_table' => array(
//           'render element' => 'form',
//           'file' => 'includes/admin_requests.inc',
//        ),
//        'courier_connector_product_table' => array(
//           'render element' => 'form',
//           'file' => 'includes/admin_requests.inc',
//        ),
//        'courier_connector_customer_table' => array(
//           'render element' => 'form',
//           'file' => 'includes/admin_requests.inc',
//        ),
//         'courier_connector_order_mapping_table' => array(
//       'render element' => 'form',
//       'file' => 'includes/admin.inc',
//        ),
//        'courier_connector_request_table' => array(
//       'render element' => 'form',
//       'file' => 'includes/admin_requests.inc',
//        ),
//       'courier_connector_request_log_table' => array(
//       'render element' => 'form',
//       'file' => 'includes/admin_requests.inc',
//        ),
//        'courier_connector_customer_mapping_table' => array(
//       'render element' => 'form',
//       'file' => 'includes/admin.inc',
//        )
    );
            
    return $theme_hooks;
}

function courier_connector_get_drupal_version() {
  preg_match("/(.*?)\.../", VERSION, $matches);
  
  if(is_array($matches))  
    return $matches[1];
  
  return 6;
}