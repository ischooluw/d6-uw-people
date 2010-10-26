<?php

/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Shared functions for the uwpeople module.
 */

module_load_include('inc', 'lts', 'lts.api');
module_load_include('inc', 'easycck');

module_load_include('inc', 'uwpeople', 'uwpeople_affiliation_field');
module_load_include('inc', 'uwpeople', 'uwpeople_collaborator_field');
module_load_include('inc', 'uwpeople', 'uwpeople_education_field');
module_load_include('inc', 'uwpeople', 'uwpeople_keywork_field');
module_load_include('inc', 'uwpeople', 'uwpeople_membership_field');
module_load_include('inc', 'uwpeople', 'uwpeople_project_field');
module_load_include('inc', 'uwpeople', 'uwpeople_recognition_field');

define('UWPEOPLE_ALL_PEOPLE_ROLE', 'UWPEOPLE_ALL_PEOPLE_ROLE');
define('UWPEOPLE_ALL_PEOPLE_ROLE_DEFAULT', '');

define('UWPEOPLE_IGNORE', 'UWPEOPLE_IGNORE');
define('UWPEOPLE_IGNORE_DEFAULT', '');

define('UWPEOPLE_FACULTY_ROLES', 'UWPEOPLE_FACULTY_ROLES');
define('UWPEOPLE_FACULTY_ROLES_DEFAULT', '');

define('UWPEOPLE_STAFF_ROLES', 'UWPEOPLE_STAFF_ROLES');
define('UWPEOPLE_STAFF_ROLES_DEFAULT', '');

define('UWPEOPLE_STUDENT_ROLES', 'UWPEOPLE_STUDENT_ROLES');
define('UWPEOPLE_STUDENT_ROLES_DEFAULT', '');

define('UWPEOPLE_RESEARCHER_ROLES', 'UWPEOPLE_RESEARCHER_ROLES');
define('UWPEOPLE_RESEARCHER_ROLES_DEFAULT', '');

define('UWPEOPLE_PHD_ROLES', 'UWPEOPLE_PHD_ROLES');
define('UWPEOPLE_PHD_ROLES_DEFAULT', '');

define('UWPEOPLE_PERM', 'manage uw people');

/**
 * Gets or sets a uwpeople module variable.
 */
function _uwpeople_var($var, $val = NULL) {
  switch ($var) {
    case UWPEOPLE_ALL_PEOPLE_ROLE:
      if ($val)
        variable_set(UWPEOPLE_ALL_PEOPLE_ROLE, $val);
      else
        return variable_get(UWPEOPLE_ALL_PEOPLE_ROLE, UWPEOPLE_ALL_PEOPLE_ROLE_DEFAULT);
    case UWPEOPLE_IGNORE:
      if ($val)
        variable_set(UWPEOPLE_IGNORE, $val);
      else
        return variable_get(UWPEOPLE_IGNORE, UWPEOPLE_IGNORE_DEFAULT);
    case UWPEOPLE_FACULTY_ROLES:
      if ($val)
        variable_set(UWPEOPLE_FACULTY_ROLES, $val);
      else
        return variable_get(UWPEOPLE_FACULTY_ROLES, UWPEOPLE_FACULTY_ROLES_DEFAULT);
    case UWPEOPLE_STAFF_ROLES:
      if ($val)
        variable_set(UWPEOPLE_STAFF_ROLES, $val);
      else
        return variable_get(UWPEOPLE_STAFF_ROLES, UWPEOPLE_STAFF_ROLES_DEFAULT);
    case UWPEOPLE_STUDENT_ROLES:
      if ($val)
        variable_set(UWPEOPLE_STUDENT_ROLES, $val);
      else
        return variable_get(UWPEOPLE_STUDENT_ROLES, UWPEOPLE_STUDENT_ROLES_DEFAULT);
    case UWPEOPLE_RESEARCHER_ROLES:
      if ($val)
        variable_set(UWPEOPLE_RESEARCHER_ROLES, $val);
      else
        return variable_get(UWPEOPLE_RESEARCHER_ROLES, UWPEOPLE_RESEARCHER_ROLES_DEFAULT);
    case UWPEOPLE_PHD_ROLES:
      if ($val)
        variable_set(UWPEOPLE_PHD_ROLES, $val);
      else
        return variable_get(UWPEOPLE_PHD_ROLES, UWPEOPLE_PHD_ROLES_DEFAULT);
  }
  
  return NULL;
}

