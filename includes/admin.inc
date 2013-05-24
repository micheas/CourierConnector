<?php

function courier_ubercart_admin($form, &$form_state) {

  // Put fieldsets into vertical tabs
  $form['courier-settings'] = array('#type' => 'vertical_tabs');

//  $form['General Settings'] = array(
//    '#type' => 'fieldset',
//    '#title' => t('General settings'),
//    '#group' => 'courier-settings',
//    '#weight' => -1,
//  );
//  
//  $form['General Settings']['order_type'] = array(
//    '#type' => 'select',
//    '#title' => t('QuickBooks order type'),
//    '#description' => t('The type of order that will be used when orders from the site are downloaded into QuickBooks.'),
//    '#options' => array("SalesReceipt" => "Sales Receipt", "Invoice" => "Invoice", "SalesOrder" => "Sales Order", "PurchaseOrder" => "Purchase Order"),
//    '#default_value' => 'SalesReceipt',
//  );
//  
//  $form['General Settings']['item_type'] = array(
//    '#type' => 'select',
//    '#title' => t('QuickBooks item type'),
//    '#description' => t('The type of item that will be used when products from the site are downloaded into QuickBooks.'),
//    '#options' => array("InventoryItem" => "Inventory Item", "NonInventoryItem" => "Non Inventory Item", "ServiceItem" => "Service Item", "DiscountItem" => "Discount Item"),
//    '#default_value' => 'InventoryItem',
//  );

  $form['product_mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Product Mapping'),
    '#group' => 'courier-settings',
    '#weight' => 1, 
    '#theme' => 'courier_ubercart_product_mapping_table',
    '#tree' => TRUE,
  );
  
  $result = db_query("select * from courier_product_mapping");
      
  $uc_item_fields = courier_ubercart_get_uc_item_fields();
  
  $d = array();
  
  foreach ($result as $row) {         
    $form['product_mapping']['prod_uc_field'][$row->uc_field] = array('#markup' => check_plain($uc_item_fields[$row->uc_field]));
    
    $form['product_mapping']['prod_qb_field'][$row->uc_field] = array(    
                                                    '#type' => 'select',    
                                                    '#options' => courier_ubercart_get_qb_item_fields(),
                                                    //'#value' => "{$row->qb_field}",
                                                    '#default_value'=> "{$row->qb_field}"
                                                    );
    $form['product_mapping']['prod_custom_value'][$row->uc_field] = array(    
                                                    '#type' => 'textfield',       
                                                    '#default_value' => ($row->qb_field == "Custom" ? $row->custom_value : ''),
                                                    //'#value' => ($row->qb_field == "Custom" ? $row->custom_value : '')
                                                    ); 
  }

  $form['order_mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Order Mapping'),
    '#group' => 'courier-settings',
    '#weight' => 1,
    '#theme' => 'courier_ubercart_order_mapping_table',
    '#tree' => TRUE,
  );
  
  $result = db_query("select * from courier_order_mapping");
      
  $uc_order_fields = courier_ubercart_get_uc_order_fields();
  
  $d = array();
    
  foreach ($result as $row) {         
    $form['order_mapping']['ord_uc_field'][$row->uc_field] = array('#markup' => check_plain($uc_order_fields[$row->uc_field]));
    
    $form['order_mapping']['ord_qb_field'][$row->uc_field] = array(    
                                                    '#type' => 'select',    
                                                    '#options' => courier_ubercart_get_qb_order_fields(),
                                                    //'#value' => "{$row->qb_field}",
                                                    '#default_value'=> "{$row->qb_field}"
                                                    );
    $form['order_mapping']['ord_custom_value'][$row->uc_field] = array(    
                                                    '#type' => 'textfield',       
                                                    '#default_value' => ($row->qb_field == "Custom" ? $row->custom_value : ''),
                                                    //'#value' => ($row->qb_field == "Custom" ? $row->custom_value : '')
                                                    ); 
  }
  
  $result = db_query("select * from courier_order_product_mapping");
      
  $uc_order_product_fields = courier_ubercart_get_uc_order_product_fields();
  
  $d = array();
  
  foreach ($result as $row) {         
    $form['order_mapping']['ord_product_uc_field'][$row->uc_field] = array('#markup' => check_plain($uc_order_product_fields[$row->uc_field]));
    
    $form['order_mapping']['ord_product_qb_field'][$row->uc_field] = array(    
                                                    '#type' => 'select',    
                                                    '#options' => courier_ubercart_get_qb_order_product_fields(),
                                                    //'#value' => "{$row->qb_field}",
                                                    '#default_value'=> "{$row->qb_field}"
                                                    );
    $form['order_mapping']['ord_product_custom_value'][$row->uc_field] = array(    
                                                    '#type' => 'textfield',       
                                                    '#default_value' => ($row->qb_field == "Custom" ? $row->custom_value : ''),
                                                    //'#value' => ($row->qb_field == "Custom" ? $row->custom_value : '')
                                                    ); 
  }
    
  $form['customer_mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Customer Mapping'),
    '#group' => 'courier-settings',
    '#weight' => 1,
    '#theme' => 'courier_ubercart_customer_mapping_table',
    '#tree' => TRUE,
  );
  
  $result = db_query("select * from courier_customer_mapping");
  
  $uc_cust_fields = courier_ubercart_get_uc_customer_fields();
  
  $d = array();
  
  foreach ($result as $row) {         
    $form['customer_mapping']['cust_uc_field'][$row->uc_field] = array('#markup' => check_plain($uc_cust_fields[$row->uc_field]));
    
    $form['customer_mapping']['cust_qb_field'][$row->uc_field] = array(    
                                                    '#type' => 'select',    
                                                    '#options' => courier_ubercart_get_qb_customer_fields(),
                                                    //'#value' => "{$row->qb_field}",
                                                    '#default_value'=> "{$row->qb_field}"
                                                    );
    $form['customer_mapping']['cust_custom_value'][$row->uc_field] = array(    
                                                    '#type' => 'textfield',       
                                                    '#default_value' => ($row->qb_field == "Custom" ? $row->custom_value : ''),
                                                    //'#value' => ($row->qb_field == "Custom" ? $row->custom_value : '')
                                                    ); 
  }
  
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  
  return $form;
}

