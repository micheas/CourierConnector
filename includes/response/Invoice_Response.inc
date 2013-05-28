<?php

require_once 'uc_order.inc';

function courier_ubercart_Invoice_Response($responseXml)
{ 
    courier_ubercart_process_order("Invoice", $responseXml);
}
