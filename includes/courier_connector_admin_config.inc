<?php

/**
 * @file
 * Administrative callbacks for the courier quickbooks module.
 */

/**
 * Builds the conifg form.
 */
function courier_connector_config_form(&$form_state) {
   
  // Put fieldsets into vertical tabs
  $form_state['courier-settings'] = array('#type' => 'vertical_tabs');

  $form_state['General Settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configuration'),    
    '#weight' => -1,
  );
  
  $form_state['General Settings']['order_type'] = array(
    '#type' => 'select',
    '#title' => t('QuickBooks Order Type'),
    '#description' => t('The type of order that will be used when orders from the site are downloaded into QuickBooks.'),
    '#options' => array("SalesReceipt" => "Sales Receipt", "Invoice" => "Invoice", "SalesOrder" => "Sales Order", "PurchaseOrder" => "Purchase Order"),
    '#default_value' => 'SalesReceipt',
  );
  
  $form_state['General Settings']['item_type'] = array(
    '#type' => 'select',
    '#title' => t('QuickBooks Item Type'),
    '#description' => t('The type of item that will be used when products from the site are downloaded into QuickBooks.'),
    '#options' => array("InventoryItem" => "Inventory Item", "NonInventoryItem" => "Non Inventory Item", "ServiceItem" => "Service Item", "DiscountItem" => "Discount Item"),
    '#default_value' => 'InventoryItem',
  );

  $form_state['#validate'][] = 'courier_quickbooks_config_config_validate';

  return system_settings_form($form_state);
}

/**
 * Form validate handler for the courier_quickbooks_config_form.
 */
function courier_connector_config_form_validate(&$form_state) {
  
}
