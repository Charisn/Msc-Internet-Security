<?php
require_once('../config.php');
include 'register.html';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {

  $username = $_POST['username'];
  $password = $_POST['password'];
  $email = $_POST['email'];
  
  $username = mysqli_real_escape_string($conn, $username);
  $password = password_hash($password, PASSWORD_DEFAULT);
  $email = mysqli_real_escape_string($conn, $email);
  
  $SQL = "SELECT * FROM `" . PREFIX . "users` WHERE `username` LIKE '" . $username . "'";

  $query = "INSERT INTO " . PREFIX ."users (id, username, password, status, date_added, user_group_id, email)
            VALUES (NULL, '$username', '$password', 1, NOW(), 1,  '$email')";

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