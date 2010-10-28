<?php

/***************************************************************
 * iPeople Module                                              *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Provides the iPeople module administration interface.
 */

/**
 * Gets the configuration form.
 */
function _ipeople_admin_form($form_state) {
  $form = array();
  
  $form['grouping'] = array(
    '#type' => 'fieldset',
    '#title' => t('iPeople Module Settings'),
    '#collapsible' => FALSE,
    '#tree' => TRUE,
  );
  
  $form['grouping']['reminder'] = array(
    '#value' => '<p>' . t('NOTE: LTS caches data. If new data is not pulled try clearing the LTS cache.  Also note that members are fetched using LTS not Drupal roles.') . '</p>',
  );
  
  $form['grouping']['istudent'] = array(
    '#type' => 'fieldset',
    '#title' => t('iStudent Settings'),
    '#collapsible' => FALSE,
    '#tree' => TRUE,
  );
  
  $form['grouping']['istudent']['insc'] = array(
    '#type' => 'textfield',
    '#title' => t('INSC Roles'),
    '#default_value' => _ipeople_var(IPEOPLE_STUDENT_INSC_GROUPS),
    '#description' => t('Comma separated list of all the roles that INSC students belong to.'),
    '#size' => 55,
    '#maxlength' => 350,
  );
  
  $form['grouping']['istudent']['msim'] = array(
    '#type' => 'textfield',
    '#title' => t('MSIM Roles'),
    '#default_value' => _ipeople_var(IPEOPLE_STUDENT_MSIM_GROUPS),
    '#description' => t('Comma separated list of all the roles that MSIM students belong to.'),
    '#size' => 55,
    '#maxlength' => 350,
  );
  
  $form['grouping']['istudent']['mlis'] = array(
    '#type' => 'textfield',
    '#title' => t('MLIS Roles'),
    '#default_value' => _ipeople_var(IPEOPLE_STUDENT_MLIS_GROUPS),
    '#description' => t('Comma separated list of all the roles that MLIS students belong to.'),
    '#size' => 55,
    '#maxlength' => 350,
  );
  
  $form['grouping']['istudent']['info'] = array(
    '#type' => 'textfield',
    '#title' => t('Informatics Roles'),
    '#default_value' => _ipeople_var(IPEOPLE_STUDENT_INFO_GROUPS),
    '#description' => t('Comma separated list of all the roles that Informatics students belong to.'),
    '#size' => 55,
    '#maxlength' => 350,
  );
  
  $form['grouping']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );
  
  return $form;
}

/**
 * Handles the submission of the form described in '_ipeople_admin_form'.
 */
function _ipeople_admin_form_submit($form, &$form_state) {
  if (!user_access(IPEOPLE_PERM)) {
    drupal_set_message(t('You are unauthorized to make changes.'), 'error');
    _dirprof_error('Unauthorized user attempted to make changes.');
    return;
  }
  
  $op = $form_state['values']['op'];
  
  if ($op == t('Save Settings')) {
    $istudent = $form_state['values']['grouping']['istudent'];
    _ipeople_var(IPEOPLE_STUDENT_INSC_GROUPS, $istudent['insc']);
    _ipeople_var(IPEOPLE_STUDENT_MSIM_GROUPS, $istudent['msim']);
    _ipeople_var(IPEOPLE_STUDENT_MLIS_GROUPS, $istudent['mlis']);
    _ipeople_var(IPEOPLE_STUDENT_INFO_GROUPS, $istudent['info']);
    
    drupal_set_message(t('Settings have been saved.'));
  }
}

/**
 * Page callback for the iPeople administration page.
 */
function _ipeople_admin_page() {
  return drupal_get_form('_ipeople_admin_form');
}
