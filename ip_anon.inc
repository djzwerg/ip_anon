<?php

/**
 * @file
 * Page callbacks and utility functions for IP Anonymize module.
 */

/**
 * Anonymize IP addresses which have exceeded the retention period.
 */
function ip_anon_scrub() {
  foreach (ip_anon_tables() as $table => $columns) {
    db_update($table)
      ->fields(array($columns['hostname'] => 0))
      ->condition($columns['timestamp'], REQUEST_TIME - config_get('ip_anon.settings', "ip_anon_period_$table"), '<')
      ->execute();
  }
}

/**
 * Configuration options for IP Anonymize.
 */
function ip_anon_settings() {
  $config = config('ip_anon.settings');
  $form = array();
  $form['#config'] = 'ip_anon.settings';
  $form['ip_anon_policy'] = array(
    '#type' => 'radios',
    '#title' => t('Retention policy'),
    '#options' => array(
      0 => t('Preserve IP addresses'),
      1 => t('Anonymize IP addresses'),
    ),
    '#description' => t('This setting may be used to temporarily disable IP anonymization.'),
    '#default_value' => $config->get('ip_anon_policy'),
  );
  $form['ip_anon_period'] = array(
    '#type' => 'fieldset',
    '#title' => t('Retention period'),
    '#description' => t('IP addresses older than the retention period will be anonymized.'),
  );
  $options = backdrop_map_assoc(array(
    0,
    30,
    60,
    120,
    180,
    300,
    600,
    900,
    1800,
    2700,
    3600,
    5400,
    7200,
    10800,
    21600,
    32400,
    43200,
    64800,
    86400,
    172800,
    259200,
    345600,
    432000,
    518400,
    604800,
    1209600,
    1814400,
    2419200,
    2592000,
    4838400,
    5184000,
    7776000,
    9676800,
    15552000,
    23328000,
    31536000,
  ), 'format_interval');
  foreach (ip_anon_tables() as $table => $columns) {
    $schema = backdrop_get_schema($table);
    $schema = backdrop_get_schema_unprocessed($schema['module'], $table);
    $form['ip_anon_period']['ip_anon_period_' . $table] = array(
      '#type' => 'select',
      '#title' => t('@table table', array('@table' => $table)),
      '#options' => $options,
      '#default_value' => $config->get('ip_anon_period_' . $table),
      '#description' => isset($schema['description']) ? check_plain($schema['description']) : '',
    );
  }
  return system_settings_form($form);
}

/**
 * Default array of hostname and timestamp columns.
 */
function ip_anon_columns() {
  return backdrop_map_assoc(array('hostname', 'timestamp'));
}

/**
 * Array of tables and columns which store hostnames and timestamps.
 *
 * Modules may add tables by implementing hook_ip_anon_alter().
 */
function ip_anon_tables() {
  $tables = array('sessions' => ip_anon_columns());
  backdrop_alter('ip_anon', $tables);
  return $tables;
}

/**
 * Implements hook_ip_anon_alter() for comment module.
 */
function comment_ip_anon_alter(&$tables) {
  $tables['comment'] = array(
    'hostname' => 'hostname',
    'timestamp' => 'changed',
  );
}

/**
 * Implements hook_ip_anon_alter() for Commerce order module.
 */
function commerce_order_ip_anon_alter(&$tables) {
  $tables['commerce_order'] = array(
    'hostname' => 'hostname',
    'timestamp' => 'changed',
  );
}

/**
 * Implements hook_ip_anon_alter() for dblog module.
 */
function dblog_ip_anon_alter(&$tables) {
  $tables['watchdog'] = ip_anon_columns();
}

/**
 * Implements hook_ip_anon_alter() for Login Activity module.
 */
function login_activity_ip_anon_alter(&$tables) {
  $tables['login_activity'] = ip_anon_columns();
}

/**
 * Implements hook_ip_anon_alter() for Login History module.
 */
function login_history_ip_anon_alter(&$tables) {
  $tables['login_history'] = array(
    'hostname' => 'hostname',
    'timestamp' => 'login',
  );
}

/**
 * Implements hook_ip_anon_alter() for statistics module.
 */
function statistics_ip_anon_alter(&$tables) {
  $tables['accesslog'] = ip_anon_columns();
}

/**
 * Implements hook_ip_anon_alter() for Ubercart order module.
 */
function uc_order_ip_anon_alter(&$tables) {
  $tables['uc_orders'] = array(
    'hostname' => 'host',
    'timestamp' => 'modified',
  );
}

/**
 * Implements hook_ip_anon_alter() for user_stats module.
 */
function user_stats_ip_anon_alter(&$tables) {
  $tables['user_stats_ips'] = array(
    'hostname' => 'ip_address',
    'timestamp' => 'first_seen_timestamp',
  );
}

/**
 * Implements hook_ip_anon_alter() for Voting API module.
 */
function votingapi_ip_anon_alter(&$tables) {
  $tables['votingapi_vote'] = array(
    'hostname' => 'vote_source',
    'timestamp' => 'timestamp',
  );
}

/**
 * Implements hook_ip_anon_alter() for webform module.
 */
function webform_ip_anon_alter(&$tables) {
  $tables['webform_submissions'] = array(
    'hostname' => 'remote_addr',
    'timestamp' => 'submitted',
  );
}
