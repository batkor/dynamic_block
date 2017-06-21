<?php
/**
 * @see dynamic_block_menu
 * form setting form
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function dynamic_block_setting_form($form, &$form_state) {

  $form['block_module_delta'] = array(
    '#markup' => '<ul id="dynamic-block-list"><label for="dynamic-block-list">Созданы динамические блоки</label>'
      .'<li><a href="' . $GLOBALS['base_url'] . '/admin/structure/block/manage/dynamic_block/dynamic-block-header/configure">Dynamic block header</a></li>'
      .'<li><a href="' . $GLOBALS['base_url'] . '/admin/structure/block/manage/dynamic_block/dynamic-block-footer/configure">Dynamic block footer</a></li></ul>',
  );

  $header = array(
    'url'     => t('Url'),
    'body'    => t('Body'),
    'actions' => t('Actions'),
  );
  $options = dynamic_block_get_settings_vars();

  $form['settings'] = array(
    '#type'    => 'tableselect',
    '#title'   => t('Settings'),
    '#header'  => $header,
    '#options' => $options,
    '#empty'   => t('No settings yet'),
  );

  $form['remove_settings'] = array(
    '#type'   => 'submit',
    '#value'  => t('Remove selected settings'),
    '#submit' => array('dynamic_block_setting_form_remove_settings'),
  );
  $form['new_setting'] = array(
    '#type'   => 'fieldset',
    '#title'  => t('New settings'),
    '#prefix' => '<div id="dynamic_block_settings">',
    '#suffix' => '</div>',
  );

  $form['new_setting']['new_url'] = array(
    '#type'        => 'textarea',
    '#title'       => t('Indicate url'),
    '#description' => t('Enter the path to each page in a separate line.'),
  );

  $form['new_setting']['new_body_header'] = array(
    '#type'   => 'text_format',
    '#title'  => t('Body for Dynamic block header'),
    '#format' => NULL,
  );

  $form['new_setting']['new_body_footer'] = array(
    '#type'   => 'text_format',
    '#title'  => t('Body for Dynamic block footer'),
    '#format' => NULL,
  );

  $form['new_setting']['add_setting'] = array(
    '#type'     => 'submit',
    '#value'    => t('Add settings and save'),
    '#submit'   => array('dynamic_block_setting_form_add_setting'),
    '#validate' => array('dynamic_block_setting_form_add_setting_validate'),
  );

  $form['clear_settings'] = array(
    '#type'   => 'submit',
    '#value'  => t('Clear all settings'),
    '#submit' => array('dynamic_block_setting_form_clear_settings'),
  );

  return $form;
}

/**
 * @see dynamic_block_setting_form
 *
 * @param $form
 * @param $form_state
 */
function dynamic_block_setting_form_submit_selected_validate($form, &$form_state) {
  if ($form_state['values']['block_module_delta'] == 'none') {
    form_set_error('block_module_delta', t('Please selected block'));
  }
}

/**
 * @see dynamic_block_setting_form
 *
 * @param $form
 * @param $form_state
 */
function dynamic_block_setting_form_add_setting($form, &$form_state) {
  $vars = variable_get('dynamic_block_setting_vars', array());
  $urls = str_replace("\r\n", ', ', $form_state['values']['new_url']);
  $vars[] = array(
    'url'  => $urls,
    'body_header' => $form_state['values']['new_body_header'],
    'body_footer' => $form_state['values']['new_body_footer'],
  );
  variable_set('dynamic_block_setting_vars', $vars);
}

/**
 * @see dynamic_block_setting_form
 *
 * @param $form
 * @param $form_state
 */
function dynamic_block_setting_form_add_setting_validate($form, &$form_state) {
  if ($form_state['values']['new_url'] == '') {
    form_set_error('new_setting][new_url', t('Please fill in the field "URL"'));
  }

  $urls = explode("\r\n", $form_state['values']['new_url']);
  $vars = variable_get('dynamic_block_setting_vars', array());
  foreach ($vars as $var) {
    foreach ($urls as $url) {
      $pos = strripos($var['url'], $url);
      if ($pos !== FALSE) {
        form_set_error('new_setting][new_url', t('URL "@url" already set', array('@url' => $url)));
      }
    }
  }
}

