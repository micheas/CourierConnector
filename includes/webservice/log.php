<?php

function log_transaction($type, $status, $message)
{
    db_query("insert into courier_transaction_log (trans_date, type, message, status) value('%s', '%s', '%s', '%s')",
            array(date('Y-m-d h:m:s'), $type, $message, $status));
}

?>