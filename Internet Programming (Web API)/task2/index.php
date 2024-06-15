<?php

// index is first loaded on server

if (is_file('config.php')) {
	require_once('config.php');
}

header("Location: login/login.php");