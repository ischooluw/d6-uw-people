<?php

/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Defines a "Membership" CCK field, formatter, & widget.
 */


/***************************************************************
 *                          Field                              *
 ***************************************************************/

function _uwpeople_membership_field_info() {
  return array(
    'uwpeople_membership' => array(
      'label' => t('Membership'),
      'description' => t('A membership for an individual - defined by the UW People module.'),
      'content_icon' => 'icon_content_text.png',
    ),
  );
}

function _uwpeople_membership_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
    case 'database columns':
      $columns = array();
      
      $columns['membership_organization'] = array(
        'type' => 'varchar',
        'length' => 256,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['membership_link'] = array(
        'type' => 'varchar',
        'length' => 1024,
        'not null' => TRUE,
        'sortable' => FALSE,
        'views' => TRUE,
        'default' => '',
      );
      
      return $columns;
    case 'views data':
      $data = content_views_field_views_data($field);
      $db_info = content_database_info($field);
      $table_alias = content_views_tablename($field);
      $field_data = $data[$table_alias][$field['field_name'] . '_value'];
      
      return $data;
  }
}

function _uwpeople_membership_field($op, &$node, $field, &$items, $teaser, $page) {
  switch ($op) {
    case 'validate':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          $error_element = '';
          if (is_array($item) && isset($item['_error_element'])) {
            $error_element = $item['_error_element'];
            unset($item['_error_element']);
          }
          
          if (_uwpeople_membership_content_is_empty($item, $field))
            continue;
          
          if (empty($item['membership_organization'])) {
            form_set_error($error_element, t('%name: Membership Organization field is required.', array(
              '%name' => $field['widget']['label'],
            )));
          }
        }
      }
      break;
    case 'sanitize':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          if (!empty($items[$delta]['membership_organization']))
            $items[$delta]['membership_organization_safe']= check_plain($items[$delta]['membership_organization']);
          
          if (!empty($items[$delta]['membership_link']))
            $items[$delta]['membership_link_safe'] = check_url($items[$delta]['membership_link']);
        }
      }
  }
}

function _uwpeople_membership_content_is_empty($item, $field) {
  return empty($item['membership_organization']) && empty($item['membership_link']);
}

function _uwpeople_membership_content_generate($node, $field) {
  $node_field = array();
  $node_field['membership_organization'] = 'Organization ' . rand(0, 99999);
  $node_field['membership_link'] = 'http://www.google.com';
  
  return $node_field;
}


/***************************************************************
 *                         Formatter                           *
 ***************************************************************/

function _uwpeople_membership_theme() {
  return array(
    'uwpeople_formatter_membership_link' => array(
      'arguments' => array('element' => NULL, ),
    ),
    'uwpeople_formatter_membership_plain' => array(
      'arguments' => array('element' => NULL, ),
    ),
  );
}

function _uwpeople_membership_field_formatter_info() {
  return array(
    'membership_link' => array(
      'label' => t('Link'),
      'field types' => array('uwpeople_membership'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
    'membership_plain' => array(
      'label' => t('Plain text'),
      'field types' => array('uwpeople_membership'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'membership link' field formatter.
 */
function theme_uwpeople_formatter_membership_link($element) {
  if (!empty($element['#item']['membership_link']))
    return '<div class="membership_entry">' . l($element['#item']['membership_organization'], $element['#item']['membership_link']) . '</div>';
  return '<div class="membership_entry">' . $element['#item']['membership_organization_safe'] . '</div>';
}

/**
 * Theme function for 'membership plain' field formatter.
 */
function theme_uwpeople_formatter_membership_plain($element) {
  return '<div class="membership_entry_plain">' . $element['#item']['membership_organization_safe'] . ', ' . $element['#item']['membership_link_safe'] . '</div>';
}

function _uwpeople_membership_can_theme($element) {
  return array_key_exists('membership_organization', $element['#item']);
}

function _uwpeople_membership_default_theme($element) {
  return theme_uwpeople_formatter_membership_link($element);
}


/***************************************************************
 *                           Widget                            *
 ***************************************************************/

function _uwpeople_membership_widget_info() {
  return array(
    'uwpeople_membership_widget' => array(
      'label' => t('Membership Widget'),
      'field types' => array('uwpeople_membership'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'callbacks' => array(
        'default value' => CONTENT_CALLBACK_DEFAULT,
      ),
    ),
  );
}

/**
 * Implementation of hook_widget_settings().
 */
function uwpeople_membership_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
  }
}

function _uwpeople_membership_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  $element = array();
  
  $element['membership_organization'] = array(
    '#type' => 'textfield',
    '#title' => t('Organization'),
    '#default_value' => isset($items[$delta]['membership_organization']) ? $items[$delta]['membership_organization'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'membership_organization'),
    '#maxlength' => 256,
  );
  
  $element['membership_link'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#default_value' => isset($items[$delta]['membership_link']) ? $items[$delta]['membership_link'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'membership_link'),
    '#maxlength' => 1024,
  );
  
  if (empty($form['#parents']))
    $form['#parents'] = array();
  
  $element['_error_element'] = array(
    '#type' => 'value',
    '#value' => implode('][', array_merge($form['#parents'], array('value'))),
  );
  
  return $element;
}
