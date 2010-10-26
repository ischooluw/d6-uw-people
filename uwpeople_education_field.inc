<?php

/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Defines an "Education" CCK field, formatter, & widget.
 */


/***************************************************************
 *                          Field                              *
 ***************************************************************/

function _uwpeople_education_field_info() {
  return array(
    'uwpeople_education' => array(
      'label' => t('Education'),
      'description' => t('An education entry for an individual - defined by the UW People module.'),
      'content_icon' => 'icon_content_text.png',
    ),
  );
}

function _uwpeople_education_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
    case 'database columns':
      $columns = array();
      
      $columns['education_degree'] = array(
        'type' => 'varchar',
        'length' => 12,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['education_year'] = array(
        'type' => 'varchar',
        'length' => 4,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['education_institution'] = array(
        'type' => 'varchar',
        'length' => 256,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['education_concentration'] = array(
        'type' => 'varchar',
        'length' => 256,
        'not null' => TRUE,
        'sortable' => TRUE,
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

function _uwpeople_education_field($op, &$node, $field, &$items, $teaser, $page) {
  switch ($op) {
    case 'validate':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          $error_element = '';
          if (is_array($item) && isset($item['_error_element'])) {
            $error_element = $item['_error_element'];
            unset($item['_error_element']);
          }
          
          if (_uwpeople_education_content_is_empty($item, $field))
            continue;
          
          if (empty($item['education_degree']) ||
              empty($item['education_year']) ||
              empty($item['education_institution']) ||
              empty($item['education_concentration'])) {
              form_set_error($error_element, t('%name: All fields are required.', array(
                '%name' => $field['widget']['label'],
              )));
          }
        }
      }
      break;
    case 'sanitize':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          if (!empty($items[$delta]['education_degree']))
            $items[$delta]['education_degree_safe'] = check_plain($items[$delta]['education_degree']);
          
          if (!empty($items[$delta]['education_year']))
            $items[$delta]['education_year_safe'] = check_plain($items[$delta]['education_year']);
          
          if (!empty($items[$delta]['education_institution']))
            $items[$delta]['education_institution_safe'] = check_plain($items[$delta]['education_institution']);
          
          if (!empty($items[$delta]['education_concentration']))
            $items[$delta]['education_concentration_safe'] = check_plain($items[$delta]['education_concentration']);
        }
      }
  }
}

function _uwpeople_education_content_is_empty($item, $field) {
  return empty($item['education_degree']) && empty($item['education_year']) && empty($item['education_institution']) && empty($item['education_concentration']);
}

function _uwpeople_education_content_generate($node, $field) {
  $node_field = array();
  $node_field['education_degree'] = rand(0, 1) == 0 ? 'Ph.D.' : 'M.S.';
  $node_field['education_year'] = '' . rand(1964, 2009);
  $node_field['education_institution'] = 'Institution ' . rand(0, 99999);
  $node_field['education_concentration'] = 'Concentration ' . rand(0, 99999);
  
  return $node_field;
}


/***************************************************************
 *                         Formatter                           *
 ***************************************************************/

function _uwpeople_education_theme() {
  return array(
    'uwpeople_formatter_education_plain' => array(
      'arguments' => array('element' => NULL, ),
    ),
  );
}

function _uwpeople_education_field_formatter_info() {
  return array(
    'education_plain' => array(
      'label' => t('Plain text'),
      'field types' => array('uwpeople_education'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'education plain' field formatter.
 */
function theme_uwpeople_formatter_education_plain($element) {
  $ret = '<div class="education_entry">';
  
  $ret .= $element['#item']['education_degree_safe'];
  $ret .= ', ';
  $ret .= $element['#item']['education_concentration_safe'];
  $ret .= ', ';
  $ret .= $element['#item']['education_institution_safe'];
  $ret .= ', ';
  $ret .= $element['#item']['education_year_safe'];
  
  $ret .= '</div>';
  return $ret;
}

function _uwpeople_education_can_theme($element) {
  return array_key_exists('education_degree', $element['#item']);
}

function _uwpeople_education_default_theme($element) {
  return theme_uwpeople_formatter_education_plain($element);
}


/***************************************************************
 *                           Widget                            *
 ***************************************************************/

function _uwpeople_education_widget_info() {
  return array(
    'uwpeople_education_widget' => array(
      'label' => t('Education Widget'),
      'field types' => array('uwpeople_education'),
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
function uwpeople_education_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
  }
}

function _uwpeople_education_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  $element = array();
  
  $element['education_degree'] = array(
    '#type' => 'textfield',
    '#title' => t('Degree'),
    '#default_value' => isset($items[$delta]['education_degree']) ? $items[$delta]['education_degree'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 12,
    '#attributes' => array('class' => 'education_degree'),
    '#maxlength' => 12,
  );
  
  $element['education_year'] = array(
    '#type' => 'textfield',
    '#title' => t('Year'),
    '#default_value' => isset($items[$delta]['education_year']) ? $items[$delta]['education_year'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 4,
    '#attributes' => array('class' => 'education_year'),
    '#maxlength' => 4,
  );
  
  $element['education_institution'] = array(
    '#type' => 'textfield',
    '#title' => t('College/University'),
    '#default_value' => isset($items[$delta]['education_institution']) ? $items[$delta]['education_institution'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'education_institution'),
    '#maxlength' => 256,
  );
  
  $element['education_concentration'] = array(
    '#type' => 'textfield',
    '#title' => t('Program/Concentration'),
    '#default_value' => isset($items[$delta]['education_concentration']) ? $items[$delta]['education_concentration'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'education_concentration'),
    '#maxlength' => 256,
  );
  
  if (empty($form['#parents']))
    $form['#parents'] = array();
  
  $element['_error_element'] = array(
    '#type' => 'value',
    '#value' => implode('][', array_merge($form['#parents'], array('value'))),
  );
  
  return $element;
}
