<?php
require_once('config.php');

// I used this to insert the  first user into the database. This was done for test purpose. 

$username = 'admin';
$password = 'admin';
$hashedPassword = password_hash($password, PASSWORD_DEFAULT);

$query = "REPLACE INTO " . PREFIX ."users (id, username, password, status, date_added, user_group_id, email)
          VALUES (NULL, '$username', '$hashedPassword', 1, NOW(), 1, test@test.com)";

// Execute the query
if ($conn->query($query)) {
    echo "User inserted successfully!";
} else {
    echo "Error ";
}
