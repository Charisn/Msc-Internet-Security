<?php
require_once('../config.php');

session_start();

// Check if the user is not logged in
if (!isset($_SESSION['logged_in']) || $_SESSION['logged_in'] !== true) {
    // User is not logged in, redirect them to the login page
    header('Location: ' . LOGIN . '/login.php');
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

include HTML . '/header.php'; ?>

<div>Insert New Course</div><br>
<div>
    <form action="courses.php" method="get">
        <label for="name">Name: </label>
        <input type="text"  id="name" name="name"><br>

        <label for="name">Overview: </label>
        <input type="text" id="overview" name="overview"><br>

        <label for="name">Key Facts: </label>
        <input type="text" id="key_facts" name="key_facts"><br>

        <label for="name">Highlights: </label>
        <input type="text" id="highlights" name="highlights"><br>

        <label for="modules[]">List of Content: </label><br>
        <input type="text" id="modules[]" name="modules[]"><br>
        <input type="text" id="modules[]" name="modules[]"><br>
        <input type="text" id="modules[]" name="modules[]"><br><br>

        <input type="submit">
    </form>
</div>

<?php include HTML . '/footer.php'; ?>