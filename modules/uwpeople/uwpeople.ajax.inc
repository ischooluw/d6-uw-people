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

/**
 * Sets the progress percentage and message to be displayed to the client.
 */
function _uwpeople_ajax_set_progress($percent, $msg, $error = FALSE) {
  if (!$msg) {
    variable_del('uwpeople_ajax_progress');
    variable_del('uwpeople_ajax_message');
    variable_del('uwpeople_ajax_error');
  }
  else {
    variable_set('uwpeople_ajax_progress', strval(intval($percent)));
    variable_set('uwpeople_ajax_message', strval($msg));
    variable_set('uwpeople_ajax_error', $error);
  }
}

/**
 * Determines if the AJAX operation was canceled.
 */
function _uwpeople_ajax_canceled() {
  for ($i = 0; $i < 5; ++$i) {
    echo "\n";
    flush();
    @ob_end_flush();
    
    usleep(5000);
    
    if (connection_aborted()) {
      _uwpeople_error('AJAX refresh canceled.');
      return TRUE;
    }
  }
  
  return FALSE;
}

/**
 * Gets an AJAX token to use.
 */
function _uwpeople_ajax_get_token() {
  $token = variable_get('uwpeople_lock', NULL);
  variable_set('uwpeople_ajax_token', $token);
  
  return $token;
}

/**
 * Includes needed JavaScript for the AJAX refresh page.
 */
function _uwpeople_js_include($token) {
  $settings = array(
    'destination_path' => '/admin/settings/uwpeople',
    'redirect_timeout' => 2000,
    'ajax_url' => '/admin/uwpeople/ajax',
    'ajax_token' => $token,
    'refresh_timeout' => 1000,
  );
  
  drupal_add_js('misc/progress.js', 'module');
  drupal_add_js(array('uwpeople' => $settings, ), 'setting');
  drupal_add_js(drupal_get_path('module', 'uwpeople') . '/uwpeople.js', 'module');
}

/**
 * Callback for the AJAX refresh page.
 */
function _uwpeople_refresh_page() {
  if (!user_access(UWPEOPLE_PERM)) {
    drupal_set_message(t('You are unauthorized to refresh people data.'), 'error');
    _uwpeople_error('Unauthorized user attempted to refresh data.');
  }
  else {
    $lock = _uwpeople_lock();
    
    if (_uwpeople_locked($lock)) {
      _uwpeople_js_include(_uwpeople_ajax_get_token());
    }
    else {
      drupal_set_message(t('Another user is currently refreshing people data.'), 'error');
      _uwpeople_js_include(NULL);
    }
  }
  
  return '<h1>UW People Refresh</h1>' .
  '<div id="people_refresh_message"></div><br />' .
  '<div id="uwpeople_progress_container"></div><br />' .
  '<div id="uwpeople_refresh_form_container"><form id="uwpeople_refresh_form" method="post" action="">' .
  '<input id="uwpeople-refresh-cancel" class="form-submit" type="submit" value="Cancel" /></form></div>';
}

/**
 * AJAX version of <code>_uwpeople_cleanup</code>.
 * @see _uwpeople_cleanup
 */
function _uwpeople_ajax_cleanup($netids) {
  _uwpeople_ajax_set_progress(-1, 'Preparing to clean...');
  
  $current = _uwpeople_get_current();
  
  if ($current === FALSE) {
    _uwpeople_error('AJAX refresh canceled.');
    return FALSE;
  }
  
  $total = count($current);
  $i = 1;
  
  foreach ($current as $nid => $data) {
    if (_uwpeople_ajax_canceled())
      return FALSE;
    
    _uwpeople_ajax_set_progress(($i / $total) * 100, "Step 1/2 - Cleaning ($i / $total)");
    
    if (!in_array($data['netid'], $netids))
      _uwpeople_delete($nid, $data);
    
    ++$i;
  }
  
  return TRUE;
}

function _uwpeople_argstr($arg) {
  switch (strtolower(gettype($arg))) {
    case 'string':
      return ('"' . str_replace(array("\n"), array(''), $arg ) . '"');
    case 'boolean':
      return (bool)$arg;
    case 'object':
      return 'object(' . get_class($arg) . ')';
    case 'array':
      $ret = 'array(';
      $sep = '';
  
      foreach ($arg as $k => $v) {
        $ret .= $sep . _uwpeople_argstr($k) . ' => ' . _uwpeople_argstr($v);
        $sep = ', ';
      }
      
      $ret .= ')';
  
      return $ret;
    case 'resource':
      return 'resource('.get_resource_type($arg).')';
    default:
      return var_export($arg, true);
  }
}

