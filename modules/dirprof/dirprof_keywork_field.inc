<?php

/***************************************************************
 * Directory Profile Module                                    *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Defines a "Key Work" CCK field, formatter, & widget.
 */


/***************************************************************
 *                          Field                              *
 ***************************************************************/

function _dirprof_keywork_field_info() {
  return array(
    'dirprof_keywork' => array(
      'label' => t('Key Work'),
      'description' => t('A key work for an individual - defined by the Directory Profile module.'),
      'content_icon' => 'icon_content_text.png',
    ),
  );
}

function _dirprof_keywork_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
    case 'database columns':
      $columns = array();
      
      $columns['keywork_citation'] = array(
        'type' => 'varchar',
        'length' => 1024,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['keywork_filepath'] = array(
        'type' => 'varchar',
        'length' => 1024,
        'not null' => TRUE,
        'sortable' => FALSE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['keywork_url'] = array(
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

function _dirprof_keywork_field($op, &$node, $field, &$items, $teaser, $page) {
  switch ($op) {
    case 'validate':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          $error_element = '';
          if (is_array($item) && isset($item['_error_element'])) {
            $error_element = $item['_error_element'];
            unset($item['_error_element']);
          }
          
          if (_dirprof_keywork_content_is_empty($item, $field))
            continue;
          
          if (!empty($item['keywork_filepath']) && !file_exists($item['keywork_filepath'])) {
            form_set_error($error_element, t('%name: The specified file must exist.', array(
              '%name' => $field['widget']['label'],
            )));
          }
          else if (!empty($item['keywork_citation']) && strlen($item['keywork_citation']) > 1024) {
            form_set_error($error_element, t('%name: Maximum length for citation is 1024 characters.', array(
              '%name' => $field['widget']['label'],
            )));
          }
          else if (empty($item['keywork_citation'])) {
            form_set_error($error_element, t('%name: Citation field is required.', array(
              '%name' => $field['widget']['label'],
            )));
          }
        }
      }
      break;
    case 'sanitize':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          if (!empty($items[$delta]['keywork_citation']))
            $items[$delta]['keywork_citation_safe']= check_plain($items[$delta]['keywork_citation']);
          
          if (!empty($items[$delta]['keywork_filepath']))
            $items[$delta]['keywork_filepath_safe']= check_url($items[$delta]['keywork_filepath']);
          
          if (!empty($items[$delta]['keywork_url']))
            $items[$delta]['keywork_url_safe']= check_url($items[$delta]['keywork_url']);
        }
      }
  }
}

function _dirprof_keywork_content_is_empty($item, $field) {
  return empty($item['keywork_citation']) && empty($item['keywork_filepath']) && empty($item['keywork_url']);
}

function _dirprof_keywork_content_generate($node, $field) {
  $node_field = array();
  $node_field['keywork_citation'] = 'Citation ' . rand(0, 99999);
  $node_field['keywork_filepath'] = '';
  $node_field['keywork_url'] = 'http://www.google.com';
  
  return $node_field;
}


/***************************************************************
 *                         Formatter                           *
 ***************************************************************/

function _dirprof_keywork_theme() {
  return array(
    'dirprof_formatter_keywork_link' => array(
      'arguments' => array('element' => NULL, ),
    ),
    'dirprof_formatter_keywork_plain' => array(
      'arguments' => array('element' => NULL, ),
    ),
  );
}

function _dirprof_keywork_field_formatter_info() {
  return array(
    'keywork_link' => array(
      'label' => t('Link'),
      'field types' => array('dirprof_keywork'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
    'keywork_plain' => array(
      'label' => t('Plain text'),
      'field types' => array('dirprof_keywork'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'keywork link' field formatter.
 */
function theme_dirprof_formatter_keywork_link($element) {
  $citation = $element['#item']['keywork_citation_safe'];
  $ret = '<div class="keywork_entry">';
  
  if (!empty($element['#item']['keywork_filepath']))
    $ret .= l($citation, $element['#item']['keywork_filepath']);
  else if (!empty($element['#item']['keywork_url']))
    $ret .= l($citation, $element['#item']['keywork_url']);
  else
    $ret .= $citation;
  
  $ret .= '</div>';
  
  return $ret;
}

/**
 * Theme function for 'keywork plain' field formatter.
 */
function theme_dirprof_formatter_keywork_plain($element) {
  $citation = $element['#item']['keywork_citation_safe'];
  $ret = '<div class="keywork_entry_plain">';
  
  $ret .= $citation;
  
  if (!empty($element['#item']['keywork_filepath_safe']))
    $ret .= '[FILE] ' . $element['#item']['keywork_filepath_safe'];
  else if (!empty($element['#item']['keywork_url_safe']))
    $ret .= '[URL] ' . $element['#item']['keywork_url_safe'];
  
  $ret .= '</div>';
  
  return $ret;
}

function _dirprof_keywork_can_theme($element) {
  return array_key_exists('keywork_citation', $element['#item']);
}

function _dirprof_keywork_default_theme($element) {
  return theme_dirprof_formatter_keywork_link($element);
}


/***************************************************************
 *                           Widget                            *
 ***************************************************************/

function _dirprof_keywork_widget_info() {
  return array(
    'dirprof_keywork_widget' => array(
      'label' => t('Key Work Widget'),
      'field types' => array('dirprof_keywork'),
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
function dirprof_keywork_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
  }
}

function _dirprof_keywork_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  $element = array();
  
  $element['keywork_citation'] = array(
    '#type' => 'textarea',
    '#title' => t('Citation'),
    '#default_value' => isset($items[$delta]['keywork_citation']) ? $items[$delta]['keywork_citation'] : NULL,
    '#attributes' => array('class' => 'keywork_citation'),
  );
  
  $element['keywork_filepath'] = array(
    '#type' => 'textfield',
    '#title' => t('Document File Path'),
    '#default_value' => isset($items[$delta]['keywork_filepath']) ? $items[$delta]['keywork_filepath'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'keywork_filepath'),
    '#maxlength' => 1024,
  );
  
  $element['keywork_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Document URL'),
    '#default_value' => isset($items[$delta]['keywork_url']) ? $items[$delta]['keywork_url'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'keywork_url'),
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
