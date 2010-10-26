<?php

/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Defines a "Collaborator" CCK field, formatter, & widget.
 */


/***************************************************************
 *                          Field                              *
 ***************************************************************/

function _uwpeople_collaborator_field_info() {
  return array(
    'uwpeople_collaborator' => array(
      'label' => t('Collaborator'),
      'description' => t('A collaborator for an individual - defined by the UW People module.'),
      'content_icon' => 'icon_content_text.png',
    ),
  );
}

function _uwpeople_collaborator_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
    case 'database columns':
      $columns = array();
      
      $columns['collaborator_first'] = array(
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['collaborator_last'] = array(
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['collaborator_link'] = array(
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

function _uwpeople_collaborator_field($op, &$node, $field, &$items, $teaser, $page) {
  switch ($op) {
    case 'validate':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          $error_element = '';
          if (is_array($item) && isset($item['_error_element'])) {
            $error_element = $item['_error_element'];
            unset($item['_error_element']);
          }
          
          if (_uwpeople_collaborator_content_is_empty($item, $field))
            continue;
          
          if (empty($item['collaborator_first']) ||
              empty($item['collaborator_last'])) {
            form_set_error($error_element, t('%name: First and Last Name fields are required.', array(
              '%name' => $field['widget']['label'],
            )));
          }
        }
      }
      break;
    case 'sanitize':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          if (!empty($items[$delta]['collaborator_first']))
            $items[$delta]['collaborator_first_safe']= check_plain($items[$delta]['collaborator_first']);
          
          if (!empty($items[$delta]['collaborator_last']))
            $items[$delta]['collaborator_last_safe']= check_plain($items[$delta]['collaborator_last']);
          
          if (!empty($items[$delta]['collaborator_link']))
            $items[$delta]['collaborator_link_safe'] = check_url($items[$delta]['collaborator_link']);
        }
      }
  }
}

function _uwpeople_collaborator_content_is_empty($item, $field) {
  return empty($item['collaborator_first']) && empty($item['collaborator_last']) && empty($item['collaborator_link']);
}

function _uwpeople_collaborator_content_generate($node, $field) {
  $node_field = array();
  $node_field['collaborator_first'] = 'First ' . rand(0, 99999);
  $node_field['collaborator_last'] = 'Last ' . rand(0, 99999);
  $node_field['collaborator_link'] = 'http://www.google.com';
  
  return $node_field;
}


/***************************************************************
 *                         Formatter                           *
 ***************************************************************/

function _uwpeople_collaborator_theme() {
  return array(
    'uwpeople_formatter_collaborator_link' => array(
      'arguments' => array('element' => NULL, ),
    ),
    'uwpeople_formatter_collaborator_plain' => array(
      'arguments' => array('element' => NULL, ),
    ),
  );
}

function _uwpeople_collaborator_field_formatter_info() {
  return array(
    'collaborator_link' => array(
      'label' => t('Link'),
      'field types' => array('uwpeople_collaborator'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
    'collaborator_plain' => array(
      'label' => t('Plain text'),
      'field types' => array('uwpeople_collaborator'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'collaborator link' field formatter.
 */
function theme_uwpeople_formatter_collaborator_link($element) {
  if (!empty($element['#item']['collaborator_link']))
    return '<div class="collaborator_entry">' . l($element['#item']['collaborator_first'] . ' ' . $element['#item']['collaborator_last'], $element['#item']['collaborator_link']) . '</div>';
  return '<div class="collaborator_entry">' . ($element['#item']['collaborator_first_safe'] . ' ' . $element['#item']['collaborator_last_safe']) . '</div>';
}

/**
 * Theme function for 'collaborator plain' field formatter.
 */
function theme_uwpeople_formatter_collaborator_plain($element) {
  return '<div class="collaborator_entry_plain">' . $element['#item']['collaborator_first_safe'] . ' ' . $element['#item']['collaborator_last_safe'] . ', ' . $element['#item']['collaborator_link_safe'] . '</div>';
}

function _uwpeople_collaborator_can_theme($element) {
  return array_key_exists('collaborator_first', $element['#item']);
}

function _uwpeople_collaborator_default_theme($element) {
  return theme_uwpeople_formatter_collaborator_link($element);
}


/***************************************************************
 *                           Widget                            *
 ***************************************************************/

function _uwpeople_collaborator_widget_info() {
  return array(
    'uwpeople_collaborator_widget' => array(
      'label' => t('Collaborator Widget'),
      'field types' => array('uwpeople_collaborator'),
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
function uwpeople_collaborator_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
  }
}

function _uwpeople_collaborator_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  $element = array();
  
  $element['collaborator_first'] = array(
    '#type' => 'textfield',
    '#title' => t('First Name'),
    '#default_value' => isset($items[$delta]['collaborator_first']) ? $items[$delta]['collaborator_first'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 50,
    '#attributes' => array('class' => 'collaborator_first'),
    '#maxlength' => 128,
  );
  
  $element['collaborator_last'] = array(
    '#type' => 'textfield',
    '#title' => t('Last Name'),
    '#default_value' => isset($items[$delta]['collaborator_last']) ? $items[$delta]['collaborator_last'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 50,
    '#attributes' => array('class' => 'collaborator_last'),
    '#maxlength' => 128,
  );
  
  $element['collaborator_link'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#default_value' => isset($items[$delta]['collaborator_link']) ? $items[$delta]['collaborator_link'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'collaborator_link'),
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
