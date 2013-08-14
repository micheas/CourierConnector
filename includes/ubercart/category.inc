<?php
/**
 * @file update the category.
 */

/**
 * @param $xml_parser
 * @return string
 */
function courier_get_categories($xml_parser) {
  //$vid = db_result(db_query("select vid from vocabulary where module = 'uc_catalog'"));
  db_query("select vid from {taxonomy_vocabulary} where module = 'uc_catalog'")->fetchField();

  $taxonomy = taxonomy_get_tree($vid);

  if (sizeof($taxonomy) == 0) {
    return "<getCategoryResult Status = \"OK\"><Category /></getCategoryResult>";
  }

  $categories = '<getCategoryResult Status = "OK">';

  foreach ($taxonomy as $category) {
    $categories .= "<Category ";

    $categories .= 'tid = "' . $category->tid . '" ';
    $categories .= 'name = "' . $category->name . '" ';
    $categories .= 'catalogID = "' . $vid . '" ';
    $categories .= '><CategoryParents>';
    foreach ($category->parents as $parent_id) {
      $categories .= '<Parent tid = "' . $parent_id . '" />';
    }

    $categories .= '</CategoryParents>';

    $categories .= "</Category>";
  }

  $categories .= "</getCategoryResult>";

  return $categories;
}

//update the Categories from the provided information
function courier_update_categories($xml_parser) {
  $status = "";
  $error_message = "";
  $warning_message = "";

  $id_list = array();
  $parent_list = array();

  foreach ($xml_parser->output[0]['child'] as $category_node) {
    try {
      $term_data = array();

      //set the explicitly set id
      if (isset($category_node['attrs']['CATALOGID']) && $category_node['attrs']['CATALOGID'] != "") {
        $term_data['vid'] = $category_node['attrs']['CATALOGID'];
      }
      else {
        if (isset($category_node['attrs']['CATALOG_NAME']) && $category_node['attrs']['CATALOG_NAME'] != "") {
          //find the id from the name specified
          //$termData['vid'] = db_result(db_query("select vid from vocabulary where module = 'uc_catalog' and name = '%s'", $categoryNode['attrs']['CATALOG_NAME']));
          $term_data['vid'] = db_query("select vid from {taxonomy_vocabulary} where module = 'uc_catalog' and name = :name", array(":name" => $category_node['attrs']['CATALOG_NAME']))->fetchField();
        }
      }

      if (isset($category_node['attrs']['NAME']) && $category_node['attrs']['NAME'] != "") {
        $term_data['name'] = $category_node['attrs']['NAME'];
      }

      //if(isset($categoryNode['attrs']['DESCRIPTION']) && $categoryNode['attrs']['DESCRIPTION'] != "")
      //{
      //    $termData['description'] = $categoryNode['attrs']['DESCRIPTION'];
      //}

      //if(isset($categoryNode['attrs']['WEIGHT']) && $categoryNode['attrs']['WEIGHT'] != "")
      //{
      //    $termData['weight'] = $categoryNode['attrs']['WEIGHT'];
      //}

      //set the parent taxonomy
      foreach ($category_node['child'] as $parent_record) {
        if (isset($parent_record['attrs']['PARENTID']) && $parent_record['attrs']['PARENTID'] != "") {
          //set the parent id
          $term_data['parent'] = array($parent_record['attrs']['PARENTID']);
        }
        else {
          if (isset($parent_record['attrs']['PARENTNAME']) && $parent_record['attrs']['PARENTNAME'] != "") {
            //need to find the taxonomy for this data, the name is presented Parent:parent:parent:category
            $parent_names = split(":", $parent_record['attrs']['PARENTNAME']);

            //    if(sizeof($parentNames > 0))
            //    {
            //track which level we are looking for
            //        $level = 0;
            //        $pid = 0;
            //        foreach($parentNames as $name)
            //        {
            //            if($name != $categoryNode['attrs']['NAME'])
            //            {
            //                $term = taxonomy_get_term_by_name($name);

            //return $name . " " . var_export($term[0], true) . "\n";

            //return $term[0]->tid;
            //                $x = taxonomy_get_parents($term[0]->tid);

            //return $term['tid'];
            //                return var_export($x, true);
            //                if(taxonomy_get_parents($term[0]->tid) == $level)
            //                {

            //                }

            //            }
            //            else
            //            {
            //                break;
            //            }
            //        }

            //        $termData['parent'] = array($pid);
            //    }

            //will work for now, but not fool proof, just takes the +1 parent and finds its id
            if (sizeof($parent_names > 1)) {
              //return $parentNames[sizeof($parentNames) - 2];

              $term = taxonomy_get_term_by_name($parent_names[sizeof($parent_names) - 2]);

              $term_data['parent'] = array($term[0]->tid);
            }
          }
        }
      }

      $term = (object) $term_data;
      taxonomy_term_save($term);

      //save the relation data
      $id_list[$category_node['attrs']['DATAID']] = $term->tid;
    } catch (Exception $ex) {
      $error_message .= $ex->getMessage();
    }
  }

  //return the error message if one exists
  if ($error_message != "") {
    $status = "<CategoryUpdateResult Status = \"Error\">";
    $status .= "<Error Message=\"" . $error_message . "\" />";
  }
  else {
    //check if there is any warnings
    if ($warning_message != "") {
      $status = "<CategoryUpdateResult Status = \"Warnings\">";
      $status .= "<Warning Message=\"" . $warning_message . "\" />";
    }
    else {
      //no errors or warnings mark that status
      $status = "<CategoryUpdateResult Status = \"OK\">";
    }

    //generate a return message
    foreach ($id_list as $dataid => $id) {
      $status .= "<Category ID=\"" . $id . "\" DataID = \"" . $dataid . "\" />";
    }
  }

  $status .= "</CategoryUpdateResult>";

  return $status;
}

