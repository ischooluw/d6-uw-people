<?php

/***************************************************************
 * iPeople Module                                              *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Shared functions for the ipeople module.
 */

module_load_include('inc', 'easycck');


define('IPEOPLE_STUDENT_INSC_GROUPS', 'IPEOPLE_STUDENT_INSC_GROUPS');
define('IPEOPLE_STUDENT_INSC_GROUPS_DEFAULT', '');

define('IPEOPLE_STUDENT_MSIM_GROUPS', 'IPEOPLE_STUDENT_MSIM_GROUPS');
define('IPEOPLE_STUDENT_MSIM_GROUPS_DEFAULT', '');

define('IPEOPLE_STUDENT_MLIS_GROUPS', 'IPEOPLE_STUDENT_MLIS_GROUPS');
define('IPEOPLE_STUDENT_MLIS_GROUPS_DEFAULT', '');

define('IPEOPLE_STUDENT_INFO_GROUPS', 'IPEOPLE_STUDENT_INFO_GROUPS');
define('IPEOPLE_STUDENT_INFO_GROUPS_DEFAULT', '');

define('IPEOPLE_PERM', 'change ipeople settings');

define('IPEOPLE_INFO', 0);
define('IPEOPLE_MSIM', 1);
define('IPEOPLE_MLIS', 2);
define('IPEOPLE_INSC', 3);
define('IPEOPLE_NONE', 4);


/**
 * Gets or sets a uwpeople module variable.
 */
function _ipeople_var($var, $val = NULL) {
  switch ($var) {
    case IPEOPLE_STUDENT_INSC_GROUPS:
      if ($val)
        variable_set(IPEOPLE_STUDENT_INSC_GROUPS, $val);
      else
        return variable_get(IPEOPLE_STUDENT_INSC_GROUPS, IPEOPLE_STUDENT_INSC_GROUPS_DEFAULT);
    case IPEOPLE_STUDENT_MSIM_GROUPS:
      if ($val)
        variable_set(IPEOPLE_STUDENT_MSIM_GROUPS, $val);
      else
        return variable_get(IPEOPLE_STUDENT_MSIM_GROUPS, IPEOPLE_STUDENT_MSIM_GROUPS_DEFAULT);
    case IPEOPLE_STUDENT_MLIS_GROUPS:
      if ($val)
        variable_set(IPEOPLE_STUDENT_MLIS_GROUPS, $val);
      else
        return variable_get(IPEOPLE_STUDENT_MLIS_GROUPS, IPEOPLE_STUDENT_MLIS_GROUPS_DEFAULT);
    case IPEOPLE_STUDENT_INFO_GROUPS:
      if ($val)
        variable_set(IPEOPLE_STUDENT_INFO_GROUPS, $val);
      else
        return variable_get(IPEOPLE_STUDENT_INFO_GROUPS, IPEOPLE_STUDENT_INFO_GROUPS_DEFAULT);
  }
  
  return NULL;
}

/**
 * Logs a watchdog error.
 */
function _ipeople_error($msg, $var = NULL) {
  if ($var)
    watchdog('iPeople', $msg, array('%var' => $var,), WATCHDOG_ERROR);
  else
    watchdog('iPeople', $msg, array(), WATCHDOG_ERROR);
}

/**
 * Loads a node.
 */
function _ipeople_node_load($type, $title) {
  return easycck_node_load(array(
    'type' => $type,
    'title' => $title,
  ));
}

/**
 * Determines if any of the given roles are in a set of settings roles.
 */
function _ipeople_in_roles($roles, $settings) {
  foreach ($settings as $setting) {
    if (in_array($setting, $roles))
      return TRUE;
  }
  
  return FALSE;
}

/**
 * Explodes and trims a setting roles array.
 */
function _ipeople_setting_roles($setting) {
  $ret = explode(',', _ipeople_var($setting));
  
  foreach ($ret as $k => $v)
    $ret[$k] = trim($v);
  
  return $ret;
}

/**
 * Reload iStudent profile.
 */
function _ipeople_reload_istudent($regid, $nid, $roles) {
  $prof = _ipeople_node_load('istudent', $regid);
  
  if (!$prof) {
    $prof = easycck_node_create('istudent', $regid);
    
    if (!$prof)
      return FALSE;
  }
  
  easycck_set_noderef_field($prof, 'field_istudent_person', $nid);
  
  $settings = _ipeople_setting_roles(IPEOPLE_STUDENT_INSC_GROUPS);
  if (_ipeople_in_roles($roles, $settings)) {
    easycck_set_int_field($prof, 'field_istudent_program', IPEOPLE_INSC);
    return easycck_node_save($prof);
  }
  
  $settings = _ipeople_setting_roles(IPEOPLE_STUDENT_MSIM_GROUPS);
  if (_ipeople_in_roles($roles, $settings)) {
    easycck_set_int_field($prof, 'field_istudent_program', IPEOPLE_MSIM);
    return easycck_node_save($prof);
  }
  
  $settings = _ipeople_setting_roles(IPEOPLE_STUDENT_MLIS_GROUPS);
  if (_ipeople_in_roles($roles, $settings)) {
    easycck_set_int_field($prof, 'field_istudent_program', IPEOPLE_MLIS);
    return easycck_node_save($prof);
  }
  
  $settings = _ipeople_setting_roles(IPEOPLE_STUDENT_INFO_GROUPS);
  if (_ipeople_in_roles($roles, $settings)) {
    easycck_set_int_field($prof, 'field_istudent_program', IPEOPLE_INFO);
    return easycck_node_save($prof);
  }
  
  easycck_set_int_field($prof, 'field_istudent_program', IPEOPLE_NONE);
  return easycck_node_save($prof);
}

/**
 * Deletes "iStudent" data for an individual.
 */
function _ipeople_delete_istudent($regid) {
  $istudent = db_query("SELECT nid FROM {node} WHERE type = 'istudent' AND title = '%s'", $regid);
  
  if ($istudent)
    easycck_node_delete(db_result($istudent));
}
