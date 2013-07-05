<?php

//get a list of fields that have been mapped, this will be iterated to do the assignment
function courier_connector_get_mapped_field_list($type, $filter = NULL) {
  if (courier_connector_get_drupal_version() == "6") {
    $query = "select distinct to_field from courier_mapping_to_{$type}";

    if (isset($filter)) {
      $query .= " where to_field like {$filter}%";
    }
    $q = db_query($query);
    while ($row = db_fetch_array($q)) {
      $ret[] = $row['to_field'];
    }

    return $ret;
  }

  $query = db_select("courier_mapping_to_{$type}", "map_to");
  $query->fields("map_to", array("to_field"));

  if (isset($filter)) {
    $query->condition("to_field", $filter . "%", "LIKE");
  }

  $results = $query->execute();

  $ret = array();
  foreach ($results as $result) {
    $ret[] = $result->to_field;
  }

  return $ret;
}

function courier_connector_map_value($type, $to_field, $record) {

  $mapped_fields = array();

  if (courier_connector_get_drupal_version() == "6") {
    $result = db_query("select map_to.default_value, map_to.commands, map_from.from_field, map_from.from_alternative, map_from.sort_order, map_from.commands from courier_mapping_to_{$type} map_to left join courier_mapping_from_{$type} map_from on map_from.to_{$type}_field_id = map_to.to_{$type}_field_id where map_to.to_field = '{$to_field}' order by sort_order asc");
    $mapped_fields = db_fetch_array($result);
  }
  else {
    $query = db_select("courier_mapping_to_{$type}", "map_to");
    $query->fields("map_to", array("default_value", "commands"));
    $query->fields("map_from", array("from_field", "from_alternative", "sort_order", "commands"));
    $query->leftJoin("courier_mapping_from_{$type}", "map_from", "map_from.to_{$type}_field_id = map_to.to_{$type}_field_id");
    $query->condition("map_to.to_field", $to_field);
    $query->orderBy("sort_order", "ASC");

    $mapped_fields = $query->execute();
  }

  $return_value = FALSE;

  foreach ($mapped_fields as $mapped_field) {
    $mapped_field = (array) $mapped_field;

    //process the logic according to the mapping          

    $from_field = $mapped_field['from_field'];

    if (isset($from_field)) {
      $from_value = courier_connector_get_value($record, $from_field);

      //check if the field specifed is null    
      if ($from_value == '') {
        $from_alternative = $mapped_field['from_alternative'];
        //$from_alternative_value = (string) trim($record->$from_alternative);
        $from_alternative_value = courier_connector_get_value($record, $from_alternative);

        if ($from_alternative_value == '') {
          if ($mapped_field['default_value'] != '') {
            $return_value .= trim($mapped_field['default_value']);
          }

        }
        else {
          $from_alternative_value = courier_connector_process_commands($record, $from_alternative_value, $from_alternative, $mapped_field['map_from_commands']);
          $return_value .= $from_alternative_value;
        }
      }
      else {
        $from_value = courier_connector_process_commands($record, $from_value, $from_field, $mapped_field['map_from_commands']);
        $return_value .= $from_value;
      }
    }
    else {
      //run the commands
      $return_value = courier_connector_process_commands($record, $mapped_field['default_value'], $to_field, $mapped_field['commands']);
    }
  }

  return $return_value;
}

function courier_connector_get_value($record, $field_name) {
  if (is_object($record)) {
    $record = get_object_vars($record);
  }

  if (strpos($field_name, ':') !== FALSE) {
    $fields = explode(':', $field_name);

    $field_value = $record;

    foreach ($fields as $field) {
      if (array_key_exists($field, $field_value)) {
        $field_value = $field_value[$field];
      }
      else {
        return FALSE;
      }
    }

    if (is_array($field_value)) {
      return $field_value;
    }

    return trim((string) $field_value);
  }

  if (array_key_exists($field_name, $record)) {

    if (is_array($record[$field_name])) {
      return $record[$field_name];
    }

    return trim((string) $record[$field_name]);
  }

  return FALSE;
}

//ADD IN ACTIVE COMMAND, to mapping, if active is false ignore the mapping, dont push any value


/*
 * Process the record against a list of commands
 */
function courier_connector_process_commands($record, $field_value, $field_name, $commands) {

  if (isset($commands)) {
    //break the commands into the individual ones 

    $command_list = explode('|', $commands);

    if (is_array(($command_list))) {

      foreach ($command_list as $command) {
        //get the parameters list
        preg_match_all('/(.*?)\((.*?)\)/', $commands, $parameters);

        include_once "commands/{$parameters[1][0]}.inc";

        $c = (string) "courier_connector_command_{$parameters[1][0]}";

        $field_value = $c($field_value, $field_name, $parameters[2][0], $record);
      }
    }
  }

  return $field_value;
}


