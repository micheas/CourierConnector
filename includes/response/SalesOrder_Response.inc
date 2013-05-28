<?php

require_once 'uc_order.inc';

function courier_ubercart_SalesOrder_Response($responseXml)
{ 
    courier_ubercart_process_order("SalesOrder", $responseXml);
}
