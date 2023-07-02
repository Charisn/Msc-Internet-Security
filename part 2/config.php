<?php

define('DIR', 'C:\xampp\htdocs\final');

define('LOGIN', ''.DIR.'/login');
define('COURSE', ''.DIR.'/courses');
define('HTML', ''.DIR.'/elements');

// DATABASE CREDENTIALS
define('HOSTNAME', 'localhost');
define('USERNAME', 'charis');
define('PASSWORD', '_Oc[IhpFTQ91]v3P');
define('DATABASE', 'csym019');
define('PREFIX', 'csym_');
define('PORT', '3306');

try  {
    $conn = new mysqli (HOSTNAME, USERNAME, PASSWORD, DATABASE);
}  catch(Error) {
    exit;
}