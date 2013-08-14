<?php
/**
 * @file
 * The admin mapping. This needs an import from upstream.
 */

/**
 * Builds the config form.
 * @param $form_state
 * @return mixed
 */
function courier_ubercart_pos_mapping_form(&$form_state) {

  return system_settings_form($form_state);
}

/**
 * Form validate handler for the courier_quickbooks_config_form.
 */
function courier_ubercart_pos_mapping_form_validate(&$form_state) {

}
