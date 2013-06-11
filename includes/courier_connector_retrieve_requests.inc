<?php
//special object_ids in json
//id - the id that is being passed in
//object - the object that is being passed in
//anything else will be looked up on the mapping

function courier_connector_retrieve_requests($job_id) {
  require_once 'courier_connector_mapping.inc';

  //if a job id is passed in here then that triggers the processing of any xml in the reponse table for that job that needs to be processed
  if(!is_null($job_id)) {
    require_once 'courier_connector_process_responses.inc';
    return courier_connector_process_response($job_id);
  }
  
  $request_parameters = file_get_contents(drupal_get_path('module', 'courier_connector_uber_7') . "/includes/" . "request_parameters.json");
  $request_parameters=json_decode($request_parameters,true);

  $jobs = array();
  $job_ids = array();
  
  if(courier_connector_get_drupal_version() == '6') {
    //get a list of the job ids that we are going to process here
    $result = db_query("Select job_id from courier_requests where processed = 0 order by job_date desc");
    $job_ids = db_fetch_array($result);
  }
  else {
      $job_ids = db_select("courier_requests")
          ->fields("courier_requests", array("job_id"))
          ->orderBy("job_date", "DESC")
          ->condition("processed", "0")
         ->execute();  
  }
  $job_number = 0;
  
  $jobs['key'] = 'job_request';
  
  foreach($job_ids as $job_id) {  
        
    $job_id = (array) $job_id;
    
    $courier_requests = array();

    $result = array();
    if(courier_connector_get_drupal_version() == '6') {
      $request_data = db_query("select job_id, job_date, request_type, request_method, request_data from courier_requests where job_id = %d order by job_date asc", $job_id['job_id']);           
      $result = db_fetch_array($request_data);
    }
    else {
      $query = db_select("courier_requests");
      $query->fields("courier_requests", array("job_id", "job_date", "request_type", "request_method", "request_data"));  
      $query->orderBy("job_date", "asc");  
      $query->condition("job_id", $job_id['job_id']);
      $result = $query->execute();  
    }
    
    //while($record = db_fetch_array($request_data)) {
    foreach($result as $record) { 
   
      $record = (array) $record;
      
      courier_connector_process_request($record['job_id'], $record['request_method'], $request_parameters[COURIER_CONNECTOR_CART], $record['request_data'], $record['request_type'], $courier_requests);
      
      $courier_requests["attrs"] = array("job_id" => $record['job_id'], "job_number" => $job_number);           
    }

    if(courier_connector_get_drupal_version() == '6') {
      db_query("update courier_requests set processed = 1, processed_date = '%s' where job_id = %d", date( 'Y-m-d H:i:s', time()), $job_id['job_id']);
    }
    else {
      $query = db_update("courier_requests");
      $query->fields(array("processed" => 1, "processed_date" => date( 'Y-m-d H:i:s', time())));
      $query->condition('job_id', $job_id['job_id']);      
      $query->execute(); 
    }

    //should put the jobs into an array and that is what is returned
    //jobid => job
    //a job is a complete thought
    //like a customer, and and order, where that customer was the one related on that order, and the customer had to be downloaded before the order could
    //making it a job request as a complete thought

    //process a list of jobs found, fill out and array of these and populate the array of them to be processed into the xml

    //the xml schema must be modified to support jobs, wrap the header in a job_count, job_number, job_id
    //then can track the current job, how many there are, and the id of this job
    //<Courier_requests job_count=''>
    //<Job_Request job_number='' job_id=''>
    //<InventoryItem_request>   
        
    $jobs[] =  $courier_requests;
    $job_number++;
  }
  
  $return_value = array();
  
  if(count($jobs) > 0) {
    $return_value['attrs'] = array("job_count" => count($jobs) - 1);
    $return_value['courier_requests'] = $jobs;
  }   
  
  return (object)$return_value; 
}