/**
 * Gets a backtrace.
 */
function _uwpeople_debug_backtrace() {
  $backtrace = "\n";
  $bdat = array_reverse(debug_backtrace());
  
  foreach ($bdat as $v) {
    $backtrace .= "\tin function " . $v['function'].  '(';
    $sep = '';
    
    if (!empty($v['args'])) {
      foreach ($v['args'] as $arg) {
        $backtrace .= ($sep . _uwpeople_argstr($arg));
        $sep = ', ';
      }
    }
    
    $backtrace .= ")\n";
  }
  
  return $backtrace;
}

/**
 * Handles critical errors in the the AJAX handler to ensure a clean shutdown.
 */
function _uwpeople_critical_error($errno, $errstr, $errfile, $errline) {
  $backtrace = _uwpeople_debug_backtrace();
  
  _uwpeople_error("Critical error for $errfile:$errline - $errstr ($errno) $backtrace");
  _uwpeople_ajax_set_progress(-1, 'Critical error, please check watchdog.', TRUE);
  _uwpeople_unlock(variable_get('uwpeople_ajax_token', NULL));
  
  cache_clear_all();
  
  exit(1);
  
  return FALSE;
}

/**
 * AJAX version of <code>_uwpeople_reload_all</code>.
 * @see _uwpeople_reload_all
 */
function _uwpeople_ajax_start($token) {
  $timeout = ini_get('max_execution_time');
  $ignore = ini_get('ignore_user_abort');
  
  drupal_json();
  cache_clear_all();
  
  if (variable_get('uwpeople_ajax_token', NULL) !== $token) {
    echo drupal_to_js(array("error" => TRUE, ));
    return;
  }
  
  $lock = $token;
  set_error_handler('_uwpeople_critical_error', E_ALL ^ E_USER_WARNING ^ E_NOTICE);
  
  _uwpeople_ajax_set_progress(-1, 'Initializing...');
  
  $netids = _uwpeople_all_netids();
  if (!$netids) {
    _uwpeople_error('Failed to load all NetIDs.');
    _uwpeople_ajax_set_progress(-1, 'Error loading all NetIDs.', TRUE);
    _uwpeople_unlock($lock);
    
    echo drupal_to_js(array("error" => TRUE, ));
    flush();
    
    return;
  }
  
  ini_set('max_execution_time', 0);
  ignore_user_abort(TRUE);
  
  $total = count($netids) + 1;
  $current = 1;
  $error = FALSE;
  
  if (_uwpeople_ajax_cleanup($netids)) {
    foreach ($netids as $netid) {
      if (_uwpeople_ajax_canceled()) {
        $error = TRUE;
        break;
      }
      
      _uwpeople_ajax_set_progress(($current / $total) * 100, "Step 2/2 - Loading data ($current / $total)");
      
      if (!_uwpeople_reload_person($netid))
        _uwpeople_error('Failed to reload person "%var".', $netid);
      
      ++$current;
    }
    
    if (!$error)
      _uwpeople_ajax_set_progress(100, 'Done');
    else
      _uwpeople_ajax_set_progress(-1, 'Canceled', TRUE);
  }
  else {
    _uwpeople_ajax_set_progress(-1, 'Error cleaning.', TRUE);
    $error = TRUE;
  }
  
  _uwpeople_unlock($lock);
  cache_clear_all();
  
  echo drupal_to_js(array("error" => $error, ));
  flush();
  
  ini_set('max_execution_time', $timeout);
  ini_set('ignore_user_abort', $ignore);
}

/**
 * Callback for AJAX script - get current progress.
 */
function _uwpeople_ajax_progress($token) {
  if (variable_get('uwpeople_ajax_token', NULL) !== $token) {
    drupal_json(array("error" => TRUE, ));
    return;
  }
  
  drupal_json(array(
    "progress" => intval(variable_get('uwpeople_ajax_progress', 0)),
    "message" => variable_get('uwpeople_ajax_message', 'N/A'),
    "error" => variable_get('uwpeople_ajax_error', TRUE),
  ));
}

/**
 * Callback for AJAX script - finish execution.
 */
function _uwpeople_ajax_finish($token) {
  if (variable_get('uwpeople_ajax_token', NULL) !== $token) {
    drupal_json(array("error" => TRUE, ));
    return;
  }
  
  _uwpeople_ajax_set_progress(NULL, NULL);
  variable_del('uwpeople_ajax_token');
  drupal_json(array("error" => FALSE, ));
}

/**
 * Callback for AJAX handler.
 */
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