/**
 * Logs a watchdog error.
 */
function _uwpeople_error($msg, $var = NULL) {
  if ($var)
    watchdog('UW People', $msg, array('%var' => $var,), WATCHDOG_ERROR);
  else
    watchdog('UW People', $msg, array(), WATCHDOG_ERROR);
}

/**
 * Acquires a lock to update the data.
 * @return Lock value [<code>String</code>] if successful; <code>FALSE</code> otherwise.
 */
function _uwpeople_lock() {
  if (!variable_get('uwpeople_lock', NULL)) {
    $token = drupal_get_token();
    variable_set('uwpeople_lock', $token);
    return $token;
  }
  
  return FALSE;
}

/**
 * Determines if a lock succeeded or failed.
 */
function _uwpeople_locked($lock) {
  return $lock ? TRUE : FALSE;
}

/**
 * Releases update lock.
 * @see _uwpeople_lock
 */
function _uwpeople_unlock($lock) {
  $token = variable_get('uwpeople_lock', NULL);
  
  if ($token == $lock)
    variable_del('uwpeople_lock');
}

/**
 * Gets all data for a UW NetID.
 */
function _uwpeople_data_for_netid($netid) {
  $ret = FALSE;
  
  for ($i = 0; $i < 5; ++$i) {
    $ret = lts_command('people data ' . $netid);
    
    if ($ret)
      break;
    
    usleep(10000);
  }
  
  return $ret;
}

/**
 * Gets the members of a role.
 */
function _uwpeople_members($role, $ignore, &$list) {
  $members = NULL;
  
  for ($i = 0; $i < 5; ++$i) {
    $members = lts_command('groups members ' . $role);
    
    if ($members)
      break;
    
    usleep(10000);
  }
  
  if (!$members)
    return FALSE;
  
  foreach ($members as $member) {
    if (!in_array($member, $ignore) && !in_array($member, $list))
      $list[] = $member;
  }
  
  return TRUE;
}

/**
 * Gets the UW NetIDs of all members.
 */
function _uwpeople_all_netids() {
  $ignore = explode("\n", _uwpeople_var(UWPEOPLE_IGNORE));
  
  foreach ($ignore as $delta => $item)
    $ignore[$delta] = trim($item);
  
  $groups = explode(',', _uwpeople_var(UWPEOPLE_ALL_PEOPLE_ROLE));
  $ret = array();
  
  foreach ($groups as $group) {
    if (connection_status() != CONNECTION_NORMAL)
      return FALSE;
    
    if (!_uwpeople_members($group, $ignore, $ret))
      _uwpeople_error('Could not fetch members for group "%var".', $group);
  }
  
  return $ret;
}

/**
 * Gets the roles of a UW NetID.
 */
function _uwpeople_netid_roles($netid) {
  $ret = NULL;
  
  for ($i = 0; $i < 5; ++$i) {
    $ret = lts_command('groups user ' . $netid);
    
    if ($ret)
      break;
    
    usleep(10000);
  }
  
  return $ret;
}

/**
 * Gets role settings.
 */
