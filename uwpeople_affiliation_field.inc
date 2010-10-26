<?php

/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Defines an "Affiliation" CCK field, formatter, & widget.
 */


/***************************************************************
 *                          Field                              *
 ***************************************************************/

function _uwpeople_affiliation_field_info() {
  return array(
    'uwpeople_affiliation' => array(
      'label' => t('Affiliation'),
      'description' => t('An affiliation for an individual - defined by the UW People module.'),
      'content_icon' => 'icon_content_text.png',
    ),
  );
}

function _uwpeople_affiliation_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
    case 'database columns':
      $columns = array();
      
      $columns['affiliation'] = array(
        'type' => 'varchar',
        'length' => 256,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['affiliation_link'] = array(
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

function _uwpeople_affiliation_field($op, &$node, $field, &$items, $teaser, $page) {
  switch ($op) {
    case 'validate':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          $error_element = '';
          if (is_array($item) && isset($item['_error_element'])) {
            $error_element = $item['_error_element'];
            unset($item['_error_element']);
          }
          
          if (_uwpeople_affiliation_content_is_empty($item, $field))
            continue;
          
          if (empty($item['affiliation'])) {
            form_set_error($error_element, t('%name: Affiliation field is required.', array(
              '%name' => $field['widget']['label'],
            )));
          }
        }
      }
      break;
    case 'sanitize':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          if (!empty($items[$delta]['affiliation']))
            $items[$delta]['affiliation_safe'] = check_plain($items[$delta]['affiliation']);
          
          if (!empty($items[$delta]['affiliation_link']))
            $items[$delta]['affiliation_link_safe'] = check_url($items[$delta]['affiliation_link']);
        }
      }
  }
}

function _uwpeople_affiliation_content_is_empty($item, $field) {
  return empty($item['affiliation']) && empty($item['affiliation_link']);
}

function _uwpeople_affiliation_content_generate($node, $field) {
  $node_field = array();
  $node_field['affiliation'] = 'Affiliation ' . rand(0, 99999);
  $node_field['affiliation_link'] = 'http://www.google.com';
  
  return $node_field;
}


/***************************************************************
 *                         Formatter                           *
 ***************************************************************/

function _uwpeople_affiliation_theme() {
  return array(
    'uwpeople_formatter_affiliate_link' => array(
      'arguments' => array('element' => NULL, ),
    ),
    'uwpeople_formatter_affiliate_plain' => array(
      'arguments' => array('element' => NULL, ),
    ),
  );
}

function _uwpeople_affiliation_field_formatter_info() {
  return array(
    'affiliate_link' => array(
      'label' => t('Link'),
      'field types' => array('uwpeople_affiliation'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
    'affiliate_plain' => array(
      'label' => t('Plain text'),
      'field types' => array('uwpeople_affiliation'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'affiliate link' field formatter.
 */
function theme_uwpeople_formatter_affiliate_link($element) {
  if (!empty($element['#item']['affiliation_link']))
    return '<div class="affliation_entry">' . l($element['#item']['affiliation'], $element['#item']['affiliation_link']) . '</div>';
  return '<div class="affliation_entry">' . $element['#item']['affiliation_safe'] . '</div>';
}

/**
 * Theme function for 'affiliate plain' field formatter.
 */
function theme_uwpeople_formatter_affiliate_plain($element) {
  return '<div class="affiliation_entry_plain">' . $element['#item']['affiliation_safe'] . ', ' . $element['#item']['affiliation_link_safe'] . '</div>';
}

function _uwpeople_affiliation_can_theme($element) {
  return array_key_exists('affiliation', $element['#item']);
}

function _uwpeople_affiliation_default_theme($element) {
  return theme_uwpeople_formatter_affiliate_link($element);
}


/***************************************************************
 *                           Widget                            *
 ***************************************************************/

function _uwpeople_affiliation_widget_info() {
  return array(
    'uwpeople_affiliation_widget' => array(
      'label' => t('Affiliation Widget'),
      'field types' => array('uwpeople_affiliation'),
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
function uwpeople_affiliation_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
  }
}

function _uwpeople_affiliation_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  $element = array();
  
  $element['affiliation'] = array(
    '#type' => 'textfield',
    '#title' => t('Affiliation'),
    '#default_value' => isset($items[$delta]['affiliation']) ? $items[$delta]['affiliation'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'affiliation'),
    '#maxlength' => 256,
  );
  
  $element['affiliation_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Affiliation Link'),
    '#default_value' => isset($items[$delta]['affiliation_link']) ? $items[$delta]['affiliation_link'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'affiliation_link'),
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
