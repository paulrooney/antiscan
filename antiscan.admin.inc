<?php
/**
 * @file
 * Admin form to manage module settings
 *
 */
/**
 * Define the form for manage blocking restricted paths
 *
 */
function antiscan_form() {
  $config = config('antiscan.settings');
  $form   = array();
  
  $form['top'] = array(
    '#markup' => t('When a web crawler or even a person tries to visit a path that contains a pattern that matches one of defined in the field below, their IP address will be blocked.')
  );

  $path_patterns = $config->get('path_patterns');
  $form['path_patterns'] = array(
    '#type' => 'textarea',
    '#rows' => 6,
    '#columns' => 60,
    '#default_value' => $path_patterns,
    '#title' => t('Restricted path patterns'),
    '#required' => TRUE,
    '#description' => t('Enter paths or parts of paths to restrict, separating them with commas or new lines.')
      . '<br />' . t('Please note: the <b>*</b> character is a wildcard for end of string, so pattern like:') . '<b> wp-* </b>' 
      . t('will match to any of "wp-admin", "wp-login" and so on.'),
    
  );

  $url = url('admin/config/people/ip-blocking');
  $form['info'] = array(
    '#markup' => '<p><b>'. t('Please note') .': </b>' . t('you can manage the list of blocked IPs at  <a href="@url">this page</a>.', array('@url' => $url)) . '</p>',
  );

  $form['log_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable logging'),
    '#default_value' => $config->get('log_enabled'),
    '#description' => t('Enable logging for blocked access attempts.'), 
  );
  
  $form['unblock'] = array(
    '#type' => 'checkbox',
    '#title' => t('Unblock blocked IPs automatically after:') . '&nbsp;',
    '#default_value' => $config->get('unblock'),
    '#prefix' => '<div style="display:inline-flex;align-items:center">'
  );
  $form['unblock_after'] = array(
    '#type' => 'select',
    '#options' => array(
      86400 => t('1 day'),
      604800 => t('7 days'),
      2592000 => t('30 days'),
    ),
    '#default_value' => $config->get('unblock_after'),
    '#suffix' => '</div>',
    '#states' => array(
      'disabled' => array(
        ':input[name="unblock"]' => array('checked' => FALSE),
      ),
    ),  
  );
  
  $form['test_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Test mode'),
    '#default_value' => $config->get('test_mode'),
    '#description' => t('Enable for test your patterns, your IP will not be blocked, but you can see message when trying visit restricted path.'), 
  );
  
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  return $form;
}

/**
 * Implements hook_validate().
 */
function antiscan_form_validate($form, &$form_state) {
  $config         = config('antiscan.settings');
  $path_noblock   = $config->get('path_noblock');
  $path_patterns  = trim($form_state['values']['path_patterns']);
  
  if (strpos(strtolower($path_patterns), $path_noblock) !== FALSE) {
    form_set_error('path_patterns', t('You cannot use "%path_noblock" within pattern. If you need restrict users login ability, please use another module.', array('%path_noblock' => $path_noblock)));
  }
}

/**
 * Implements hook_submit().
 */
function antiscan_form_submit($form, &$form_state) {
  $config         = config('antiscan.settings');
  $path_patterns  = trim($form_state['values']['path_patterns']);
  $log_enabled    = (int) $form_state['values']['log_enabled'];
  $unblock        = (int) $form_state['values']['unblock'];
  $unblock_after  = (int) $form_state['values']['unblock_after']; 
  $test_mode      = (int) $form_state['values']['test_mode'];
  
  $config->set('path_patterns',  strtolower($path_patterns));
  $config->set('log_enabled',    $log_enabled);
  $config->set('unblock',        $unblock);
  $config->set('unblock_after',  $unblock_after);
  $config->set('test_mode',      $test_mode);
  
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));
}