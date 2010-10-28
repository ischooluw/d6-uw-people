<?php

/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Provides the UW People module administration interface.
 */

/**
 * Gets the configuration form.
 */
function _uwpeople_admin_form($form_state) {
  $form = array();
  
  $form['grouping'] = array(
    '#type' => 'fieldset',
    '#title' => t('UW People Module Settings'),
    '#collapsible' => FALSE,
    '#tree' => TRUE,
  );
  
  $form['grouping']['reminder'] = array(
    '#value' => '<p>' . t('NOTE: LTS caches data. If new data is not pulled try clearing the LTS cache.  Also note that members are fetched using LTS not Drupal roles.') . '</p>',
  );
  
  $form['grouping']['allpeople'] = array(
    '#type' => 'textfield',
    '#title' => t('People Roles'),
    '#default_value' => _uwpeople_var(UWPEOPLE_ALL_PEOPLE_ROLE),
    '#description' => t('Comma separated list of all the roles that people belong to.'),
    '#size' => 55,
    '#maxlength' => 350,
  );
  
  $form['grouping']['ignore'] = array(
    '#type' => 'textarea',
    '#title' => t('Ignore'),
    '#default_value' => _uwpeople_var(UWPEOPLE_IGNORE),
    '#description' => t('List of users to ignore (one on each line).'),
  );
  
  $form['grouping']['faculty'] = array(
    '#type' => 'textfield',
    '#title' => t('Faculty Roles'),
    '#default_value' => _uwpeople_var(UWPEOPLE_FACULTY_ROLES),
    '#description' => t('Comma separated list of roles that faculty belong to.'),
    '#size' => 55,
    '#maxlength' => 250,
  );
  
  $form['grouping']['staff'] = array(
    '#type' => 'textfield',
    '#title' => t('Staff Roles'),
    '#default_value' => _uwpeople_var(UWPEOPLE_STAFF_ROLES),
    '#description' => t('Comma separated list of roles that staff belong to.'),
    '#size' => 55,
    '#maxlength' => 250,
  );
  
  $form['grouping']['student'] = array(
    '#type' => 'textfield',
    '#title' => t('Student Roles'),
    '#default_value' => _uwpeople_var(UWPEOPLE_STUDENT_ROLES),
    '#description' => t('Comma separated list of roles that students belong to.'),
    '#size' => 55,
    '#maxlength' => 250,
  );
  
  $form['grouping']['researcher'] = array(
    '#type' => 'textfield',
    '#title' => t('Researcher Roles'),
    '#default_value' => _uwpeople_var(UWPEOPLE_RESEARCHER_ROLES),
    '#description' => t('Comma separated list of roles that researchers belong to.'),
    '#size' => 55,
    '#maxlength' => 250,
  );
  
  $form['grouping']['phd'] = array(
    '#type' => 'textfield',
    '#title' => t('Ph.D. Roles'),
    '#default_value' => _uwpeople_var(UWPEOPLE_PHD_ROLES),
    '#description' => t('Comma separated list of roles that Ph.D.s belong to.'),
    '#size' => 55,
    '#maxlength' => 250,
  );
  
  $form['grouping']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );
  
  $form['grouping']['reload'] = array(
    '#type' => 'submit',
    '#value' => t('Reload People'),
  );
  
  return $form;
}

/**
 * Handles the submission of the form described in '_uwpeople_admin_form'.
 */
function _uwpeople_admin_form_submit($form, &$form_state) {
  if (!user_access(UWPEOPLE_PERM)) {
    drupal_set_message(t('You are unauthorized to make UW People changes.'), 'error');
    _uwpeople_error('Unauthorized user attempted to make changes.');
    return;
  }
  
  $op = $form_state['values']['op'];
  
  if ($op == t('Save Settings')) {
    _uwpeople_var(UWPEOPLE_ALL_PEOPLE_ROLE, $form_state['values']['grouping']['allpeople']);
    _uwpeople_var(UWPEOPLE_IGNORE, $form_state['values']['grouping']['ignore']);
    _uwpeople_var(UWPEOPLE_FACULTY_ROLES, $form_state['values']['grouping']['faculty']);
    _uwpeople_var(UWPEOPLE_STAFF_ROLES, $form_state['values']['grouping']['staff']);
    _uwpeople_var(UWPEOPLE_STUDENT_ROLES, $form_state['values']['grouping']['student']);
    _uwpeople_var(UWPEOPLE_RESEARCHER_ROLES, $form_state['values']['grouping']['researcher']);
    _uwpeople_var(UWPEOPLE_PHD_ROLES, $form_state['values']['grouping']['phd']);
    
    drupal_set_message(t('Settings have been saved.'));
  }
  else if ($op == t('Reload People')) {
    // if there is no JS support
    if (_uwpeople_reload_all())
      drupal_set_message(t('People have been reloaded.'));
    else
      drupal_set_message(t('Failed to reload people'), 'warning');
  }
}

/**
 * Page callback for the UW People administration page.
 */
function _uwpeople_admin_page() {
  $settings = array(
    'destination_path' => '/admin/uwpeople/refresh',
  );
  
  drupal_add_js(array('uwpeople' => $settings, ), 'setting');
  drupal_add_js(drupal_get_path('module', 'uwpeople') . '/uwpeople_settings.js', 'module');
  
  return drupal_get_form('_uwpeople_admin_form');
}
