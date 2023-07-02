<?php
require_once('../config.php');
include 'login.html';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {

  $username = $_POST['username'];
  $password = $_POST['password'];
  $username = mysqli_real_escape_string($conn, $username);
  $SQL = "SELECT * FROM `" . PREFIX . "users` WHERE `username` LIKE '" . $username . "'";

  $result = $conn->query($SQL);

  if ($result) {
    if ($result->num_rows > 0) { // if any username found
      while ($row = $result->fetch_assoc()) {
        if (password_verify($password, $row['password'])) {
          if ($row['status'] == 0) {
            echo 'Account Is  Blocked. Please contact administrator';
            return;
          }
          session_start();
          $_SESSION['logged_in'] = true;

          header("Location: " . COURSE . "/course_table.php");
          exit();
        } else {
          echo 'Invalid Password';
        }
      }
    }
  } else {
    echo 'Invalid Username';
  }
}