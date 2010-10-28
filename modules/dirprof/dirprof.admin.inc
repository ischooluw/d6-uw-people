<?php

/***************************************************************
 * Directory Profile Module                                    *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Provides the Directory Profile module administration interface.
 */

/**
 * Gets the configuration form.
 */
function _dirprof_admin_form($form_state) {
  $form = array();
  
  $form['grouping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Directory Profile Module Settings'),
    '#collapsible' => FALSE,
    '#tree' => TRUE,
  );
  
  $form['grouping']['reminder'] = array(
    '#value' => '<p>' . t('NOTE: LTS caches data. If new data is not pulled try clearing the LTS cache.') . '</p>',
  );
  
  $form['grouping']['simple'] = array(
    '#type' => 'checkboxes', 
    '#title' => t('Directories that get simple profiles'), 
    '#default_value' => _dirprof_var(DIRPROF_SIMPLEPROF),
    '#options' => array(
      'student' => t('Student'), 
      'phd' => t('Ph.D.'), 
      'researcher' => t('Researcher'),
      'faculty' => t('Faculty'),
      'staff' => t('Staff'),
    ),
    '#description' => t('Users in the selected directories will have a "Simplified ' .
                        'Profile" node automatically created and associated ' .
                        'with their "Person" node.'),
  );
  
  $form['grouping']['extended'] = array(
    '#type' => 'checkboxes', 
    '#title' => t('Directories that get extended profiles'), 
    '#default_value' => _dirprof_var(DIRPROF_EXTENDEDPROF),
    '#options' => array(
      'student' => t('Student'), 
      'phd' => t('Ph.D.'), 
      'researcher' => t('Researcher'),
      'faculty' => t('Faculty'),
      'staff' => t('Staff'),
    ),
    '#description' => t('Users in the selected directories will have a "Extended ' .
                        'Profile" node automatically created and associated ' .
                        'with their "Person" node.'),
  );
  
  $form['grouping']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );
  
  return $form;
}

/**
 * Handles the submission of the form described in '_dirprof_admin_form'.
 */
function _dirprof_admin_form_submit($form, &$form_state) {
  if (!user_access(DIRPROF_PERM)) {
    drupal_set_message(t('You are unauthorized to make changes.'), 'error');
    _dirprof_error('Unauthorized user attempted to make changes.');
    return;
  }
  
  $op = $form_state['values']['op'];
  
  if ($op == t('Save Settings')) {
    _dirprof_var(DIRPROF_SIMPLEPROF, $form_state['values']['grouping']['simple']);
    _dirprof_var(DIRPROF_EXTENDEDPROF, $form_state['values']['grouping']['extended']);
    
    drupal_set_message(t('Settings have been saved.'));
  }
}

/**
 * Page callback for the Directory Profile administration page.
 */
function _dirprof_admin_page() {
  return drupal_get_form('_dirprof_admin_form');
}