function courier_ubercart_admin_submit($form, &$form_state) 
{    
//    print_r($form_state);
//    exit;
    
    db_delete('courier_product_mapping')    
    ->execute();               

    db_delete('courier_customer_mapping')    
    ->execute();        
    
    db_delete('courier_order_mapping')    
    ->execute();        
     
    db_delete('courier_order_product_mapping')    
    ->execute();        
     
    foreach ($form_state['values']['product_mapping']['prod_qb_field'] as $uc_field => $qb_field)        
    {
        $custom = "";
        if($qb_field == "Custom")
        {
            $custom = $form_state['values']['product_mapping']['prod_custom_value'][$uc_field];
        }
        
        //drupal_set_message(t($uc_field . ">" . $qb_field));
        
        db_insert("courier_product_mapping")        
            ->fields(array(                            
                            "uc_field" => $uc_field, 
                            "qb_field" => $qb_field, 
                            "custom_value" => $custom
                          )
                    )->execute();
    }
    
    foreach ($form_state['values']['customer_mapping']['cust_qb_field'] as $uc_field => $qb_field)        
    {
        $custom = "";
        if($qb_field == "Custom")
        {
            $custom = $form_state['values']['customer_mapping']['cust_custom_value'][$uc_field];
        }
       // drupal_set_message(t($uc_field . ">" . $qb_field));
        db_insert("courier_customer_mapping")        
            ->fields(array(                            
                            "uc_field" => $uc_field, 
                            "qb_field" => $qb_field, 
                            "custom_value" => $custom
                          )
                    )->execute();
    }
    
    foreach ($form_state['values']['order_mapping']['ord_qb_field'] as $uc_field => $qb_field)        
    {
        $custom = "";
        if($qb_field == "Custom")
        {
            $custom = $form_state['values']['order_mapping']['ord_custom_value'][$uc_field];
        }
       // drupal_set_message(t($uc_field . ">" . $qb_field));
        db_insert("courier_order_mapping")        
            ->fields(array(                            
                            "uc_field" => $uc_field, 
                            "qb_field" => $qb_field, 
                            "custom_value" => $custom
                          )
                    )->execute();
    }
    
    foreach ($form_state['values']['order_mapping']['ord_product_qb_field'] as $uc_field => $qb_field)        
    {
        $custom = "";
        if($qb_field == "Custom")
        {
            $custom = $form_state['values']['order_mapping']['ord_product_custom_value'][$uc_field];
        }
       // drupal_set_message(t($uc_field . ">" . $qb_field));
        db_insert("courier_order_product_mapping")        
            ->fields(array(                            
                            "uc_field" => $uc_field, 
                            "qb_field" => $qb_field, 
                            "custom_value" => $custom
                          )
                    )->execute();
    }
   
    drupal_set_message(t('Setting Saved.'));
}

