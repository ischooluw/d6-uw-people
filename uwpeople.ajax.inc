<?php

/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Contains the AJAX interface for the UW People module.
 */

function _uwpeople_ajax_set_progress($percent, $msg) {
  if (!$msg) {
    variable_del('uwpeople_ajax_progress');
    variable_del('uwpeople_ajax_message');
  }
  else {
    variable_set('uwpeople_ajax_progress', strval(intval($percent)));
    variable_set('uwpeople_ajax_message', strval($msg));
  }
}

function _uwpeople_ajax_get_token() {
  $token = NULL;
  
  if (!variable_get('uwpeople_ajax_token', NULL)) {
    $token = drupal_get_token();
    variable_set('uwpeople_ajax_token', $token);
    _uwpeople_ajax_set_progress(NULL, NULL);
  }
  
  return $token;
}

function _uwpeople_js_include($token) {
  $settings = array(
    'destination_path' => '/admin/settings/uwpeople',
    'redirect_timeout' => 1500,
    'ajax_url' => '/admin/uwpeople/ajax',
    'ajax_token' => $token,
    'refresh_timeout' => 1000,
  );
  
  drupal_add_js('misc/progress.js', 'module');
  drupal_add_js(array('uwpeople' => $settings, ), 'setting');
  drupal_add_js(drupal_get_path('module', 'uwpeople') . '/uwpeople.js', 'module');
}

function _uwpeople_refresh_page() {
  if (!user_access(UWPEOPLE_PERM)) {
    drupal_set_message(t('You are unauthorized to refresh people data.'), 'error');
    _uwpeople_error('Unauthorized user attempted to refresh data.');
  }
  else {
    $token = _uwpeople_ajax_get_token();
    
    if (!$token)
      drupal_set_message(t('Another user is currently refreshing people data.'), 'error');
    else
      _uwpeople_js_include($token);
  }
  
  return '<div id="#message"></div><br /><div id="uwpeople_progress_container"></div><br />' .
  '<div id="uwpeople_refresh_form_container"><form id="uwpeople_refresh_form" method="post" action="">' .
  '<input id="uwpeople-refresh-cancel" class="form-submit" type="submit" value="Cancel" /></form></div>';
}

function _uwpeople_ajax_cleanup($netids) {
  _uwpeople_ajax_set_progress(-1, 'Preparing to clean...');
  
  $current = _uwpeople_get_current();
  
  if (!$current) {
    _uwpeople_error('AJAX refresh canceled.');
    return FALSE;
  }
  
  $total = count($current);
  $i = 1;
  
  foreach ($current as $nid => $data) {
    if (connection_status() != CONNECTION_NORMAL) {
      _uwpeople_error('AJAX refresh canceled.');
      return FALSE;
    }
    
    _uwpeople_ajax_set_progress(($i / $total) * 100, "Step 1/2 - Cleaning ($i / $total)");
    
    if (!in_array($data['netid'], $netids))
      _uwpeople_delete($nid, $data);
    
    ++$i;
    usleep(10000);
  }
  
  return TRUE;
}

function _uwpeople_critical_error($errno, $errstr, $errfile, $errline) {
  ob_start();
  debug_print_backtrace();
  $backtrace = ob_end_clean();
  
  _uwpeople_error("Critical error for $errfile:$errline - $errstr ($errno)\nBacktrace: $backtrace");
  variable_del('uwpeople_ajax_token');
  cache_clear_all();
  
  exit(1);
  
  return FALSE;
}

function _uwpeople_ajax_start($token) {
  drupal_json();
  cache_clear_all();
  
  if (variable_get('uwpeople_ajax_token', NULL) !== $token) {
    echo drupal_to_js(array("error" => TRUE, ));
    return;
  }
  
  set_error_handler('_uwpeople_critical_error', E_ALL ^ E_USER_WARNING ^ E_NOTICE);
  
  $timeout = ini_get('max_execution_time');
  $ignore = ini_get('ignore_user_abort');
  ini_set('max_execution_time', 0);
  ignore_user_abort(TRUE);
  
  _uwpeople_ajax_set_progress(-1, 'Initalizing...');
  
  $netids = _uwpeople_all_netids();
  
  if (!$netids) {
    _uwpeople_error('Failed to load all netids with group "%var".', _uwpeople_var(UWPEOPLE_ALL_PEOPLE_ROLE));
    return FALSE;
  }
  
  $total = count($netids) + 1;
  $current = 1;
  $error = FALSE;
  
  if (_uwpeople_ajax_cleanup($netids)) {
    foreach ($netids as $netid) {
      echo "\n";
      flush();
      
      if (connection_status() != CONNECTION_NORMAL) {
        _uwpeople_error('AJAX refresh canceled.');
        break;
      }
      
      _uwpeople_ajax_set_progress(($current / $total) * 100, "Step 2/2 - Loading data ($current / $total)");
      
      if (!_uwpeople_reload_person($netid))
        _uwpeople_error('Failed to reload person "%var".', $netid);
      
      ++$current;
      usleep(10000);
    }
    
    _uwpeople_ajax_set_progress(100, 'Done');
  }
  else {
    $error = TRUE;
  }
  
  variable_del('uwpeople_ajax_token');
  cache_clear_all();
  
  echo drupal_to_js(array("error" => $error, ));
  
  ini_set('max_execution_time', $timeout);
  ini_set('ignore_user_abort', $ignore);
}

function _uwpeople_ajax_progress($token) {
  drupal_json(array(
    "progress" => intval(variable_get('uwpeople_ajax_progress', 0)),
    "message" => variable_get('uwpeople_ajax_message', 'N/A'),
    "error" => FALSE,
  ));
}

function _uwpeople_ajax_finish($token) {
  if (variable_get('uwpeople_ajax_token', NULL) !== $token) {
    drupal_json(array("error" => TRUE, ));
    return;
  }
  
  _uwpeople_ajax_set_progress(NULL, NULL);
  drupal_json(array("error" => FALSE, ));
}

function _uwpeople_ajax_page() {
  if (!$_POST['action'] || !$_POST['token']) {
    drupal_json(array("error" => TRUE, ));
    return;
  }
  
  switch ($_POST['action']) {
    case 'start':
      _uwpeople_ajax_start($_POST['token']);
      break;
    case 'progress':
      _uwpeople_ajax_progress($_POST['token']);
      break;
    case 'finish':
      _uwpeople_ajax_finish($_POST['token']);
      break;
  }
}
