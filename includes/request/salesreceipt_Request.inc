<?php

//$order

//stdClass Object
//(
//    [order_id] => 2
//    [uid] => 1
//    [order_status] => pending
//    [order_total] => 433
//    [product_count] => 1
//    [primary_email] => pbliss81@gmail.com
//    [delivery_first_name] => Peter
//    [delivery_last_name] => Bliss
//    [delivery_phone] => 5039755326
//    [delivery_company] => 
//    [delivery_street1] => 222 SW Harrison APT 22H
//    [delivery_street2] => 
//    [delivery_city] => Portland
//    [delivery_zone] => 49
//    [delivery_postal_code] => 97201
//    [delivery_country] => 840
//    [billing_first_name] => Peter
//    [billing_last_name] => Bliss
//    [billing_phone] => 5039755326
//    [billing_company] => 
//    [billing_street1] => 222 SW Harrison APT 22H
//    [billing_street2] => 
//    [billing_city] => Portland
//    [billing_zone] => 49
//    [billing_postal_code] => 97201
//    [billing_country] => 840
//    [payment_method] => 
//    [data] => Array
//        (
//            [complete_sale] => logged_in
//        )
//
//    [created] => 1340330713
//    [modified] => 1340330721
//    [host] => 127.0.0.1
//    [currency] => USD
//    [products] => Array
//        (
//            [2] => stdClass Object
//                (
//                    [order_product_id] => 2
//                    [order_id] => 2
//                    [nid] => 1
//                    [title] => test
//                    [model] => 32423
//                    [qty] => 1
//                    [cost] => 0.00000
//                    [price] => 433.00000
//                    [weight] => 0
//                    [weight_units] => lb
//                    [data] => Array
//                        (
//                            [attributes] => Array
//                                (
//                                )
//
//                            [shippable] => 1
//                            [type] => product
//                            [module] => uc_product
//                        )
//
//                    [rdf_mapping] => Array
//                        (
//                        )
//
//                    [order] => stdClass Object
// *RECURSION*
//                    [order_uid] => 1
//                )
//
//        )
//
//    [line_items] => Array
//        (
//            [0] => Array
//                (
//                    [line_item_id] => subtotal
//                    [type] => subtotal
//                    [title] => Subtotal
//                    [amount] => 433
//                    [weight] => 0
//                    [data] => Array
//                        (
//                        )
//
//                )
//
//        )
//
//    [rdf_mapping] => Array
//        (
//        )
//)

function courier_ubercart_generate_SalesReceipt_Request($orders)
{    
    require_once "order_request.inc";
                
    //calls the generic order request creation
    $order_data = courier_ubercart_generate_order_request("SalesReceipt", $orders);

    foreach($order_data as $order)
    {       
        //add any extra data here
        //order['customer'] - customer xml doc
        //order['order'] - order xml doc
        
        //need to add in the tender information here
        $ord = uc_order_load($order);
        $r = $order['order']->getElementsByTagName("SalesReceipt")->item(0);               
        $tenders = $order['order']->createElement("Tenders");
        $tender = $order['order']->createElement("TenderCreditCard");        
        $tender->appendChild($order['order']->createElement ("TenderAmount", $ord->order_total));
        $tender->appendChild($order['order']->createElement ("CardName", $ord->payment_method));        
        $tenders->appendChild($tender);
        $r->appendChild($tenders);
               
        //write the to the database
        db_query("Insert into courier_requests (request_xml) values('%s')", trim(str_replace('<?xml version="1.0"?>', "", $order['customer']->saveXML() . $order['order']->saveXML()))); 
    }
}


?>
