<?php
define('DIR', '/part-2');

define('LOGIN', DIR . '/login');
define('COURSE', DIR . '/courses');
define('HTML', DIR . '/elements');

// DEFINE DIRS TO USE LATER

// DATABASE CREDENTIALS
define('HOSTNAME', 'localhost');
define('USERNAME', 'nikolaidis_csym_part2_usr');
define('PASSWORD', '5Jpqrn]$eEv0wW6G9m');
define('DATABASE', 'nikolaidis_db');
define('PREFIX', 'csym_');
define('PORT', '3306');

try {
    $conn = new mysqli(HOSTNAME, USERNAME, PASSWORD, DATABASE);
} catch (Exception $e) {
    exit;
}
