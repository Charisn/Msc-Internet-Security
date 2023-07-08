<?php
// make db connection
require_once('../config.php');

$prefix = PREFIX;
    
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // get the data from inputs.
    $username = $_POST['username'];
    $password = $_POST['password'];

    // escape username
    $username = mysqli_real_escape_string($conn, $username);

    // create query
    $SQL = "SELECT * FROM {$prefix}users WHERE username = '$username'";
    $result = $conn->query($SQL);

    if ($result) {
        if ($result->num_rows > 0) { // if any username found
            $row = $result->fetch_assoc();
            if (!$row['status'] == 1) { // check if the account is active first
                echo '<script> alert("Account Is Blocked. Please contact the administrator");</script>';
                echo '<script> window.location.href = window.location.href; </script>';
                exit();
            } // if it is not, prevent from posting and show an alert box. After that, reload page.

            if (password_verify($password, $row['password'])) { // this is a function from php documentation that verifies user with encrypted password from db.
                session_start(); // if verified  start session
                $_SESSION['logged_in'] = true; // store a variable logged in to use later
                $_SESSION['user'] = $row['user_group_id']; // store a second variable to know the access rights to the user. This is not an actual security.

                // Reset tries to 0 on successful login to prevent blocking the account in the future.
                $SQL = "UPDATE {$prefix}users SET tries = 0 WHERE username = '$username'";
                $conn->query($SQL);
                header("Location: " . COURSE . "/course_table.php"); // redirect to main page.
                exit();
            } else { // if password not correct
                // Add one more try into the table
                $tries = $row['tries'] + 1;
                if ($tries > 3) {
                    // If maximum tries reached, set account status to 0
                    $SQL = "UPDATE {$prefix}users SET status = 0, tries = 0 WHERE username = '$username'";
                    $conn->query($SQL);
                    echo '<script>alert("Account Is Blocked. Please contact the administrator");</script>'; // If account blocked pop a message.
                } else { //if password wrong  but still have tries left
                    // Update tries column for the current user
                    $SQL = "UPDATE {$prefix}users SET tries = $tries WHERE username = '$username'";
                    $conn->query($SQL);
                    echo '<script>alert("Invalid Password");</script>'; // pop a message.
                }
            }
        } else { // if username not found
            echo '<script>alert("Invalid Username");</script>'; // pop a message.
        }
    }
}

include 'login.html';
