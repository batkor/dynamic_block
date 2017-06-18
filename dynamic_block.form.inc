<?php

function dynamic_block_setting_form($form, &$form_state) {

  $saved_block_module_delta = variable_get('dynamic_block_block_module_delta', 'none');
  $form['block_module_delta'] = array(
    '#type'          => 'select',
    '#title'         => t('Selected block'),
    '#options'       => dynamic_block_get_all_blocks(),
    '#default_value' => $saved_block_module_delta,
  );

  $form['submit_selected'] = array(
    '#type'     => 'submit',
    '#value'    => t('Select and save'),
    '#submit'   => array('dynamic_block_setting_form_submit_selected'),
    '#validate' => array('dynamic_block_setting_form_submit_selected_validate'),
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
    '#empty'   => t('Select the block first'),
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

  if ($saved_block_module_delta != 'none') {
    $form['new_setting']['new_url'] = array(
      '#type'        => 'textarea',
      '#title'       => t('Indicate url'),
      '#description' => t('Enter the path to each page in a separate line.'),
    );

    $form['new_setting']['new_body'] = array(
      '#type'   => 'text_format',
      '#title'  => t('Body'),
      '#format' => NULL,
    );

    $form['new_setting']['add_setting'] = array(
      '#type'     => 'submit',
      '#value'    => t('Add settings and save'),
      '#submit'   => array('dynamic_block_setting_form_add_setting'),
      '#validate' => array('dynamic_block_setting_form_add_setting_validate'),
    );
  }

  $form['clear_settings'] = array(
    '#type'   => 'submit',
    '#value'  => t('Clear all settings'),
    '#submit' => array('dynamic_block_setting_form_clear_settings'),
  );

  return $form;
}

function dynamic_block_setting_form_submit_selected($form, &$form_state) {
  variable_set('dynamic_block_block_module_delta', $form_state['values']['block_module_delta']);
}

function dynamic_block_setting_form_submit_selected_validate($form, &$form_state) {
  if ($form_state['values']['block_module_delta'] == 'none') {
    form_set_error('block_module_delta', t('Please selected block'));
  }
}

function dynamic_block_setting_form_add_setting($form, &$form_state) {
  $vars = variable_get('dynamic_block_setting_vars', array());
  $urls = str_replace("\r\n", ', ', $form_state['values']['new_url']);
  $vars[] = array(
    'url'  => $urls,
    'body' => $form_state['values']['new_body'],
  );
  variable_set('dynamic_block_setting_vars', $vars);
}

function dynamic_block_setting_form_add_setting_validate($form, &$form_state) {
  if ($form_state['values']['new_url'] == '') {
    form_set_error('new_setting][new_url', t('Please fill in the field "URL"'));
  }
  if ($form_state['values']['new_body'] == '') {
    form_set_error('new_setting][new_body', t('Please fill in the field "Body"'));
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

function dynamic_block_setting_form_clear_settings($form, &$form_state) {
  variable_del('dynamic_block_setting_vars');
  variable_del('dynamic_block_block_module_delta');
}

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
        $var_settings['body'] = $var['body'];
      }
    }
  }

  $form['url'] = array(
    '#type'        => 'textarea',
    '#title'       => t('Indicate url'),
    '#description' => t('Enter the path to each page in a separate line.'),
    '#default_value' => $var_settings['url'],
  );

  $form['body'] = array(
    '#type'   => 'text_format',
    '#title'  => t('Body'),
    '#default_value' => (!empty($var_settings['body']))? $var_settings['body']['value']:'',
    '#format' => (!empty($var_settings['body']))? $var_settings['body']['format']:'',
  );

  $form['save_setting'] = array(
    '#type'     => 'submit',
    '#value'    => t('Save settings'),
    '#submit'   => array('dynamic_block_setting_form_save_setting'),
    '#validate' => array('dynamic_block_setting_form_save_setting_validate'),
  );

  return $form;
}

function dynamic_block_setting_form_save_setting($form, &$form_state){
  $vars = variable_get('dynamic_block_setting_vars', array());
  if(!empty($vars)){
    foreach ($vars as $key => $var) {
      if($key==$form_state['key']){
        $urls = str_replace("\r\n", ', ', $form_state['values']['url']);
        $vars[$key] = array(
          'url'  => $urls,
          'body' => $form_state['values']['body'],
        );
      }
    }
  }
  variable_set('dynamic_block_setting_vars', $vars);
  drupal_goto('admin/structure/block/dynamic_block');
}

function dynamic_block_setting_form_save_setting_validate($form, &$form_state){
  if ($form_state['values']['url'] == '') {
    form_set_error('url', t('Please fill in the field "URL"'));
  }
  if ($form_state['values']['body'] == '') {
    form_set_error('body', t('Please fill in the field "Body"'));
  }
}