function courier_ubercart_admin_validate($form, &$form_state) {
//    print_r($form_state);
//    
//    exit;
//    
//    
//     foreach ($form_state['values']['Product Mapping']['prod_qb_field'] as $uc_field => $qb_field)        
//    {
//         //drupal_set_message(t($uc_field . ">" . $qb_field));
//     }
}

function theme_courier_ubercart_product_mapping_table($variables) {
     
  $form = $variables['form'];
  
  $header = array(t('Ubercart field'), t('QuickBooks field'), t('Custom Value'));
  
  $rows = array();
  
   foreach (element_children($form['prod_uc_field']) as $key) {
      $rows[] = array(
        drupal_render($form['prod_uc_field'][$key]),
        drupal_render($form['prod_qb_field'][$key]),
        drupal_render($form['prod_custom_value'][$key]),        
      );       
    } 
    
    $output = theme('table', array(
        'header' => $header,
        'rows' => $rows
    ));
    //$output .= drupal_render_children($form);

    return $output;
}

function theme_courier_ubercart_order_mapping_table($variables) {
     
  $form = $variables['form'];
  
  $header = array(t('Ubercart field'), t('QuickBooks field'), t('Custom Value'));
  
  $rows = array();
  
   foreach (element_children($form['ord_uc_field']) as $key) {
      $rows[] = array(
        drupal_render($form['ord_uc_field'][$key]),
        drupal_render($form['ord_qb_field'][$key]),
        drupal_render($form['ord_custom_value'][$key]),        
      );       
    } 
    
    $output = theme('table', array(
        'header' => $header,
        'rows' => $rows
    ));
        
    $rows = array();
  
   foreach (element_children($form['ord_product_uc_field']) as $key) {
      $rows[] = array(
        drupal_render($form['ord_product_uc_field'][$key]),
        drupal_render($form['ord_product_qb_field'][$key]),
        drupal_render($form['ord_product_custom_value'][$key]),        
      );       
    } 
    
    $output .= theme('table', array(
        'header' => $header,
        'rows' => $rows
    ));
    
    //$output .= drupal_render_children($form);

    return $output;
}

function theme_courier_ubercart_customer_mapping_table($variables) {
     
  $form = $variables['form'];
  
  $header = array(t('Ubercart field'), t('QuickBooks field'), t('Custom Value'));
  
  $rows = array();
  
   foreach (element_children($form['cust_uc_field']) as $key) {
      $rows[] = array(
        drupal_render($form['cust_uc_field'][$key]),
        drupal_render($form['cust_qb_field'][$key]),
        drupal_render($form['cust_custom_value'][$key]),        
      );       
    } 
    
    $output = theme('table', array(
        'header' => $header,
        'rows' => $rows
    ));
    //$output .= drupal_render_children($form);

    return $output;
}

function courier_ubercart_get_uc_item_fields()
{
    return array(
        "title" => "Item Name",
        "body" => "Description",
        "status" => "Status",
        "promote" => "Promote to Front",        
        "sell_price" => "Sell Price",
        "model" => "Model",
        "weight" => "Weight",
        "weight_units" => "Weight Units",                
        "height" => "Height",
        "height_units" => "Height Units",
        "width" => "Width",        
        "length" => "Length",
        "length_units" => "Length Units",        
        "pkg_qty" => "Package Quantity",
        "default_qty" => "Default Quantity",
        "list_price" => "List Price",
        "cost" => "Cost",
        "shippable" => "Shippable",       
    );           
}
function courier_ubercart_get_qb_item_fields()
{
    return array(
        "Not_Mapped" => "Not Mapped",
        "Name" => "Name",
        "IsActive" => "IsActive",
        "ParentID" => "ParentID",
        "ParentName" => "ParentName",
        "ManufacturerPartNumber" => "ManufacturerPartNumber",
        "UnitOfMeasureSetID" => "UnitOfMeasureSetID",
        "UnitOfMeasureSetName" => "UnitOfMeasureSetName",
        "SalesTaxCodeID" => "SalesTaxCodeID", 
        "SalesTaxCodeName" => "SalesTaxCodeName",
        "SalesDescription" => "SalesDescription",
        "SalePrice" => "SalePrice",
        "IncomeAccountID" => "IncomeAccountID",
        "IncomeAccountName" => "IncomeAccountName",
        "PurchaseDescription" => "PurchaseDescription",
        "PurchaseCost" => "PurchaseCost",
        "CogsAccountName" => "CogsAccountName",
        "CogsAccountName" => "CogsAccountName",
        "PreferredVendorID" => "PreferredVendorID", 
        "PreferredVendorName" => "PreferredVendorName",
        "AssetAccountID" => "AssetAccountID",
        "AssetAccountName" => "AssetAccountName",
        "ReorderPoint" => "ReorderPoint",
        "QuantityOnHand" => "QuantityOnHand",
        "TotalValue" => "TotalValue"
    );
}

