<?php

function courier_get_items($xmlParser)
{		  
    //pull down the items in blocks
    $lastID = $xmlParser->output[0]['child'][0]['attrs']['LASTID'];
    $blockSize = $xmlParser->output[0]['child'][0]['attrs']['BLOCKSIZE'];
    $lastTime = $xmlParser->output[0]['child'][0]['attrs']['TIMEMODIFIED'];    
          
    //$nodeIDs = db_query('SELECT nid FROM {node} where nid > %d AND type = "Product" OR type = "Product_Kit" order by nid limit %d', ($lastID != "" ? $lastID : "0"), $blockSize);
    $nodeIDs = db_query('select nid from {node} where nid > :nid AND type = "Product" OR type = "Product_Kit" order by nid limit :limit', array(":nid" => ($lastID != "" ? $lastID : "0"), ":limit" => $blockSize));
    
    if(mysql_num_rows($nodeIDs) == 0)
    {
        return "<getItemsResult Status = \"NoItems\"></getItemsResult>";
    }
    
    $items = '<getItemsResult Status = "OK">';
        
    while($itemNodeID = $nodeIDs)
    {
        $node = node_load($itemNodeID['nid']);
        
        //$xc .= var_export($p, true) . "\n";        
        $items .= '<Item ';
        $items .= 'Title = "' . htmlspecialchars($node->title, ENT_QUOTES, "UTF-8")  . '" ' ;
        $items .= 'NID = "' . htmlspecialchars($node->nid, ENT_QUOTES, "UTF-8")  . '" ' ;
        $items .= 'SKU = "' . htmlspecialchars($node->model, ENT_QUOTES, "UTF-8")  . '" ' ;
        $items .= 'Status = "' . $node->status .'" ' ;
        $items .= 'uid = "' . $node->uid .'" ' ;
        $items .= 'Changed = "' . $node->created . '" ' ;
        $items .= 'changed = "' . $node->changed . '" ' ;
        $items .= 'price = "' . $node->sell_price . '" ' ;         
        
        $items .= " />";             
    }
    
    $items .= "</getItemsResult>";
    
    return $items;
}