function _uwpeople_role_settings() {
  $ret = array(
    'faculty' => explode(',', _uwpeople_var(UWPEOPLE_FACULTY_ROLES)),
    'staff' => explode(',', _uwpeople_var(UWPEOPLE_STAFF_ROLES)),
    'students' => explode(',', _uwpeople_var(UWPEOPLE_STUDENT_ROLES)),
    'phd' => explode(',', _uwpeople_var(UWPEOPLE_PHD_ROLES)),
    'researchers' => explode(',', _uwpeople_var(UWPEOPLE_RESEARCHER_ROLES)),
  );
  
  foreach ($ret as $key => $roles) {
    foreach ($roles as $delta => $item)
      $roles[$delta] = trim($item);
  }
  
  return $ret;
}

/**
 * Checks NetID roles.
 */
function _uwpeople_in_setting($name, $roles, $settings) {
  $setting_roles = $settings[$name];
  
  foreach ($roles as $role) {
    if (in_array($role, $setting_roles))
      return TRUE;
  }
  
  return FALSE;
}

/**
 * Loads a node.
 */
function _uwpeople_node_load($type, $title) {
  return easycck_node_load(array(
    'type' => $type,
    'title' => $title,
  ));
}

/**
 * Reload simplified profile.
 */
function _uwpeople_reload_simpleprof($regid, $data, &$node) {
  $prof = _uwpeople_node_load('simpleprof', $regid);
  
  if (!$prof) {
    $prof = easycck_node_create('simpleprof', $regid);
    
    if (!easycck_node_save($prof))
      return FALSE;
  }
  
  easycck_set_noderef_field($node, 'field_person_simpleprof', $prof);
  
  return TRUE;
}

/**
 * Reload extended profile.
 */
function _uwpeople_reload_extendedprof($regid, $data, &$node) {
  $prof = _uwpeople_node_load('extendedprof', $regid);
  
  if (!$prof) {
    $prof = easycck_node_create('extendedprof', $regid);
    
    if (!easycck_node_save($prof))
      return FALSE;
  }
  
  easycck_set_noderef_field($node, 'field_person_extendedprof', $prof);
  
  return TRUE;
}

/**
 * Reload employee data.
 */
function _uwpeople_reload_employee($regid, $data, &$node) {
  if (empty($data['employee']))
    return FALSE;
  
  $employee = $data['employee'];
  $prof = _uwpeople_node_load('employee', $regid);
  
  if (!$prof) {
    $prof = easycck_node_create('employee', $regid);
    easycck_set_int_field($prof, 'field_employee_phone_use', 0);
    easycck_set_int_field($prof, 'field_employee_email_use', 0);
    easycck_set_int_field($prof, 'field_employee_title_use', 0);
    easycck_set_int_field($prof, 'field_employee_address_use', 0);
    easycck_set_int_field($prof, 'field_employee_publish', $employee['publish_in_directory'] ? 1 : 0);
  }
  
  easycck_set_text_field($prof, 'field_employee_phone_ws', $employee['phone']);
  easycck_set_text_field($prof, 'field_employee_email_ws', $employee['email']);
  easycck_set_text_field($prof, 'field_employee_title1_ws', $employee['title_1']);
  easycck_set_text_field($prof, 'field_employee_title2_ws', $employee['title_2']);
  easycck_set_text_field($prof, 'field_employee_address1_ws', $employee['address_1']);
  easycck_set_text_field($prof, 'field_employee_address2_ws', $employee['address_2']);
  
  easycck_set_noderef_field($node, 'field_person_employee', $prof);
  
  return easycck_node_save($prof);
}

/**
 * Reload student data.
 */
function _uwpeople_reload_student($regid, $data, &$node) {
  if (empty($data['student']))
    return FALSE;
  
  $student = $data['student'];
  $prof = _uwpeople_node_load('student', $regid);
  
  if (!$prof) {
    $prof = easycck_node_create('student', $regid);
    easycck_set_int_field($prof, 'field_student_email_use', 0);
    easycck_set_int_field($prof, 'field_student_publish', $student['publish_in_directory'] ? 1 : 0);
  }
  
  easycck_set_text_field($prof, 'field_student_email_ws', $student['email']);
  easycck_set_text_field($prof, 'field_student_class', $student['class']);
  easycck_set_text_field($prof, 'field_student_department1', $student['department_1']);
  easycck_set_text_field($prof, 'field_student_department2', $student['department_2']);
  easycck_set_text_field($prof, 'field_student_department3', $student['department_3']);
  
  easycck_set_noderef_field($node, 'field_person_student', $prof);
  
  return easycck_node_save($prof);
}