//gather the data to generate the request
function courier_connector_process_request($job_id, $object_type, $parameters, $request_data, $request_type, &$requests_queued, $return_values = false) {     

  $data = unserialize($request_data);
 
  if($request_type == 'read') {
    
    foreach($data as $k => $v) {
      if(!empty($v)) {
        $requests_queued[$object_type . "_Request"][$k] = $v;
      }
    }   
  }
  else {        
    if(array_key_exists($object_type . '_ids', $data)) {
      
      $data[$object_type . "s"]["key"] = $object_type;
   
      foreach($data[$object_type . '_ids'] as $object_id) {                
        //create the request
        $record = array();
        $object_record = array();
        $reference_object = array();
                        
        //foreach($parameters as $parameter_type) {          
          //foreach($parameter_type as $parameter) {          
        foreach($parameters[$object_type] as $parameter) {                         
          
          //if the requested id is all of them, we will need to do some recursion
          if($object_id == "all") {

            $object_ids = array();
            
            //get a list of all of the objects
            if(courier_connector_get_drupal_version() == '6') {
              //TODO
              //            $request_data = db_query("select job_id, job_date, request_type, request_method, request_data from courier_requests where job_id = %d order by job_date asc", $job_id['job_id']);           
              //            $result = db_fetch_array($request_data);
            }
            else {
              $query = db_select($parameter['table']);
              $query->fields($parameter['table'], array($parameter['cart_id']));                
              $object_ids = $query->execute();  
            }
                        
            foreach($object_ids as $i) {
              $i = (array)$i;
              
              courier_connector_process_request($job_id, $object_type, $parameters, serialize(array($object_type . '_ids' => array($i[$parameter['cart_id']]))), $request_type, $requests_queued);                                         
            }
            
            return null;
          }
          
          if($parameter['object_id'] == 'id') {            
            $object_record = $parameter['method']($object_id);
            $reference_object = $object_record; //a copy of the original object for reference
          }
          else if($parameter['object_id'] == 'object') {            
            $object_record = $parameter['method']($object_record);                         
          }
          else {                        
            $object_record = $parameter['method'](courier_connector_get_value($reference_object, $parameter['object_id']));
          }

          $filter_passed = true;

            //check for field_filter these will exclude the record because of a field does not = the value
            //for excluding some things that are not what we are looking for
            //such as the line items is generic, and we only want the product line items, skip the shipping ones
            if(array_key_exists("field_filter", $parameter)) {
              foreach($parameter['field_filter'] as $filter => $value) {
                  if(is_object($object_record)) {
                    $r = get_object_vars($object_record);
                  }

                  if(array_key_exists($filter, $r)) {
                    if($r[$filter] != $value) {
                      $filter_passed = false;
                    }
                  }
              }            
            }

            if($filter_passed) {                      
              //sub method, like a customer on a order. Its a supporting request on its own, called before the method
              if(array_key_exists("sub_methods", $parameter)) {
                foreach($parameter['sub_methods'] as $sub_method => $sub_method_id) {

                  //$sub_parameters = $parameters[$sub_method];

                  $sub_method_id_value = courier_connector_get_value($object_record, $sub_method_id);

                  courier_connector_process_request($job_id, $sub_method, $parameters, serialize(array($sub_method . '_ids' => array($sub_method_id_value))), $request_type, $requests_queued);             
                } 
              }                          

              //process the method request
              $new_data = courier_connector_process_request_parameters($parameter, $object_record, $object_type);
              
              //add in the foreign id this value does not need to be in the mapping, should always be in there if needed              
              if(array_key_exists('foreign_id', $parameter)) {                                
                $foreign_id = courier_connector_get_value($object_record, $parameter['foreign_id'][key($parameter['foreign_id'])]);                                    
                if($foreign_id != '') {                  
                  $new_data[key($parameter['foreign_id'])] = $foreign_id;
                }
              }
              
              //this is only if we want to use the relation through auto magic and not a related value, for not relation is fine
              //add any foreign relations, these are relations that are crossing item types, like items on an order
              //the items are related to QB and this tells us that that foreign_id on the related field is 
              //the value we want to use here
//              if(array_key_exists('foreign_relation', $parameter)) {                
//                //get the related object
//                $relation_data = $parameters[$parameter['foreign_relation']['object_type']];
//                foreach($relation_data as $r) {
//                  
//                  $id = courier_connector_get_value($object_record, $parameter['foreign_relation']['object_id']);
//                  
//                  $relation_object = $r['method']($id);
//                  
//                  $foreign_id = courier_connector_get_value($relation_object, $parameter['foreign_id'][key($parameter['foreign_id'])]);                                    
//                  
//                  if(!$foreign_id) {
//                    //this is possible if the item is not related, this case may be a good idea to put the creatation into the request job list                                        
//                  }
//                  else {                  
//                    $new_data[key($parameter['foreign_id'])] = $foreign_id;
//                  }  
//                }
//              }
              
              $record = array_merge($record, $new_data);

              if(array_key_exists("sub_arrays", $parameter)) {
                //sub array, a sub supporting array its inside the request, like items on an order
                foreach($parameter['sub_arrays'] as $sub_array_name => $sub_array_parameters) {

                  //$sub_parameters = $parameters[$sub_array_name];

                  $sub_array = courier_connector_get_value($reference_object, $sub_array_parameters['sub_array_key']);

                  if(is_array($sub_array)) {                

                    $sub_array_ids = array();

                    foreach($sub_array as $sub_record) {                  
                      if(is_object($sub_record)) {
                        $sub_record = get_object_vars($sub_record);
                      } 

                      $sub_array_ids[] = $sub_record[$sub_array_parameters['sub_id']];                  
                    }

                    $record = array_merge($record, courier_connector_process_request($job_id, $sub_array_name, $parameters, serialize(array($sub_array_name . '_ids' => $sub_array_ids)), $request_type, $requests_queued, true));
                  }
                } 
              }
            }
          }
        //}
        //put the order into the array
        if(count($record) > 0) {
          $data[$object_type . "s"][] = $record;
        }
      }        
    }

    //remove the order ids from the array
    unset($data[$object_type . '_ids']);
    
    if(!$return_values)
            
      if(array_key_exists($object_type . "_Request", $requests_queued)) {
        $requests_queued[$object_type . "_Request"][$object_type . "s"][] = $record;
      }
      else {
        $requests_queued[$object_type . "_Request"] = $data;    
      }
    else
      return $data;
  }
}

function courier_connector_process_request_parameters($parameters, $object_record, $object_type) {
  $record = array();
  
  $filter = null;
  
  if(array_key_exists("mapping_filter", $parameters))
    $filter = $parameters['mapping_filter'];
  
  $mapped_fields = courier_connector_get_mapped_field_list(strtolower($object_type) . "_request", $filter);
  
  foreach($mapped_fields as $field) {        
    $object_value = courier_connector_map_value(strtolower($object_type) . "_request", $field, $object_record);           
    
    if($object_value !== false && isset($object_value)) {
      $record[$field] = $object_value;      
    }                   
  }
  
  return $record;  
}