function courier_update_items($xmlParser)
{
    $status = "";
    $errorMessage = "";
    $warningMessage = "";
    $nidList = array();
    
    try
    {		
        //loop through each item to add or update
        foreach($xmlParser->output[0][child] as $itemNode)
        {	                           
              //$nid = db_result(db_query("select nid from {uc_products} where model = '%s'", $itemNode['attrs']['SKU']));
              $nid = db_query("select nid from {uc_products} where model = :model", array(":model" => $itemNode['attrs']['SKU']))->fetchField();
              
              $node = null;
              
              try
              {			
                  if(isset($itemNode['attrs']['NID']) && $itemNode['attrs']['NID'] == "")
                  {
                      $node = node_load($itemNode['attrs']['NID']);
                  }
                  else if($nid)
                  {
                      $node = node_load($nid);
                  }
                  else
                  {
                      $node->is_new = true;
                      $node->created = time();
                  }                 
                                                      	
                	if(isset($itemNode['attrs']['TITLE']) && $itemNode['attrs']['TITLE'] != "")
                  {
                      $node->title = $itemNode['attrs']['TITLE'];                  
                  }
                  
                  if(isset($itemNode['attrs']['DESCRIPTION']) && $itemNode['attrs']['DESCRIPTION'] != "")
                  {
                      $node->body = $itemNode['attrs']['DESCRIPTION'];                  
                  }                                    
                                  	                  
                  if(isset($itemNode['attrs']['STATUS']))
                  {                    
                      $node->status = $itemNode['attrs']['STATUS'];
                  }                     
                  
                  $node->promote = 0;
                  $node->sticky = 0;  
                  
                  if(isset($itemNode['attrs']['TYPE']))
                  {                
                      $node->type = $itemNode['attrs']['TYPE'];
                  }
                  else                  
                  {
                      $node->type = "product";
                  }                                    					                                  
                 
                  if(isset($itemNode['attrs']['PRICE']) && $itemNode['attrs']['PRICE'] != "")
                  {
                      $node->sell_price = $itemNode['attrs']['PRICE'];
                  }
                  
                  if(isset($itemNode['attrs']['SKU']) && $itemNode['attrs']['SKU'] != "")
                  {
                      $node->model = $itemNode['attrs']['SKU'];
                  }                                       
                 
                  if(isset($itemNode['attrs']['WEIGHTMEASUREMENT']))
                  {
                      $node->weight_units = $itemNode['attrs']['WEIGHTMEASUREMENT'];
                  }   
                  
                  if(isset($itemNode['attrs']['WEIGHT']))
                  {
                      $node->weight = $itemNode['attrs']['WEIGHT'];
                  }   
                  
                  if(isset($itemNode['attrs']['HEIGHT']))
                  {
                      $node->height = $itemNode['attrs']['HEIGHT'];
                  }  
                  
                  if(isset($itemNode['attrs']['LENGTHMEASUREMENT']))
                  {
                      $node->length_units = $itemNode['attrs']['LENGTHMEASUREMENT'];
                  }   
                  
                  if(isset($itemNode['attrs']['PKGQTY']))
                  {
                      $node->pkg_qty = $itemNode['attrs']['PKGQTY'];
                  }  
                  
                  if(isset($itemNode['attrs']['DEFAULT_QTY']))
                  {
                      $node->default_qty = $itemNode['attrs']['DEFAULT_QTY'];
                  }  
                  
                  if(isset($itemNode['attrs']['LENGTH']))
                  {
                      $node->length = $itemNode['attrs']['LENGTH'];
                  } 
                  
                  if(isset($itemNode['attrs']['LIST_PRICE']))
                  {
                      $node->list_price = $itemNode['attrs']['LIST_PRICE'];
                  }                                      
                  
                  if(isset($itemNode['attrs']['COST']) && $itemNode['attrs']['COST'] != "")
                  {
                      $node->cost = $itemNode['attrs']['COST'];
                  }                                                                             
                  
                  //check the category relations
                  foreach($itemNode['child'] as $categoryRecord)
                  {
                      if(isset($categoryRecord['attrs']['CATEGORY_ID']) && $categoryRecord['attrs']['CATEGORY_ID'] != "")
                      {
                          //taxonomy_node_save($node, array($categoryRecord['attrs']['CATEGORY_ID']));
                          $node->field_tags[$node->language][]['tid'] = $categoryRecord['attrs']['CATEGORY_ID'];
                      }
                  }                 
                  
                  node_save($node);	                                        
                  
                  //record the nid             
                  $nidList[$node->nid] = array("sku"=>$node->model, "dataid" => $itemNode['attrs']['DATA_ID']);
                  //$nidList[$node->model] = $node->nid;                   
              }
              catch(Exception $ex)
              {
                  $errorMessage .= $ex->getMessage();      
              }                     
         }                           
    }
    catch(Exception $exx)
    {      
        $errorMessage .= $exx->getMessage();           
    }
    
    //return the error message if one exists
    if($errorMessage != "")
    {
        $status = "<ItemUpdateResult Status = \"Error\">";
        $status .= "<Error Message=\"" . $errorMessage . "\" />";      
    }
    else
    {
        //check if there is any warnings
        if($warningMessage != "")
        {
          $status = "<ItemUpdateResult Status = \"Warnings\">";
          $status .= "<Warning Message=\"" . $warningMessage . "\" />";
        }
        else
        {
          //no errors or warnings mark that status
          $status = "<ItemUpdateResult Status = \"OK\">";
        }
        
         //generate a return message
        foreach($nidList as $nid => $data)
        {
          $status .= '<Item NID="' . $nid . '" SKU = "' . htmlspecialchars($data['sku'], ENT_QUOTES, "UTF-8") . '" DataID = "' . $data['dataid'] . '" />';
        }  
    }

    $status .= "</ItemUpdateResult>";    
    return $status;
}

//************************************

//Stock 
//************************************

