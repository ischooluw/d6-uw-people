<?php

/***************************************************************
 * Directory Profile Module                                    *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Shared functions for the dirprof module.
 */

module_load_include('inc', 'easycck');

module_load_include('inc', 'dirprof', 'dirprof_affiliation_field');
module_load_include('inc', 'dirprof', 'dirprof_collaborator_field');
module_load_include('inc', 'dirprof', 'dirprof_education_field');
module_load_include('inc', 'dirprof', 'dirprof_keywork_field');
module_load_include('inc', 'dirprof', 'dirprof_membership_field');
module_load_include('inc', 'dirprof', 'dirprof_project_field');
module_load_include('inc', 'dirprof', 'dirprof_recognition_field');


define('DIRPROF_SIMPLEPROF', 'DIRPROF_SIMPLEPROF');
define('DIRPROF_EXTENDEDPROF', 'DIRPROF_EXTENDEDPROF');
define('DIRPROF_PERM', 'change directory profile settings');


/**
 * Gets or sets a uwpeople module variable.
 */
function _dirprof_var($var, $val = NULL) {
  switch ($var) {
    case DIRPROF_SIMPLEPROF:
      if ($val)
        variable_set(DIRPROF_SIMPLEPROF, $val);
      else
        return variable_get(DIRPROF_SIMPLEPROF, array('staff',));
    case DIRPROF_EXTENDEDPROF:
      if ($val)
        variable_set(DIRPROF_EXTENDEDPROF, $val);
      else
        return variable_get(DIRPROF_EXTENDEDPROF, array('faculty', 'phd', 'researcher',));
  }
  
  return NULL;
}

/**
 * Logs a watchdog error.
 */
function _dirprof_error($msg, $var = NULL) {
  if ($var)
    watchdog('Directory Profile', $msg, array('%var' => $var,), WATCHDOG_ERROR);
  else
    watchdog('Directory Profile', $msg, array(), WATCHDOG_ERROR);
}

/**
 * Determines if the given individual is in the settings set.
 */
function _dirprof_has_prof($args, $settings) {
  return ($args['directories']['student'] && in_array('student', $settings)) ||
         ($args['directories']['phd'] && in_array('phd', $settings)) ||
         ($args['directories']['researcher'] && in_array('researcher', $settings)) ||
         ($args['directories']['faculty'] && in_array('faculty', $settings)) ||
         ($args['directories']['staff'] && in_array('staff', $settings));
}

/**
 * Determines if the given individual can have a simplified profile.
 */
function _dirprof_has_simple($args) {
  $settings = _dirprof_var(DIRPROF_SIMPLEPROF);
  return _dirprof_has_prof($args, $settings);
}

/**
 * Determines if the given individual can have an extended profile.
 */
function _dirprof_has_extended($args) {
  $settings = _dirprof_var(DIRPROF_EXTENDEDPROF);
  return _dirprof_has_prof($args, $settings);
}

/**
 * Loads a node.
 */
function _dirprof_node_load($type, $title) {
  return easycck_node_load(array(
    'type' => $type,
    'title' => $title,
  ));
}

/**
 * Reload simplified profile.
 */
function _dirprof_reload_simpleprof($regid, $nid) {
  $prof = _dirprof_node_load('simpleprof', $regid);
  
  if (!$prof) {
    $prof = easycck_node_create('simpleprof', $regid);
    
    if (!$prof)
      return FALSE;
  }
  
  easycck_set_noderef_field($prof, 'field_simpleprof_person', $nid);
  return easycck_node_save($prof);
}

/**
 * Reload extended profile.
 */
function _dirprof_reload_extendedprof($regid, $nid) {
  $prof = _dirprof_node_load('extendedprof', $regid);
  
  if (!$prof) {
    $prof = easycck_node_create('extendedprof', $regid);
    
    if (!$prof)
      return FALSE;
  }
  
  easycck_set_noderef_field($prof, 'field_extendedprof_person', $nid);
  return easycck_node_save($prof);
}


/**
 * Deletes "simpleprof" data for an individual.
 */
function _dirprof_delete_simpleprof($regid) {
  $extended = db_query("SELECT nid FROM {node} WHERE type = 'extendedprof' AND title = '%s'", $regid);
  
  if ($extended)
    easycck_node_delete(db_result($extended));
}

/**
 * Deletes "extendedprof" data for an individual.
 */
function _dirprof_delete_extendedprof($regid) {
  $simple = db_query("SELECT nid FROM {node} WHERE type = 'simpleprof' AND title = '%s'", $regid);
  
  if ($simple)
    easycck_node_delete(db_result($simple));
}
