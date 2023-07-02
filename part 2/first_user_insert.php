<?php
require_once('config.php');

$username = 'admin';
$password = 'admin';
$hashedPassword = password_hash($password, PASSWORD_DEFAULT);

$query = "REPLACE INTO " . PREFIX ."users (id, username, password, status, date_added, user_group_id, email)
          VALUES (NULL, '$username', '$hashedPassword', 1, NOW(), 1, ncharis97@gmail.com)";

// Execute the query
if ($conn->query($query)) {
    echo "User inserted successfully!";
} else {
    echo "Error ";
}