function courier_update_stock($xmlParser)
{    
    $updated = 0;
    $inserted = 0;
    
    foreach($xmlParser->output[0][child] as $stockNode)
    { 
        try
        {              
            //do an update 
            //db_query("UPDATE {uc_product_stock} SET active = %d, stock = %d, threshold = %d WHERE sku = '%s'", $stockNode['attrs']['ACTIVE'], $stockNode['attrs']['STOCKLEVEL'], $stockNode['attrs']['THRESHOLD'], $stockNode['attrs']['SKU']);           
            
            $affected = db_update('uc_product_stock')
              ->fields(array(
                'active' => $stockNode['attrs']['ACTIVE'],
                'stock' => $stockNode['attrs']['STOCKLEVEL'],
                'threshold' => $stockNode['attrs']['THRESHOLD'],
            ))
            ->condition('sku', $stockNode['attrs']['SKU'])
            ->execute();
            
            //db_query("UPDATE {} SET active = :active, stock = :stock, threshold = :threshold WHERE sku = :sku", array(":active" => $stockNode['attrs']['ACTIVE'], ":stock" => $stockNode['attrs']['STOCKLEVEL'], ":threshold" => $stockNode['attrs']['THRESHOLD'], ":sku" => $stockNode['attrs']['SKU']));
             
            //if there wasnt a record to update
            if ($affected == 0) 
            {
                //check if there was an nid passed
                if(!isset($stockNode['attrs']['NID']) || $stockNode['attrs']['NID'] == "")
                {
                    //if not find it based on sku
                    $nid = db_result(db_query("select nid from {uc_products} where model = '%s'", $itemNode['attrs']['SKU']));
                }
                else
                {
                  $nid = $stockNode['attrs']['NID'];
                }
            
                //create the record
                //db_query("INSERT INTO {uc_product_stock} (sku, nid, active, stock, threshold) VALUES ('%s', %d, %d, %d, %d)", $stockNode['attrs']['SKU'], $nid, $stockNode['attrs']['ACTIVE'], $stockNode['attrs']['STOCKLEVEL'], $stockNode['attrs']['THRESHOLD']);
                db_query("INSERT INTO {uc_product_stock} (sku, nid, active, stock, threshold) VALUES (:sku, :nid, :active, :stock, :threshold)", array(":sku" => $stockNode['attrs']['SKU'], ":nid" => $nid, ":active" => $stockNode['attrs']['ACTIVE'], ":stock" => $stockNode['attrs']['STOCKLEVEL'], ":threshold" => $stockNode['attrs']['THRESHOLD']));
                
                //mark item as inserted
                $inserted++;
            }
            else
            {
                module_invoke_all('uc_stock_adjusted', $sku, $stock->stock, $qty);
                
                //mark item as updated
                $updated++;
            }
        }
        catch(Exception $ex)
        {
            $errorMessage .= $ex->getMessage();      
        } 
    }    
				
		//return the error message if one exists
    if($errorMessage != "")
    {
        $status = "<StockUpdateResult Status = \"Error\">";
        $status .= "<Error Message=\"" . $errorMessage . "\" />";      
    }
    else
    {
        //check if there is any warnings
        if($warningMessage != "")
        {
          $status = "<StockUpdateResult Status = \"Warnings\">";
          $status .= "<Warning Message=\"" . $warningMessage . "\" />";
        }
        else
        {
          //no errors or warnings mark that status
          $status = '<StockUpdateResult Status = "OK" updated="' . $updated . '" inserted = "' . $inserted . '" >';
        }      
    }

    $status .= "</StockUpdateResult>";    
		
		return $status;
}

function courier_product_adjustment_update($xmlParser)
{
    foreach($xmlParser->output[0][child] as $node)
    { 
        try
        {                             
            if($node['attrs']['NID'] != "")
            {
                $combo = array();
                foreach($node[child][0][child] as $comboData)
                {
                    $combo[$comboData['attrs']['AID']] = $comboData['attrs']['OID'];                
                } 

                //do an update 
                //db_query("UPDATE {uc_product_adjustments} SET combination = '%s' where nid = %d and model = '%s'", serialize($combo), $node['attrs']['NID'], $node['attrs']['SKU']);            
                //db_query("UPDATE {uc_product_adjustments} SET combination = :combination where nid = :nid and model = :model", array(":combination" => serialize($combo), ":nid" => $node['attrs']['NID'], ":model" => $node['attrs']['SKU']));
                $affected = db_update('uc_product_adjustments')
                    ->fields(array(
                      'combination' => serialize($combo),                      
                  ))
                  ->condition('model', $node['attrs']['SKU'])
                  ->execute();
                          
                //if there wasnt a record to update
                if ($affected == 0) 
                {
                    
                      //db_query("INSERT INTO {uc_product_adjustments} (nid, combination, model) values('%d', '%s', '%s')", $node['attrs']['NID'], serialize($combo), $node['attrs']['SKU']); 
                       db_query("INSERT INTO {uc_product_adjustments} (nid, combination, model) values(:nid, :combination, :model)", array(":combination" => serialize($combo), ":nid" => $node['attrs']['NID'], ":model" => $node['attrs']['SKU']));                   
                }
            }
        }
        catch(Exception $ex)
        {
            $errorMessage .= $ex->getMessage();      
        } 
    }    
				
		//return the error message if one exists
    if($errorMessage != "")
    {
        $status = "<ProductAdjustmentUpdateResult Status = \"Error\">";
        $status .= "<Error Message=\"" . $errorMessage . "\" />";      
    }
    else
    {
        //check if there is any warnings
        if($warningMessage != "")
        {
          $status = "<ProductAdjustmentUpdateResult Status = \"Warnings\">";
          $status .= "<Warning Message=\"" . $warningMessage . "\" />";
        }
        else
        {
          //no errors or warnings mark that status
          $status = '<ProductAdjustmentUpdateResult Status = "OK" >';
        }      
    }

    $status .= "</ProductAdjustmentUpdateResult>";    
		
		return $status;
}

