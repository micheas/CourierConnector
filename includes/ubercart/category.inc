<?php

function courier_get_categories($xmlParser) {
  //$vid = db_result(db_query("select vid from vocabulary where module = 'uc_catalog'"));
  db_query("select vid from taxonomy_vocabulary where module = 'uc_catalog'")->fetchField();

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
    foreach ($category->parents as $parentID) {
      $categories .= '<Parent tid = "' . $parentID . '" />';
    }

    $categories .= '</CategoryParents>';

    $categories .= "</Category>";
  }

  $categories .= "</getCategoryResult>";

  return $categories;
}

//update the Categories from the provided information
function courier_update_categories($xmlParser) {
  $status = "";
  $errorMessage = "";
  $warningMessage = "";

  $idList = array();
  $parentList = array();

  foreach ($xmlParser->output[0][child] as $categoryNode) {
    try {
      $termData = array();

      //set the explicitly set id
      if (isset($categoryNode['attrs']['CATALOGID']) && $categoryNode['attrs']['CATALOGID'] != "") {
        $termData['vid'] = $categoryNode['attrs']['CATALOGID'];
      }
      else {
        if (isset($categoryNode['attrs']['CATALOG_NAME']) && $categoryNode['attrs']['CATALOG_NAME'] != "") {
          //find the id from the name specified
          //$termData['vid'] = db_result(db_query("select vid from vocabulary where module = 'uc_catalog' and name = '%s'", $categoryNode['attrs']['CATALOG_NAME']));
          $termData['vid'] = db_query("select vid from taxonomy_vocabulary where module = 'uc_catalog' and name = :name", array(":name" => $categoryNode['attrs']['CATALOG_NAME']))->fetchField();
        }
      }

      if (isset($categoryNode['attrs']['NAME']) && $categoryNode['attrs']['NAME'] != "") {
        $termData['name'] = $categoryNode['attrs']['NAME'];
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
      foreach ($categoryNode['child'] as $parentRecord) {
        if (isset($parentRecord['attrs']['PARENTID']) && $parentRecord['attrs']['PARENTID'] != "") {
          //set the parent id
          $termData['parent'] = array($parentRecord['attrs']['PARENTID']);
        }
        else {
          if (isset($parentRecord['attrs']['PARENTNAME']) && $parentRecord['attrs']['PARENTNAME'] != "") {
            //need to find the taxonomy for this data, the name is presented Parent:parent:parent:category
            $parentNames = split(":", $parentRecord['attrs']['PARENTNAME']);

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
            if (sizeof($parentNames > 1)) {
              //return $parentNames[sizeof($parentNames) - 2];

              $term = taxonomy_get_term_by_name($parentNames[sizeof($parentNames) - 2]);

              $termData['parent'] = array($term[0]->tid);
            }
          }
        }
      }

      $term = (object) $termData;
      taxonomy_term_save($term);

      //save the relation data
      $idList[$categoryNode['attrs']['DATAID']] = $term->tid;
    } catch (Exception $ex) {
      $errorMessage .= $ex->getMessage();
    }
  }

  //return the error message if one exists
  if ($errorMessage != "") {
    $status = "<CategoryUpdateResult Status = \"Error\">";
    $status .= "<Error Message=\"" . $errorMessage . "\" />";
  }
  else {
    //check if there is any warnings
    if ($warningMessage != "") {
      $status = "<CategoryUpdateResult Status = \"Warnings\">";
      $status .= "<Warning Message=\"" . $warningMessage . "\" />";
    }
    else {
      //no errors or warnings mark that status
      $status = "<CategoryUpdateResult Status = \"OK\">";
    }

    //generate a return message
    foreach ($idList as $dataid => $id) {
      $status .= "<Category ID=\"" . $id . "\" DataID = \"" . $dataid . "\" />";
    }
  }

  $status .= "</CategoryUpdateResult>";

  return $status;
}