/**
 * Deletes employee data for an individual.
 */
function _uwpeople_delete_employee($regid) {
  $employee = db_query("SELECT nid FROM {node} WHERE type = 'employee' AND title = '%s'", $regid);
    
  if ($employee)
    easycck_node_delete(db_result($employee));
}

/**
 * Deletes student data for an individual.
 */
function _uwpeople_delete_student($regid) {
  $student = db_query("SELECT nid FROM {node} WHERE type = 'student' AND title = '%s'", $regid);
  
  if ($student)
    easycck_node_delete(db_result($student));
}

/**
 * Deletes "simpleprof" data for an individual.
 */
function _uwpeople_delete_simpleprof($regid) {
  $extended = db_query("SELECT nid FROM {node} WHERE type = 'extendedprof' AND title = '%s'", $regid);
  
  if ($extended)
    easycck_node_delete(db_result($extended));
}

/**
 * Deletes "extendedprof" data for an individual.
 */
function _uwpeople_delete_extendedprof($regid) {
  $simple = db_query("SELECT nid FROM {node} WHERE type = 'simpleprof' AND title = '%s'", $regid);
  
  if ($simple)
    easycck_node_delete(db_result($simple));
}

/**
 * Reload a persons data.
 */
function _uwpeople_reload_person($netid) {
  $data = _uwpeople_data_for_netid($netid);
  
  if (!$data) {
    _uwpeople_error('Failed to load data for "%var"', $netid);
    return FALSE;
  }
  
  $roles = _uwpeople_netid_roles($netid);
  
  if (!$roles) {
    _uwpeople_error('Failed to load role data for "%var"', $netid);
    return FALSE;
  }
  
  $settings = _uwpeople_role_settings();
  $person = $data['person'];
  $regid = $person['uwregid'];
  $node = _uwpeople_node_load('person', $regid);
  
  if (!$node) {
    $node = easycck_node_create('person', $regid);
    easycck_set_int_field($node, 'field_person_firstmiddle_use', 0);
    easycck_set_int_field($node, 'field_person_last_use', 0);
    easycck_set_int_field($node, 'field_person_publish', $person['publish_in_whitepages'] ? 1 : 0);
  }
  
  easycck_set_text_field($node, 'field_person_netid', $person['uwnetid']);
  easycck_set_text_field($node, 'field_person_firstmiddle_ws', $person['registered_first_middle_name']);
  easycck_set_text_field($node, 'field_person_last_ws', $person['registered_surname']);
  
  $faculty = _uwpeople_in_setting('faculty', $roles, $settings);
  $staff = _uwpeople_in_setting('staff', $roles, $settings);
  $student = _uwpeople_in_setting('students', $roles, $settings);
  $phd = _uwpeople_in_setting('phd', $roles, $settings);
  $researcher = _uwpeople_in_setting('researchers', $roles, $settings);
  
  if (!$faculty && !$staff && !$student && !$phd && !$researcher) {
    easycck_set_int_field($node, 'field_person_directory', 0, FALSE);
  }
  else {
    $dirs = array();
    
    if ($faculty || $staff || $researcher) {
      if (!_uwpeople_reload_employee($regid, $data, $node)) {
        _uwpeople_error('Failed to load employee data for "%var".', $netid);
        return FALSE;
      }
    }
    else {
      _uwpeople_delete_employee($regid);
    }
    
    if ($faculty || $phd || $researcher) {
      if (!_uwpeople_reload_extendedprof($regid, $data, $node)) {
        _uwpeople_error('Failed to load extended profile for "%var".', $netid);
        return FALSE;
      }
    }
    else {
      _uwpeople_delete_extendedprof($regid);
    }
    
    if ($staff) {
      if (!_uwpeople_reload_simpleprof($regid, $data, $node)) {
        _uwpeople_error('Failed to load simple profile for "%var".', $netid);
        return FALSE;
      }
    }
    else {
      _uwpeople_delete_simpleprof($regid);
    }
    
    if ($student || $phd) {
      if (!_uwpeople_reload_student($regid, $data, $node)) {
        _uwpeople_error('Failed to load student data for "%var".', $netid);
        return FALSE;
      }
    }
    else {
      _uwpeople_delete_student($regid);
    }
    
    if ($faculty)
      $dirs[] = 1;
    
    if ($staff)
      $dirs[] = 2;
    
    if ($phd)
      $dirs[] = 3;
    
    if ($researcher)
      $dirs[] = 4;
    
    if ($student)
      $dirs[] = 5;
    
    easycck_set_int_field($node, 'field_person_directory', $dirs[0], FALSE);
    
    for ($i = 1; $i < count($dirs); ++$i)
      easycck_set_int_field($node, 'field_person_directory', $dirs[$i], TRUE);
  }
  
  return easycck_node_save($node);
}

