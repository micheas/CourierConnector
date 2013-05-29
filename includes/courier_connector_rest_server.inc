<?php

class CourierXMLResponseParser extends RESTServer {
  public static function parseXML($handle)  {
	
    //just want the straight xml from the request to be passed along
    $xml = self::contentFromStream($handle);
    
	// if $xml_data is Null then we expect errors
    if (!$xml) {
	  // build an error message string
      $message = '';
      
	  // throw an error
      services_error($message, 406);
    }
    
  	return $xml;
  }
  
}

class RESTServerViewCourier extends RESTServerView {
  public function render() {
    $doc = new DOMDocument('1.0', 'utf-8');
    $root = $doc->createElement('courier_response');
    $doc->appendChild($root);

    $this->xml_recurse($doc, $root, $this->model);
    
    return $doc->saveXML();
  }
  
   private function xml_recurse(&$doc, &$parent, $data) {
    if (is_object($data)) {
      $data = get_object_vars($data);
    }

    if (is_array($data)) {
      $assoc = FALSE || empty($data);
      $k = '';
      if(array_key_exists('key', $data)) {
        $k = $value;
        $k = preg_replace('/[^A-Za-z0-9_]/', '_', $k);
        $k = preg_replace('/^([0-9]+)/', '_$1', $k);
      }
      foreach ($data as $key => $value) {
        if ($key === "key") {          
          $k = $value;
          $k = preg_replace('/[^A-Za-z0-9_]/', '_', $k);
          $k = preg_replace('/^([0-9]+)/', '_$1', $k);
          continue;
        }      
        else if($key === "attrs") {
          foreach($value as $attr_name => $attr_value) {
            $parent->setAttribute($attr_name, $attr_value);
          }
          continue;
        }
        else if (is_numeric($key)) {
          $key = $k;
        }
        else {
          $assoc = TRUE;
          $key = preg_replace('/[^A-Za-z0-9_]/', '_', $key);
          $key = preg_replace('/^([0-9]+)/', '_$1', $key);
        }
                
        try {
          $element = $doc->createElement($key);
        }
        catch (Exception $ex) {
          echo "here";
        }
        $parent->appendChild($element);
        $this->xml_recurse($doc, $element, $value);
      }

//      if (!$assoc) {
//        $parent->setAttribute('is_array', 'true');
//      }
    }
    elseif ($data !== NULL) {
      $parent->appendChild($doc->createTextNode($data));
    }
  }
}