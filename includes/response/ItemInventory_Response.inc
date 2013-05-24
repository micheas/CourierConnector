<?php

require_once 'drupal/product.inc';

function courier_ubercart_ItemInventory_Response($responseXml)
{
    courier_ubercart_process_product("ItemInventory", $responseXml);
}

?>