/**
 * Gets the people currently in the database.
 */
function _uwpeople_get_current() {
  $result = db_query("SELECT node.nid, node.title, MAX(node.vid), person.field_person_netid_value FROM {node} AS node LEFT JOIN {content_type_person} AS person ON node.nid = person.nid WHERE node.type = 'person' GROUP BY node.nid");
  
  if (!$result)
    return array();
  
  $data = array();
  
  while (($row = db_fetch_array($result))) {
    if (connection_status() != CONNECTION_NORMAL)
      return FALSE;
    
    $data[$row['nid']] = array(
      'title' => $row['title'],
      'netid' => $row['field_person_netid_value'],
    );
  }
  
  // nid => (title => TITLE, netid => NETID)
  
  return $data;
}

/**
 * Deletes records of a person.
 */
function _uwpeople_delete($nid, $data) {
  $regid = $data['title'];
  
  _uwpeople_delete_employee($regid);
  _uwpeople_delete_student($regid);
  _uwpeople_delete_simpleprof($regid);
  _uwpeople_delete_extendedprof($regid);
  
  easycck_node_delete($nid);
}

/**
 * "Cleans up."  Deletes all entries that no longer should exist.
 */
function _uwpeople_cleanup($netids) {
  $current = _uwpeople_get_current();
  
  foreach ($current as $nid => $data) {
    if (!in_array($data['netid'], $netids))
      _uwpeople_delete($nid, $data);
    
    usleep(10000);
  }
}

/**
 * Reload people data.
 */
function _uwpeople_reload_all() {
  cache_clear_all();
  
  $lock = _uwpeople_lock();
  
  if (!_uwpeople_locked($lock)) {
    _uwpeople_error('Reload already in progress.');
    return FALSE;
  }
  
  $netids = _uwpeople_all_netids();
  
  if (!$netids) {
    _uwpeople_error('Failed to load all netids with group "%var".', _uwpeople_var(UWPEOPLE_ALL_PEOPLE_ROLE));
    _uwpeople_unlock($lock);
    return FALSE;
  }
  
  _uwpeople_cleanup($netids);
  
  $timeout = ini_get('max_execution_time');
  ini_set('max_execution_time', 0);
  
  foreach ($netids as $netid) {
    if (!_uwpeople_reload_person($netid))
      _uwpeople_error('Failed to reload person "%var".', $netid);
    
    usleep(10000);
  }
  
  _uwpeople_unlock($lock);
  ini_set('max_execution_time', $timeout);
  cache_clear_all();
  
  return TRUE;
}