function courier_ubercart_get_uc_customer_fields()
{
    return array(
        'name' => 'Customer Name',            
        'pass' => 'Password',           
        'mail' => 'Email',            
        'status' => 'Status',     
        'delivery_first_name' => 'Delivery First Name',   
        'delivery_last_name' => 'Delivery Last Name',   
        'delivery_street1' => 'Delivery Street',   
        'delivery_street2' => 'Delivery Street 2',   
        'delivery_city' => 'Delivery City',   
        'delivery_state_name' => 'Delivery State',   
        'delivery_postal_code' => 'Delivery Postal Code',   
        'delivery_country_name' => 'Delivery Country',   
        'billing_first_name' => 'Billing First Name',   
        'billing_last_name' => 'Billing Last Name',   
        'billing_street1' => 'Billing Street',   
        'billing_street2' => 'Billing Street 2',   
        'billing_city' => 'Billing City',   
        'billing_state_name' => 'Billing State',   
        'billing_postal_code' => 'Billing Postal Code',
        'billing_country_name' => 'Billing Country' 
    );   
}

function courier_ubercart_get_qb_customer_fields()
{
    return array(
        "Not_Mapped" => "Not Mapped",
        'CustomerRecordName' => 'Customer Name',                    
        'FullName' => 'Full Name',            
        'IsActive' => 'Is Active',     
        'ParentName' => 'Parent Full Name',     
        'CompanyName' => 'Company Name',     
        'Salutation' => 'Salutation',     
        'FirstName' => 'First Name',     
        'MiddleName' => 'Middle Name',     
        'LastName' => 'Last Name',             
        'ShippingAddress1' => 'Shipping Street',   
        'ShippingAddress2' => 'Shipping Street 2',   
        'ShippingCity' => 'Shipping City',   
        'ShippingState' => 'Shipping State',   
        'ShippingPostalCode' => 'Shipping Postal Code',   
        'ShippingCountry' => 'Shipping Country',           
        'ShippingNote' => 'Shipping Note',
        'BillingAddress1' => 'Billing Street',   
        'BillingAddress2' => 'Billing Street 2',   
        'BillingCity' => 'Billing City',   
        'BillingState' => 'Billing State',   
        'BillingPostalCode' => 'Billing Postal Code',
        'BillingCountry' => 'Billing Country', 
        'BillingNote' => 'Billing Note', 
        'Phone' => 'Phone',
        'AltPhone' => 'Alternative Phone',
        'Email' => 'Email',
        'Contact' => 'Contact',
        'AltContact' => 'Alternative Contact',
        'CustomerTermsName' => 'Customer Terms Name',
        'SalesRepRefName' => 'Sales Rep Name',
        'Balance' => 'Account Balance',
        'TotalBalance' => 'Account Total Balance',
        'SalesTaxCodeName' => 'Sales Tax Code',
        'ItemSalesTaxName' => 'Item Sales Tax',
        'ResaleNumber' => 'Resale Number',
        'AccountNumber' => 'Account Number',
        'CreditLimit' => 'Credit Limit',
        'PreferredPaymentMethodName' => 'Preferred Payment Method',
        'JobStatus' => 'Job Status',
        'JobStartDate' => 'Job Start Date',
        'JobProjectedEndDate' => 'Job Projected End Date',
        'JobDesc' => 'Job Description',
        'JobTypeRefName' => 'Job Type Name',
        'Notes' => 'Notes',
        'PriceLevelRefName' => 'Price Level Name',
        'CurrencyName' => 'Currency Name',        
    );   
}

function courier_ubercart_get_uc_order_fields()
{
     return array(
        'delivery_first_name' => 'Delivery First Name',   
        'delivery_last_name' => 'Delivery Last Name',   
        'delivery_street1' => 'Delivery Street',   
        'delivery_street2' => 'Delivery Street 2',   
        'delivery_city' => 'Delivery City',   
        'delivery_state_name' => 'Delivery State',   
        'delivery_postal_code' => 'Delivery Postal Code',   
        'delivery_country_name' => 'Delivery Country',   
        'billing_first_name' => 'Billing First Name',   
        'billing_last_name' => 'Billing Last Name',   
        'billing_street1' => 'Billing Street',   
        'billing_street2' => 'Billing Street 2',   
        'billing_city' => 'Billing City',   
        'billing_state_name' => 'Billing State',   
        'billing_postal_code' => 'Billing Postal Code',
        'billing_country_name' => 'Billing Country',
        'order_status' => 'Order Status',           
    );   
}

