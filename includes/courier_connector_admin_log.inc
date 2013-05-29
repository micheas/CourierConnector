<?php

/**
 * Builds the log form.
 */
function courier_connector_logs_form(&$form_state) {
    
  //TODO, add a query filter for this logged data
  
  $form_state['request_log'] = array(
      '#type' => 'fieldset',
      '#title' => t('Request Log'),
      '#group' => 'courier-requests',
      '#weight' => 4,
      '#theme' => 'courier_connector_request_log_table',
      '#tree' => TRUE,  
  ); 

  $result = db_query("select *, courier_requests.job_id as 'jobid', courier_requests.job_date as 'request_date' from courier_requests left join courier_responses on courier_requests.job_id = courier_responses.job_id order by courier_requests.job_date desc");
    
  foreach ($result as $row) 
  {      
      //convert the database record from obj to array if need be
      if(is_array($row))
        $row = (array) $row;
            
      $form_state['request_log']['job_date'][$row['jobid']] = array('#value' => check_plain($row['request_date']));
      $form_state['request_log']['request_type'][$row['jobid']] = array('#value' => check_plain($row['request_type']));                                                        
      $request_data = var_export(unserialize($row['request_data']), true);
      $form_state['request_log']['request_data'][$row['jobid']] = array(    
                                                      '#type' => 'textarea',    
                                                      '#value' => "{$request_data}",
                                                      );
      $form_state['request_log']['processed_date'][$row['jobid']] = array('#value' => check_plain($row['processed_date']));
      $form_state['request_log']['response_xml'][$row['jobid']] = array(    
                                                      '#type' => 'textarea',    
                                                      '#value' => "{$row['response_xml']}",
                                                      );        
      $status = 'Error';                                            
      if($row['processed'] == 1 && $row['status'] == 'True') {
        $status = 'Processed';
      }
      else if($row['processed'] == 0) {
        $status = 'Queued';
      }
      else if($row['processed'] == 1) {
        $status = 'Awaiting Response';
      }       
                                                            
      $form_state['request_log']['status'][$row['jobid']] = array('#value' => check_plain($status));                                                                    
  }

//  return system_settings_form($form_state);
  return $form_state;
}

//function theme_courier_connector_request_log_table($variables) 
function theme_courier_connector_request_log_table($form)
{      
    //dpm($form);  
  
   $header = array(t('Request Date'), t('Request Type'), t('Request Data'), t('Processed Date'), t('Response XML'), t('Status'));
  
   foreach (element_children($form['job_date']) as $key) {     
     $rows[] = array(
        drupal_render($form['job_date'][$key]),
        drupal_render($form['request_type'][$key]),                
        drupal_render($form['request_data'][$key]),               
        drupal_render($form['processed_date'][$key]),
        drupal_render($form['response_xml'][$key]),        
        drupal_render($form['status'][$key])
      );       
    } 
    
    $output = theme('table', $header, $rows);
   
    return $output;
}