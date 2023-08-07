<?php

ob_start();

// Include the configuration file
require_once('../config.php');

$prefix = PREFIX;

// Start a new session or resume the existing one
session_start();

// If the user is not logged in, redirect them to the login page
if (
    !isset($_SESSION['logged_in'])
    || $_SESSION['logged_in'] !== true
) {
    header('Location: ' . LOGIN . '/login.php');
    exit();
}

// If the request method is POST, then the form has been submitted
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Turn off autocommit to start a new transaction, useful for multiple database operations
    $conn->autocommit(FALSE);

    // Get the form inputs and escape special characters to prevent SQL injection
    $name = mysqli_real_escape_string($conn, $_POST['name']);
    $overview = mysqli_real_escape_string($conn, $_POST['overview']);
    $keyFacts = mysqli_real_escape_string($conn, $_POST['key_facts']);
    $highlights = mysqli_real_escape_string($conn, $_POST['highlights']);
    $imageUrl = mysqli_real_escape_string($conn, $_POST['image_url']);

    // If any of the values are empty, stop the script and show an error message
    if (!$name || !$overview || !$keyFacts || !$highlights || !$imageUrl) {
        echo '<script>alert("Some values are empty");</script>';
        echo '<script>window.location.href = window.location.href;</script>';
        return;
    }

    // Prepare an SQL query to insert the course data into the database
    $SQL = "INSERT INTO {$prefix}courses (name, overview, key_facts, highlights, image_url) 
            VALUES ('$name', '$overview', '$keyFacts', '$highlights', '$imageUrl')";
    // Execute the query
    $conn->query($SQL);

    // Get the ID of the course that was just inserted
    $courseId = $conn->insert_id;

    // Get the arrays of tab names and contents from the form inputs
    $tabNames = $_POST['tab_name'] ?? []; // Use an empty array if the value is not set
    $tabContents = $_POST['tab_content'] ?? [];

    // For each tab, prepare an SQL query to insert the tab data into the database
    foreach ($tabNames as $index => $tabName) {
        $tabContent = mysqli_real_escape_string($conn, $tabContents[$index]);
        $tabName = mysqli_real_escape_string($conn, $tabName);
        $SQL = "INSERT INTO {$prefix}courses_content (course_id, tab_name, tab_content) 
                VALUES ($courseId, '$tabName', '$tabContent')";
        // Execute the query
        $conn->query($SQL);
    }

    // Get the arrays of module names and credits from the form inputs
    $moduleNames = $_POST['module_name'] ?? [];
    $moduleCreditValues = $_POST['module_credits'] ?? [];

    // For each module, prepare an SQL query to insert the module data into the database
    foreach ($moduleNames as $index => $moduleName) {
        $moduleCredits = (int) $moduleCreditValues[$index];
        $moduleName = mysqli_real_escape_string($conn, $moduleName);
        $SQL = "INSERT INTO {$prefix}modules (course_id, name, credits) 
                VALUES ($courseId, '$moduleName', $moduleCredits)";
        // Execute the query
        $conn->query($SQL);
    }

    // If an error occurred during the queries, rollback the transaction and show an error message
    if ($conn->error) {
        $conn->rollback();
        echo "Failed to insert: (" . $conn->errno . ") " . $conn->error;
    } else {
        // If everything went well, commit the transaction and redirect the user to the new course page
        $conn->commit();
        echo '<script>alert("Success!");</script>';
        header('Location: course_new.php');
        exit();
    }
}

include '../elements/header.php';
?>
<div class="course-new">
    <main>
        <h3>Προσθήκη Νέου Μαθήματος</h3>
        <div class="container">
            <div class="all-courses-container">
                <div class="create-course">
                    <form name="add_name" id="add_name" action="course_new.php" method="POST">
                        <div class="form-container">
                            <div class="form-inside-container">
                                <div class="goup-inputs">
                                    <label for="name">Name:</label>
                                    <textarea type="text" name="name" class="custom-input" id="name"></textarea>
                                </div>
                                <div class="goup-inputs overview">
                                    <label for="overview">Overview:</label>
                                    <textarea name="overview" class="custom-textarea" id="overview"></textarea>
                                </div>
                                <div class="goup-inputs key-facts">
                                    <label for="key_facts">Key Facts:</label>
                                    <textarea name="key_facts" class="custom-textarea" id="key_facts"></textarea>
                                </div>
                                <div class="goup-inputs highlights">
                                    <label for="highlights">Highlights:</label>
                                    <textarea name="highlights" class="custom-textarea" id="highlights"></textarea>
                                </div>
                                <div class="goup-inputs image-url">
                                    <label for="image_url">Image URL:</label>
                                    <textarea type="text" name="image_url" class="custom-input" id="image_url"></textarea>
                                </div>
                            </div>

                            <div class="course-group">
                                <div class="goup-inputs course-content">
                                    <label>Course Content</label>
                                    <div id="course_content">
                                        <div class="content_row">
                                            <input type="text" name="tab_name[]" class="custom-input" placeholder="Course Name">
                                            <textarea name="tab_content[]" class="custom-textarea" placeholder="Course Content"></textarea>
                                            <button type="button" class="remove_content_btn">Remove</button>
                                        </div>
                                    </div>
                                    <button type="button" class="add_content_btn">Add More Content</button>
                                </div>
                            
                                <div class="goup-inputs modules">
                                    <label>Modules</label>
                                    <div id="course_modules">
                                        <div class="module_row">
                                            <input type="text" name="module_name[]" class="custom-input" placeholder="Module Name">
                                            <input type="number" name="module_credits[]" class="custom-input" placeholder="Module Credits" oninput="this.value = this.value.replace(/[^0-9]/g, '')">
                                            <button type="button" class="remove_module_btn">Remove</button>
                                        </div>
                                    </div>
                                    <button type="button" class="add_module_btn">Add More Modules</button>
                                </div>
                            </div>

                        </div>
                        <div class="buttons button-submit-new">
                            <input type="submit" value="Submit" class="submit-new-button">
                        </div>
                    </form>
                </div>
            </div>
        </div>
        <!-- LOADING  JQUERY TO MAKE DUPLICATING INPUTS EASIER -->
        <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
    </main>
</div>
<script>
    // This script adds new html inputs into course_new.html for Course contents
$(document).ready(function () {
    // Track the number of course content rows
    var contentCount = 1;

    // Track the number of module rows
    var moduleCount = 1;

    // Add more content row
    $(".add_content_btn").click(function () {
        var contentRow = `
            <div class="content_row">
                <input type="text" name="tab_name[]" class="custom-input" placeholder="Course Name">
                <textarea name="tab_content[]" class="custom-textarea" placeholder="Course Content"></textarea>
                <button type="button" class="remove_content_btn">Remove</button>
            </div>`;
        $("#course_content").append(contentRow);
        contentCount++;
    });

    // Remove content row
    $(document).on('click', '.remove_content_btn', function () {
        $(this).closest('.content_row').remove();
    });

    // Add more module row
    $(".add_module_btn").click(function () {
        var moduleRow = `
            <div class="module_row">
                <input type="text" name="module_name[]" class="custom-input" placeholder="Module Name">
                <input type="number" name="module_credits[]" class="custom-input" placeholder="Module Credits" oninput="this.value = this.value.replace(/[^0-9]/g, '')">
                <button type="button" class="remove_module_btn">Remove</button>
            </div>`;
        $("#course_modules").append(moduleRow);
        moduleCount++;
    });

    // Remove module row
    $(document).on('click', '.remove_module_btn', function () {
        $(this).closest('.module_row').remove();
    });
});

</script>

<?php

include '../elements/footer.php';
?>