function courier_ubercart_get_qb_order_fields()
{
     return array(
        "Not_Mapped" => "Not Mapped",
        'CustomerName' => 'Customer Name', 
        'ClassName' => 'Class Name',
        'TransactionDate' => 'Transaction Date',
        'ReferenceNumber' => 'Reference Number',
        'BillingFirstName' => 'Billing First Name',
        'BillingLastName' => 'Billing Last Name',
        'BillingFirstName' => 'Billing First Name',   
        'BillingLastName' => 'Billing Last Name',   
        'BillingStreet' => 'Billing Street',   
        'BillingStreet2' => 'Billing Street 2',   
        'BillingCity' => 'Billing City',   
        'BillingPostalBillingStateCode' => 'Billing State',   
        'BillingPostalCode' => 'Billing Postal Code',
        'BillingCountry' => 'Billing Country',
        'BillingAddressNotes' => 'Billing Address Notes',
        'ShippingFirstName' => 'Shipping First Name',
        'ShippingLastName' => 'Shipping Last Name',
        'ShippingStreet' => 'Shipping Street',
        'ShippingStreet2' => 'Shipping Street 2',        
        'ShippingCity' => 'Shipping City',   
        'ShippingState' => 'Shipping State',   
        'ShippingPostalCode' => 'Shipping Postal Code',   
        'ShippingCountry' => 'Shipping Country',   
        'ShippingAddressNotes' => 'Shipping Address Notes',
        'IsPending' => 'Is Pending',
        'CheckNumber' => 'Check Number',
        'PaymentMethodName' => 'Payment Method Name',
        'ShipmentDate' => 'Shipment Date',
        'Fob' => 'Fob',
        'ItemSalesTaxName' => 'Item Sales Tax Name',
        'Memo' => 'Memo',
        'Other' => 'Other',         
    );   
}

function courier_ubercart_get_uc_order_product_fields()
{
    return array(
        'title' => 'Item Name',   
        'model' => 'Model',   
        'qty' => 'Quantity',   
        'cost' => 'Cost',   
        'price' => 'Price',   
        'weight' => 'Weight',   
        'weight_units' => 'Weight Units',     
    );   
}

function courier_ubercart_get_qb_order_product_fields()
{
    return array(
        "Not_Mapped" => "Not Mapped",        
        'ItemName' => 'Item Name',
        'ItemID' => 'QB Item ID',
        'ItemDescription' => 'Item Description',
        'Quantity' => 'Quantity',
        'UnitOfMeasure' => 'Unit Of Measure',
        'Rate' => 'Rate',
        'RatePercent' => 'Rate Percent',
        'ClassName' => 'Class Name',
        'Amount' => 'Amount',
        'Other1' => 'Other 1',
        'Other2' => 'Other 2',        
    );   
}

//function courier_ubercart_admin()
//{
//    $page = <<< PAGE
//<form name="form1" method="post" action="">
//<table width="479" height="189" border="0">
//  <tr>
//    <td width="198" height="49">QuickBooks order type</td>
//    <td width="271">
//      <select name="order_type" id="order_type">
//        <option value="SalesReceipt" selected>Sales Receipt</option>
//        <option value="Invoice">Invoice</option>
//        <option value="SalesOrder">Sales Order</option>
//        <option value="PurchaseOrder">Purchase Order</option>
//      </select>
//    </td>
//  </tr>
//  <tr>
//    <td height="41">QuickBooks item type</td>
//    <td><select name="item_type" id="item_type">
//      <option value="InventoryItem" selected>Inventory Item</option>
//      <option value="NonInventoryItem">Non Inventory Item</option>
//      <option value="ServiceItem">Service Item</option>
//      <option value="DiscountItem">Discount Item</option>
//    </select>
//    </td>
//  </tr>
//  <tr>
//    <td>Download Payments</td>
//    <td><input type="checkbox" name="download_payments" id="download_payments"></td>
//  </tr>
//  <tr>
//    <td><p>New Imported Order Status</p>
//      </td>
//    <td><select name="imported_order_status" id="imported_order_status">
//    </select>
//    </td>
//  </tr>  
//</table>
//<input type="submit" name="save" id="save" value="Submit">
//</form>
//PAGE;
//    
//    return $page;
//}

?>

