<?php

/**
 * @file
 * Administrative callbacks for the courier quickbooks module.
 */

function courier_connector_config_form_6(&$form_state) {
  return courier_connector_config_form($form, NULL);
}

/**
 * Builds the conifg form.
 */
function courier_connector_config_form($form, &$form_state) {

  // Put fieldsets into vertical tabs
  $form['courier-settings'] = array('#type' => 'vertical_tabs');

  $form['General Settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configuration'),
    '#weight' => -1,
  );

  $order_type = variable_get('courier_connector_order_type');
  $item_type = variable_get('courier_connector_item_type');

  $form['General Settings']['courier_connector_order_type'] = array(
    '#type' => 'select',
    '#title' => t('QuickBooks Order Type'),
    '#description' => t('The type of order that will be used when orders from the site are downloaded into QuickBooks.'),
    '#options' => array(
      "SalesReceipt" => "Sales Receipt",
      "Invoice" => "Invoice",
      "SalesOrder" => "Sales Order",
      "PurchaseOrder" => "Purchase Order"
    ),
    '#default_value' => 'SalesReceipt',
  );

  if (!is_null($order_type)) {
    $form['General Settings']['courier_connector_order_type']['#value'] = $order_type;
  }

  $form['General Settings']['courier_connector_item_type'] = array(
    '#type' => 'select',
    '#title' => t('QuickBooks Item Type'),
    '#description' => t('The type of item that will be used when products from the site are downloaded into QuickBooks.'),
    '#options' => array(
      "InventoryItem" => "Inventory Item",
      "NonInventoryItem" => "Non Inventory Item",
      "ServiceItem" => "Service Item",
      "DiscountItem" => "Discount Item"
    ),
    '#default_value' => 'InventoryItem',
  );

  if (!is_null($item_type)) {
    $form['General Settings']['courier_connector_item_type']['#value'] = $item_type;
  }

  return system_settings_form($form);
}