//************************************


//************************************

//Attributes 
//************************************

function courier_get_attributes($xmlParser)
{
    
}

function courier_get_options($xmlParser)
{

}
          
function courier_get_product_attributes($xmlParser)
{

}
        
function courier_get_product_options($xmlParser)
{

}

function courier_attribute_update($xmlParser)
{
    $attributeIDS = array();

    foreach($xmlParser->output[0][child] as $node)
    { 
        try
        { 
            //db_query("insert into {uc_attributes} (name) values('%s')", $node['attrs']['NAME']);
            //db_query("insert into {uc_attributes} (name) values(:name)", array(":name" => $node['attrs']['NAME']));
            
            //$attributeIDS[$node['attrs']['NAME']] = db_last_insert_id('uc_attributes', 'aid');            
            $attributeIDS[$node['attrs']['NAME']] = db_insert('uc_attributes')
                    ->fields(array(
                      'name' => $node['attrs']['NAME'],                      
                  ))                  
                  ->execute();
        }
        catch(Exception $ex)
        {
            $errorMessage .= $ex->getMessage();      
        } 
    }    
        
    //return the error message if one exists
    if($errorMessage != "")
    {
        $status = "<AttributeUpdateResult Status = \"Error\">";
        $status .= "<Error Message=\"" . $errorMessage . "\" />";      
    }
    else
    {
        //check if there is any warnings
        if($warningMessage != "")
        {
            $status = "<AttributeUpdateResult Status = \"Warnings\">";
            $status .= "<Warning Message=\"" . $warningMessage . "\" />";
        }
        else
        {
            //no errors or warnings mark that status
            $status = '<AttributeUpdateResult Status = "OK">';
        }      
        
        foreach($attributeIDS as $Name => $aid)
        {
            $status .= '<Attribute AID="' . $aid . '" name = "' . $Name . '" />';
        }  
    }
   
    $status .= "</AttributeUpdateResult>";    

    return $status;
}
          
function courier_option_update($xmlParser)
{  
    $optionData = array();

    foreach($xmlParser->output[0][child] as $node)
    { 
        try
        { 
            //get the attribute id
            
            //$aid = db_result(db_query("select aid from {uc_attributes} where name = '%s' limit 1", $node['attrs']['ATTRIBUTENAME']));
            $aid = db_query("select aid from {uc_attributes} where name = :name limit 1", array(":name" => $node['attrs']['ATTRIBUTENAME']))->fetchField();
            
            if($aid != "")
            {
                //db_query("insert into {uc_attribute_options} (name, aid) values('%s', %d)", $node['attrs']['NAME'], $aid);                
               //$optionData[$node['attrs']['NAME']] = array( "oid" => db_last_insert_id('uc_attribute_options', 'oid'), "aid" => $aid);
               
               $optionData[$node['attrs']['NAME']] =  array( "oid" => db_insert('uc_attribute_options')->fields(array(
                  'name' => $node['attrs']['NAME'],
                  'aid' => $aid,                  
                  ))->execute(), "aid" => $aid);
            }
                        
        }
        catch(Exception $ex)
        {
            $errorMessage .= $ex->getMessage();      
        } 
    }    
        
    //return the error message if one exists
    if($errorMessage != "")
    {
        $status = "<OptionUpdateResult Status = \"Error\">";
        $status .= "<Error Message=\"" . $errorMessage . "\" />";      
    }
    else
    {
        //check if there is any warnings
        if($warningMessage != "")
        {
          $status = "<OptionUpdateResult Status = \"Warnings\">";
          $status .= "<Warning Message=\"" . $warningMessage . "\" />";
        }
        else
        {
          //no errors or warnings mark that status
          $status = '<OptionUpdateResult Status = "OK">';
        }  
            
        foreach($optionData as $Name => $data)
        {
            $status .= '<Option OID="' . $data['oid'] . '" name = "' . htmlspecialchars($Name, ENT_QUOTES, "UTF-8") . '" aid ="' . $data['aid'] .'" />';
        }       
    }

    $status .= "</OptionUpdateResult>";    

    return $status;
}
         
