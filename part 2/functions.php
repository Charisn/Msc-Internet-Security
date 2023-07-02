<?php
function errorLog($errorMsg) {
  $logFile = 'error_log.txt'; // Specify the path to your error log file

  // Format the error message with timestamp
  $errorString = "[" . date("Y-m-d H:i:s") . "] " . $errorMsg . PHP_EOL;

  // Append the error message to the log file
  error_log($errorString, 3, $logFile);
}
