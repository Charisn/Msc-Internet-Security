<?php
require_once('../config.php');
include('/course_table.html');

session_start();

// Check if the user is not logged in
if (!isset($_SESSION['logged_in']) || $_SESSION['logged_in'] !== true) {
    // User is not logged in, redirect them to the login page
    header('Location: ' .LOGIN.'/login.php');
    exit();
}

$modules = $_GET["modules"];

try {
    foreach ($modules as $module) {
        $sql = "INSERT INTO modules (title) VALUES ('" . $module . "')";
        $conn->query($sql);
    }
    echo "New record created successfully";
} catch (Exception $error) {
    $errorMessage = $sql . "<br>" . $error->getMessage();

    // Log the error message to a file using error_log
    error_log($errorMessage, 3, "error.log");

    // Display the error message to the user (optional)
    echo $errorMessage;
}
$conn = null; 

?>