function courier_product_attribute_update($xmlParser)
{
    $productAttributeData = array();

    foreach($xmlParser->output[0][child] as $node)
    { 
        try
        { 
            if($node['attrs']['NID'] != "")
            {
                $nid = $node['attrs']['NID'];
            }
            else if($node['attrs']['SKU'] != "")
            {
                //$nid = db_result(db_query("select nid from {uc_products} where model = '%s'", $node['attrs']['SKU']));
                $nid = db_query("select nid from {uc_products} where model = :model", array(":model" => $node['attrs']['SKU']))->fetchField();
                
                //check the product adjustments to see if we can find the nid
                if($nid == "")
                {
                    //$nid = db_result(db_query("select nid from {product_adjustments} where model = '%s'", $node['attrs']['SKU']));
                    $nid = db_query("select nid from {product_adjustments} where model = :model", array(":model" => $node['attrs']['SKU']))->fetchField();
                }
            }
            
            if($node['attrs']['AID'] != "")
            {
                $aid = $node['attrs']['AID'];
            }
            else if($node['attrs']['ATTRIBUTENAME']  != "")
            {
                //$aid = db_result(db_query("select aid from {uc_attributes} where name = '%s'", $node['attrs']['ATTRIBUTENAME']));
                 $aid = db_query("select aid from {uc_attributes} where name = :name", array(":name" => $node['attrs']['ATTRIBUTENAME']))->fetchField();
            }
            
            //nid can not be blank
            if($nid != "" && $aid != "")
            {
                  //db_query("insert into {uc_product_attributes} (nid, aid) values(%d, %d)", $nid, $aid);
                  db_insert('uc_product_attributes')->fields(array(
                    'nid' => $nid,
                    'aid' => $aid,                  
                    ))->execute();
                  
                 // $productAttributeData[$node['attrs']['SKU']] = array("SKU" => $node['attrs']['SKU'], "nid" => $nid, "aid" => $aid, "attributeName" => $node['attrs']['ATTRIBUTENAME']);
                  
                  //$productAttributeData[$node['attrs']['SKU'] . $nid . $aid] = array("SKU" => $node['attrs']['SKU'], "nid" => $nid, "aid" => $aid, "attributeName" => $node['attrs']['ATTRIBUTENAME']);
                  
                  array_push($productAttributeData, array("SKU" => htmlspecialchars($node['attrs']['SKU'], ENT_QUOTES, "UTF-8"), "nid" => $nid, "aid" => $aid, "attributeName" => htmlspecialchars($node['attrs']['ATTRIBUTENAME'], ENT_QUOTES, "UTF-8")));
            }
        }
        catch(Exception $ex)
        {
            $errorMessage .= $ex->getMessage();      
        } 
    }    
        
    //return the error message if one exists
    if($errorMessage != "")
    {
        $status = "<ProductAttributeUpdateResult Status = \"Warnings\">";
        $status .= "<Error Message=\"" . $errorMessage . "\" />";      
    }
    else
    {
        //check if there is any warnings
        if($warningMessage != "")
        {
            $status = "<ProductAttributeUpdateResult Status = \"Warnings\">";
            $status .= "<Warning Message=\"" . $warningMessage . "\" />";
        }
        else
        {
            //no errors or warnings mark that status
            $status = '<ProductAttributeUpdateResult Status = "OK">';
        }                   
    }
    
   foreach($productAttributeData as $data)
    {
        $status .= '<ProductAttribute SKU="' . htmlspecialchars($data['SKU'], ENT_QUOTES, "UTF-8") . '" nid = "' . $data['nid'] . '" aid ="' . $data['aid'] .'" attributeName ="' . $data['attributeName'] . '" />';
    }

    $status .= "</ProductAttributeUpdateResult>";    

    return $status;
}
        
