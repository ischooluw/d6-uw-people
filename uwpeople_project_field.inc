<?php

/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Defines a "Project" CCK field, formatter, & widget.
 */


/***************************************************************
 *                          Field                              *
 ***************************************************************/

function _uwpeople_project_field_info() {
  return array(
    'uwpeople_project' => array(
      'label' => t('Project'),
      'description' => t('A project for an individual - defined by the UW People module.'),
      'content_icon' => 'icon_content_text.png',
    ),
  );
}

function _uwpeople_project_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
    case 'database columns':
      $columns = array();
      
      $columns['project_image_filepath'] = array(
        'type' => 'varchar',
        'length' => 1024,
        'not null' => TRUE,
        'sortable' => FALSE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['project_title'] = array(
        'type' => 'varchar',
        'length' => 128,
        'not null' => TRUE,
        'sortable' => TRUE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['project_website'] = array(
        'type' => 'varchar',
        'length' => 1024,
        'not null' => TRUE,
        'sortable' => FALSE,
        'views' => TRUE,
        'default' => '',
      );
      
      $columns['project_summary'] = array(
        'type' => 'varchar',
        'length' => 1024,
        'not null' => TRUE,
        'sortable' => FALSE,
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

function _uwpeople_project_field($op, &$node, $field, &$items, $teaser, $page) {
  switch ($op) {
    case 'validate':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          $error_element = '';
          if (is_array($item) && isset($item['_error_element'])) {
            $error_element = $item['_error_element'];
            unset($item['_error_element']);
          }
          
          if (_uwpeople_project_content_is_empty($item, $field))
            continue;
          
          if (!empty($item['project_image_filepath']) && !file_exists($item['project_image_filepath'])) {
            form_set_error($error_element, t('%name: The specified image file must exist.', array(
              '%name' => $field['widget']['label'],
            )));
          }
          else if (empty($item['project_title'])) {
            form_set_error($error_element, t('%name: Title field is required.', array(
              '%name' => $field['widget']['label'],
            )));
          }
        }
      }
      break;
    case 'sanitize':
      if (is_array($items) && !empty($items)) {
        foreach ($items as $delta => $item) {
          if (!empty($items[$delta]['project_image_filepath']))
            $items[$delta]['project_image_filepath_safe']= check_url($items[$delta]['project_image_filepath']);
          
          if (!empty($items[$delta]['project_title']))
            $items[$delta]['project_title_safe']= check_plain($items[$delta]['project_title']);
          
          if (!empty($items[$delta]['project_website']))
            $items[$delta]['project_website_safe']= check_url($items[$delta]['project_website']);
          
          if (!empty($items[$delta]['project_summary']))
            $items[$delta]['project_summary_safe']= check_plain($items[$delta]['project_summary']);
        }
      }
  }
}

function _uwpeople_project_content_is_empty($item, $field) {
  return empty($item['project_image_filepath']) && empty($item['project_title']) && empty($item['project_website']) && empty($item['project_summary']);
}

function _uwpeople_project_content_generate($node, $field) {
  $node_field = array();
  $node_field['project_image_filepath'] = '';
  $node_field['project_title'] = 'Project ' . rand(0, 99999);
  $node_field['project_website'] = 'http://www.google.com';
  $node_field['project_summary'] = 'This is a project summary.';
  
  return $node_field;
}


/***************************************************************
 *                         Formatter                           *
 ***************************************************************/

function _uwpeople_project_theme() {
  return array(
    'uwpeople_formatter_project_link' => array(
      'arguments' => array('element' => NULL, ),
    ),
    'uwpeople_formatter_project_plain' => array(
      'arguments' => array('element' => NULL, ),
    ),
  );
}

function _uwpeople_project_field_formatter_info() {
  return array(
    'project_link' => array(
      'label' => t('Link'),
      'field types' => array('uwpeople_project'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
    'project_plain' => array(
      'label' => t('Plain text'),
      'field types' => array('uwpeople_project'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'project link' field formatter.
 */
function theme_uwpeople_formatter_project_link($element) {
  $ret = '<div class="project_entry">';
  
  if (!empty($element['#item']['project_image_filepath_safe'])) {
    $url = $element['#item']['project_image_filepath_safe'];
    
    $ret .= '<div class="image">';
    $ret .= l('<img src="' . $url . '" alt="Project image." />', $url, array(
      'html' => TRUE,
    ));
    $ret .= '</div>';
  }
  else {
    $ret .= '<div class="image">';
    $ret .= '<div class="empty_image"></div>';
    $ret .= '</div>';
  }
  
  $ret .= '<div class="header">';
  
  if (empty($element['#item']['project_website']))
    $ret .= '<span class="title">' . $element['#item']['project_title_safe'] . '</span>';
  else
    $ret .= l($element['#item']['project_title'], $element['#item']['project_website']);
  
  $ret .= '</div>';
  
  if (!empty($element['#item']['project_summary_safe'])) {
    $ret .= '<p class="summary">';
    $ret .= $element['#item']['project_summary_safe'];
    $ret .= '</p>';
  }
  
  $ret .= '<div class="project_clear"></div></div>';
  return $ret;
}

/**
 * Theme function for 'project plain' field formatter.
 */
function theme_uwpeople_formatter_project_plain($element) {
  $ret = '<div class="project_entry_plain">';
  
  $ret .= '<div class="header">';
  
  if (empty($element['#item']['project_website_safe'])) {
    $ret .= '<span class="title">' . $element['#item']['project_title_safe'] . '</span>';
  }
  else {
    $ret .= '<span class="title">' . $element['#item']['project_title_safe'] . '</span>';
    $ret .= ' <span class="link">' . $element['#item']['project_website_safe'] . '</span>';
  }
  
  $ret .= '</div>';
  
  if (!empty($element['#item']['project_summary_safe'])) {
    $ret .= '<p class="summary">';
    $ret .= $element['#item']['project_summary_safe'];
    $ret .= '</p>';
  }
  
  $ret .= '<div class="project_clear"></div></div>';
  return $ret;
}

function _uwpeople_project_can_theme($element) {
  return array_key_exists('project_title', $element['#item']);
}

function _uwpeople_project_default_theme($element) {
  return theme_uwpeople_formatter_project_link($element);
}


/***************************************************************
 *                           Widget                            *
 ***************************************************************/

function _uwpeople_project_widget_info() {
  return array(
    'uwpeople_project_widget' => array(
      'label' => t('Project Widget'),
      'field types' => array('uwpeople_project'),
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
function uwpeople_project_widget_settings($op, $widget) {
  switch ($op) {
    case 'form':
      return array();
    case 'save':
      return array();
  }
}

function _uwpeople_project_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  $element = array();
  
  $element['project_image_filepath'] = array(
    '#type' => 'textfield',
    '#title' => t('Project Image'),
    '#default_value' => isset($items[$delta]['project_image_filepath']) ? $items[$delta]['project_image_filepath'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'project_image_filepath'),
    '#maxlength' => 1024,
  );
  
  $element['project_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => isset($items[$delta]['project_title']) ? $items[$delta]['project_title'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'project_title'),
    '#maxlength' => 128,
  );
  
  $element['project_website'] = array(
    '#type' => 'textfield',
    '#title' => t('Website'),
    '#default_value' => isset($items[$delta]['project_website']) ? $items[$delta]['project_website'] : NULL,
    '#autocomplete_path' => NULL,
    '#size' => 100,
    '#attributes' => array('class' => 'project_website'),
    '#maxlength' => 1024,
  );
  
  $element['project_summary'] = array(
    '#type' => 'textarea',
    '#title' => t('Summary'),
    '#default_value' => isset($items[$delta]['project_summary']) ? $items[$delta]['project_summary'] : NULL,
    '#attributes' => array('class' => 'project_summary'),
  );
  
  if (empty($form['#parents']))
    $form['#parents'] = array();
  
  $element['_error_element'] = array(
    '#type' => 'value',
    '#value' => implode('][', array_merge($form['#parents'], array('value'))),
  );
  
  return $element;
}
