<?php

/***************************************************************
 * Directory Profile Module                                    *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Defines a "Recognition" CCK field, formatter, & widget.
 */


/***************************************************************
 *                          Field                              *
 ***************************************************************/

function _dirprof_recognition_field_info() {
  return array(
    'dirprof_recognition' => array(
      'label' => t('Recognition'),
      'description' => t('A recognition of an individual - defined by the Directory Profile module.'),
      'content_icon' => 'icon_content_text.png',
    ),
  );
}

function _dirprof_recognition_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
    case 'database columns':
      $columns = array();
      
      $columns['recognition_title'] = array(
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['recognition_year'] = array(
        'type' => 'varchar',
        'length' => 4,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['recognition_details'] = array(
        'type' => 'varchar',
        'length' => 1024,
        'not null' => TRUE,
        'sortable' => FALSE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['recognition_link'] = array(
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

function _dirprof_recognition_field($op, &$node, $field, &$items, $teaser, $page) {
  switch ($op) {
    case 'validate':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          $error_element = '';
          if (is_array($item) && isset($item['_error_element'])) {
            $error_element = $item['_error_element'];
            unset($item['_error_element']);
          }
          
          if (_dirprof_recognition_content_is_empty($item, $field))
            continue;
          
          if (!empty($item['recognition_details']) && strlen($item['recognition_details']) > 1024) {
            form_set_error($error_element, t('%name: Maximum length for recognition details is 1024 characters.', array(
              '%name' => $field['widget']['label'],
            )));
          }
          else if (!empty($item['recognition_year'])) {
            $year = $item['recognition_year'];
            
            if (!is_numeric($year)) {
              form_set_error($error_element, t('%name: Year must be numeric.', array(
                '%name' => $field['widget']['label'],
              )));
            }
            else {
              $year = intval($year);
              
              if ($year < 1900 || $year > intval(date('Y'))) {
                form_set_error($error_element, t('%name: Year must be between 1900 and the current year.', array(
                  '%name' => $field['widget']['label'],
                )));
              }
            }
          }
          else if (empty($item['recognition_title'])) {
            form_set_error($error_element, t('%name: Recognition Title field is required.', array(
              '%name' => $field['widget']['label'],
            )));
          }
        }
      }
      break;
    case 'sanitize':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          if (!empty($items[$delta]['recognition_title']))
            $items[$delta]['recognition_title_safe'] = check_plain($items[$delta]['recognition_title']);
          
          if (!empty($items[$delta]['recognition_year']))
            $items[$delta]['recognition_year_safe'] = check_plain($items[$delta]['recognition_year']);
          
          if (!empty($items[$delta]['recognition_details']))
            $items[$delta]['recognition_details_safe'] = check_plain($items[$delta]['recognition_details']);
          
          if (!empty($items[$delta]['recognition_link']))
            $items[$delta]['recognition_link_safe'] = check_url($items[$delta]['recognition_link']);
        }
      }
  }
}

function _dirprof_recognition_content_is_empty($item, $field) {
  return empty($item['recognition_title']) && empty($item['recognition_year']) && empty($item['recognition_details']) && empty($item['recognition_link']);
}

function _dirprof_recognition_content_generate($node, $field) {
  $node_field = array();
  $node_field['recognition_title'] = 'Recognition ' . rand(0, 99999);
  $node_field['recognition_year'] = rand(1964, 2010);
  $node_field['recognition_details'] = 'All about the recognition.';
  $node_field['recognition_link'] = 'http://www.google.com';
  
  return $node_field;
}


/***************************************************************
 *                         Formatter                           *
 ***************************************************************/

function _dirprof_recognition_theme() {
  return array(
    'dirprof_formatter_recognition_link' => array(
      'arguments' => array('element' => NULL, ),
    ),
    'dirprof_formatter_recognition_plain' => array(
      'arguments' => array('element' => NULL, ),
    ),
  );
}

function _dirprof_recognition_field_formatter_info() {
  return array(
    'recognition_link' => array(
      'label' => t('Link'),
      'field types' => array('dirprof_recognition'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
    'recognition_plain' => array(
      'label' => t('Plain text'),
      'field types' => array('dirprof_recognition'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'recognition link' field formatter.
 */
function theme_dirprof_formatter_recognition_link($element) {
  $ret = '<div class="recognition_entry">';
  
  $ret .= '<div class="header">';
  
  if (empty($element['#item']['recognition_link_safe'])) {
    $ret .= '<span class="title">' . $element['#item']['recognition_title_safe'] . '</span>';
    
    if (!empty($element['#item']['recognition_year_safe']))
      $ret .= ' <span class="year">(' . $element['#item']['recognition_year_safe'] . ')</span>';
  }
  else {
    $ret .= l($element['#item']['recognition_title'], $element['#item']['recognition_link']);
    
    if (!empty($element['#item']['recognition_year_safe']))
      $ret .= ' <span class="year">' . $element['#item']['recognition_year_safe'] . '</span>';
  }
  
  $ret .= '</div>';
  
  if (!empty($element['#item']['recognition_details'])) {
    $ret .= '<p class="details">';
    $ret .= $element['#item']['recognition_details'];
    $ret .= '</p>';
  }
  
  $ret .= '<div class="recognition_clear"></div></div>';
  return $ret;
}

/**
 * Theme function for 'recognition plain' field formatter.
 */
function theme_dirprof_formatter_recognition_plain($element) {
  $ret = '<div class="recognition_entry_plain">';
  $ret .= '<div class="header">';
  
  $ret .= '<span class="title">' . $element['#item']['recognition_title_safe'] . '</span>';
  if (!empty($element['#item']['recognition_year_safe']))
    $ret .= ' <span class="year">(' . $element['#item']['recognition_year_safe'] . ')</span>';
  if (!empty($element['#item']['recognition_link_safe']))
    $ret .= ' <span class="link">' . $element['#item']['recognition_link_safe'] . '</span>';
  
  $ret .= '</div>';
  
  if (!empty($element['#item']['recognition_details'])) {
    $ret .= '<p class="details">';
    $ret .= $element['#item']['recognition_details'];
    $ret .= '</p>';
  }
  
  $ret .= '<div class="recognition_clear"></div></div>';
  return $ret;
}

function _dirprof_recognition_can_theme($element) {
  return array_key_exists('recognition_title', $element['#item']);
}

function _dirprof_recognition_default_theme($element) {
  return theme_dirprof_formatter_recognition_link($element);
}


/***************************************************************
 *                           Widget                            *
 ***************************************************************/

function _dirprof_recognition_widget_info() {
  return array(
    'dirprof_recognition_widget' => array(
      'label' => t('Recognition Widget'),
      'field types' => array('dirprof_recognition'),
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
function dirprof_recognition_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
  }
}

function _dirprof_recognition_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  $element = array();
  
  $element['recognition_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => isset($items[$delta]['collaborator_first']) ? $items[$delta]['collaborator_first'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'collaborator_first'),
    '#maxlength' => 128,
  );
  
  $element['recognition_year'] = array(
    '#type' => 'textfield',
    '#title' => t('Year'),
    '#default_value' => isset($items[$delta]['collaborator_last']) ? $items[$delta]['collaborator_last'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 4,
    '#attributes' => array('class' => 'collaborator_last'),
    '#maxlength' => 4,
  );
  
  $element['recognition_details'] = array(
    '#type' => 'textarea',
    '#title' => t('Details'),
    '#default_value' => isset($items[$delta]['collaborator_link']) ? $items[$delta]['collaborator_link'] : NULL,
    '#attributes' => array('class' => 'collaborator_link'),
  );
  
  $element['recognition_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Link'),
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
