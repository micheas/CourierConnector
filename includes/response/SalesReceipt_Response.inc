<?php

require_once 'uc_order.inc';

function courier_ubercart_SalesReceipt_Response($responseXml)
{ 
    courier_ubercart_process_order("SalesReceipt", $responseXml);
}

?>