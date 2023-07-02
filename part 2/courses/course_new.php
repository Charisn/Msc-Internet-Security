<?php
require_once('../config.php');
include('course_new.html');

session_start();

// Check if the user is not logged in
if (!isset($_SESSION['logged_in']) || $_SESSION['logged_in'] !== true) {
    // User is not logged in, redirect them to the login page
    header('Location: ' .LOGIN.'/login.php');
    exit();
}
?>

