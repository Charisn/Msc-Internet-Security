<?php

if (is_file('config.php')) {
	require_once('config.php');
}

header("Location: ". LOGIN ."/login.php");