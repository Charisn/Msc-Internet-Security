<?php
// make db connection
require_once('../config.php');

$prefix = PREFIX;
if ($_SERVER['REQUEST_METHOD'] === 'POST') {

    // get the information from the HTML form
    $username = $_POST['username'];
    $password = $_POST['password'];
    $email = $_POST['email'];

    // encrypt and escape data
    $username = mysqli_real_escape_string($conn, $username);
    $password = password_hash($password, PASSWORD_DEFAULT);
    $email = mysqli_real_escape_string($conn, $email);

    // create the query
    // status always 1 at first insert
    // user group always 2, it should only have simple user privileges. 1 = admin
    // tries always set to 0. Could skip since it's defaulted to 0 but decided to include it in the query
    $SQL = "INSERT INTO {$prefix}users (username, password, status, date_added, email, user_group_id, tries)
            VALUES ('$username', '$password', 1, NOW(), '$email', 2, 0)";

    if ($conn->query($SQL) === TRUE) {
        // if success, redirect to login
        header("Location: login.php");
        exit();
    } else {
        echo 'Error: ' . $conn->error;
    }
}

include 'register.html';