/**
 * @see dynamic_block_setting_form
 * remove items setting
 *
 * @param $form
 * @param $form_state
 */
function dynamic_block_setting_form_remove_settings($form, &$form_state) {
  $vars = variable_get('dynamic_block_setting_vars', array());
  if (isset($form_state['input']['settings'])) {
    foreach ($form_state['input']['settings'] as $key => $setting) {
      if (!is_null($setting)) {
        unset($vars[$key]);
      }
    }
  }
  variable_set('dynamic_block_setting_vars', $vars);
}

/**
 * @see dynamic_block_setting_form
 * clear all settings module
 *
 * @param $form
 * @param $form_state
 */
function dynamic_block_setting_form_clear_settings($form, &$form_state) {
  variable_del('dynamic_block_setting_vars');
}

/**
 * @see dynamic_block_menu
 * form change setting
 *
 * @param $form
 * @param $form_state
 * @param $key
 *
 * @return mixed
 */
function dynamic_block_change_setting_form($form, &$form_state, $key) {
  $form_state['key'] = $key;
  $vars = variable_get('dynamic_block_setting_vars', array());
  $var_settings = array(
    'url'  => '',
    'body' => '',
  );
  if (!empty($vars)) {
    foreach ($vars as $key_var => $var) {
      if ($key_var == $key) {
        $var_settings['url'] = $var['url'];
        $var_settings['body_header'] = $var['body_header'];
        $var_settings['body_footer'] = $var['body_footer'];
      }
    }
  }

  $form['url'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Indicate url'),
    '#description'   => t('Enter the path to each page in a separate line.'),
    '#default_value' => $var_settings['url'],
  );

  $form['body_header'] = array(
    '#type'          => 'text_format',
    '#title'         => t('Body for Dynamic block header'),
    '#default_value' => (!empty($var_settings['body_header'])) ? $var_settings['body_header']['value'] : '',
    '#format'        => (!empty($var_settings['body_header'])) ? $var_settings['body_header']['format'] : '',
  );

  $form['body_footer'] = array(
    '#type'          => 'text_format',
    '#title'         => t('Body for Dynamic block footer'),
    '#default_value' => (!empty($var_settings['body_footer'])) ? $var_settings['body_footer']['value'] : '',
    '#format'        => (!empty($var_settings['body_footer'])) ? $var_settings['body_footer']['format'] : '',
  );

  $form['save_setting'] = array(
    '#type'     => 'submit',
    '#value'    => t('Save settings'),
    '#submit'   => array('dynamic_block_setting_form_save_setting'),
    '#validate' => array('dynamic_block_setting_form_save_setting_validate'),
  );

  return $form;
}

/**
 * @see dynamic_block_change_setting_form
 *
 * @param $form
 * @param $form_state
 */
function dynamic_block_setting_form_save_setting($form, &$form_state) {
  $vars = variable_get('dynamic_block_setting_vars', array());
  if (!empty($vars)) {
    foreach ($vars as $key => $var) {
      if ($key == $form_state['key']) {
        $urls = str_replace("\r\n", ', ', $form_state['values']['url']);
        $vars[$key] = array(
          'url'  => $urls,
          'body_header' => $form_state['values']['body_header'],
          'body_footer' => $form_state['values']['body_footer'],
        );
      }
    }
  }
  variable_set('dynamic_block_setting_vars', $vars);
  drupal_goto('admin/structure/block/dynamic_block');
}

/**
 * @see dynamic_block_change_setting_form
 *
 * @param $form
 * @param $form_state
 */
function dynamic_block_setting_form_save_setting_validate($form, &$form_state) {
  if ($form_state['values']['url'] == '') {
    form_set_error('url', t('Please fill in the field "URL"'));
  }
}