function courier_product_option_update($xmlParser)
{
    $productOptionData = array();

    foreach($xmlParser->output[0][child] as $node)
    { 
        try
        { 
            if($node['attrs']['NID'] != "")
            {
                $nid = $node['attrs']['NID'];
            }
            else if($node['attrs']['SKU'] != "")
            {
                //$nid = db_result(db_query("select nid from {uc_products} where model = '%s'", $node['attrs']['SKU']));
                $nid = db_query("select nid from {uc_products} where model = :model", array(":model" => $node['attrs']['SKU']))->fetchField();
                
                //check the product adjustments to see if we can find the nid
                if($nid == "")
                {
                    //$nid = db_result(db_query("select nid from {product_adjustments} where model = '%s'", $node['attrs']['SKU']));
                    $nid = db_query("select nid from {product_adjustments} where model = :model", array(":model" => $node['attrs']['SKU']))->fetchField();
                }
            }
            
            if($node['attrs']['OID'] != "")
            {
                $oid = $node['attrs']['OID'];
            }
            else if($node['attrs']['OPTIONNAME']  != "")
            {
                //$oid = db_result(db_query("select oid from {uc_attribute_options} where name = '%s'", $node['attrs']['OPTIONNAME']));
                $oid = db_query("select oid from {uc_attribute_options} where name = :name", array(":name" => $node['attrs']['OPTIONNAME']))->fetchField();
            }
            
            //nid can not be blank
            if($nid != "" && $oid != "")
            {
                  //db_query("insert into {uc_product_options} (nid, oid, price, ordering) values(%d, %d, %f, %d)", $nid, $oid, $node['attrs']['PRICEADJUSTMENT'], $node['attrs']['ORDERING']);
                   db_insert('uc_product_options')->fields(array(
                    'nid' => $nid,
                    'oid' => $oid,
                    'cost' => 0,
                    'weight' => 0,
                    'price' => $node['attrs']['PRICEADJUSTMENT'],
                    'ordering' => ($node['attrs']['ORDERING'] != '' ? $node['attrs']['ORDERING'] : 0),                  
                    ))->execute();        
                  //db_query("insert into {uc_product_options} (nid, oid, price, ordering) values(:nid, :oid, :price, :ordering)", array(":nid" => $nid, ":oid" => $oid, ":price" => $node['attrs']['PRICEADJUSTMENT'], ":ordering" => $node['attrs']['ORDERING']));
                  
                  array_push($productOptionData, array("sku"=>htmlspecialchars($node['attrs']['SKU'], ENT_QUOTES, "UTF-8"), "nid" => $nid, "oid" => $oid, "optionName" => htmlspecialchars($node['attrs']['OPTIONNAME'], ENT_QUOTES, "UTF-8")));
            }
        }
        catch(Exception $ex)
        {
            $errorMessage .= $ex->getMessage();      
        } 
    }    
        
    //return the error message if one exists
    if($errorMessage != "")
    {
        $status = "<ProductOptionUpdateResult Status = \"Warnings\">";
        $status .= "<Error Message=\"" . $errorMessage . "\" />";      
    }
    else
    {
        //check if there is any warnings
        if($warningMessage != "")
        {
            $status = "<ProductOptionUpdateResult Status = \"Warnings\">";
            $status .= "<Warning Message=\"" . $warningMessage . "\" />";
        }
        else
        {
            //no errors or warnings mark that status
            $status = '<ProductOptionUpdateResult Status = "OK">';
        }               
    }
    
     foreach($productOptionData as $data)
      {
          $status .= '<ProductOption SKU="' . $data['sku'] . '" nid = "' . $data['nid'] . '" oid ="' . $data['oid'] .'" optionName = "' . htmlspecialchars($data['optionName'], ENT_QUOTES, "UTF-8") . '" />';
      }       

    $status .= "</ProductOptionUpdateResult>";    

    return $status;
}

?>