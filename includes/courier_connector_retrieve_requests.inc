<?php
//special object_ids in json
//id - the id that is being passed in
//object - the object that is being passed in
//anything else will be looked up on the mapping

function courier_connector_retrieve_requests() {
  require_once 'courier_connector_mapping.inc';

  $request_parameters = file_get_contents(drupal_get_path('module', 'courier_connector_uber_7') . "/includes/" . "request_parameters.json");
  $request_parameters=json_decode($request_parameters,true);

  $jobs = array();
  
  //get a list of the job ids that we are going to process here
  $result = db_query("Select job_id from courier_requests where processed = 0 order by job_date desc");
  
  $job_number = 0;
  
  $jobs['key'] = 'job_request';
  
  while($row = db_fetch_array($result)) { 
        
    $request_data = db_query("select job_id, job_date, request_type, request_method, request_data from courier_requests where job_id = %d order by job_date asc", $row['job_id']);
    
    $courier_requests = array();

    while($record = db_fetch_array($request_data)) {
   
      courier_connector_process_request($record['job_id'], $record['request_method'], $request_parameters, $record['request_data'], $record['request_type'], $courier_requests);
      
      $courier_requests["attrs"] = array("job_id" => $row['job_id'], "job_number" => $job_number);           
    }

    db_query("update courier_requests set processed = 1, processed_date = '%s' where job_id = %d", date( 'Y-m-d H:i:s', time()), $row['job_id']);

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
        
        foreach($parameters as $parameter_type) {  
          foreach($parameter_type as $parameter) {          
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

                  $sub_parameters = courier_connector_get_request_parameters($sub_method);

                  $sub_method_id_value = courier_connector_get_value($object_record, $sub_method_id);

                  courier_connector_process_request($job_id, $sub_method, $sub_parameters, serialize(array($sub_method . '_ids' => array($sub_method_id_value))), $request_type, $requests_queued);             
                } 
              }                          

              //process the method request
              $record = array_merge($record, courier_connector_process_request_parameters($parameter, $object_record, $object_type));

              if(array_key_exists("sub_arrays", $parameter)) {
                //sub array, a sub supporting array its inside the request, like items on an order
                foreach($parameter['sub_arrays'] as $sub_array_name => $sub_array_parameters) {

                  $sub_parameters = courier_connector_get_request_parameters($sub_array_name);

                  $sub_array = courier_connector_get_value($reference_object, $sub_array_parameters['sub_array_key']);

                  if(is_array($sub_array)) {                

                    $sub_array_ids = array();

                    foreach($sub_array as $sub_record) {                  
                      if(is_object($sub_record)) {
                        $sub_record = get_object_vars($sub_record);
                      } 

                      $sub_array_ids[] = $sub_record[$sub_array_parameters['sub_id']];                  
                    }

                    $record = array_merge($record, courier_connector_process_request($job_id, $sub_array_name, $sub_parameters, serialize(array($sub_array_name . '_ids' => $sub_array_ids)), $request_type, $requests_queued, true));
                  }
                } 
              }
            }
          }
        }
        //put the order into the array
        if(count($record) > 0) {
          $data[$object_type . "s"][] = $record;
        }
      }        
    }

    //remove the order ids from the array
    unset($data[$object_type . '_ids']);
    
    if(!$return_values)
      $requests_queued[$object_type . "_Request"] = $data;    
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