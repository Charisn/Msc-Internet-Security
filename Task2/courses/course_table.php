<?php
// make the db connection
require_once('../config.php');

session_start();

// if there was no login done, redirect to login page.
if (!isset($_SESSION['logged_in']) || $_SESSION['logged_in'] !== true) {
    header('Location: ' . LOGIN . '/login.php');
    exit();
}
// if he is logged in, check the user access status and set it to a variable. This was optional.
if (isset($_SESSION['user']) && $_SESSION['user'] == 1) {
    $is_admin = true; // Set a variable to indicate if the user is an admin
} else {
    $is_admin = false; // Set a variable to indicate if the user is not an admin
}

include '../elements/header.php';

// I USED FUNCTIONS INSTEAD OF RAW CODE SNIPPETS WITH THE IDEA TO DUMP THEM INTO A FUNCTION.PHP FILE AND LOAD THEM FROM THERE. WHICH I DECIDED NOT TO DO IN THE END.

// function that prints data into a td to use later.
function printHtmlEncodedTextAsTd($htmlEncodedText) {
    echo '<td>' . htmlspecialchars_decode($htmlEncodedText) . '</td>'; // Print HTML-encoded text as a table data (td) element
}

// function that prints the checkboxes used in the form
function printCheckboxTd($courseId) {
    echo '<td><input type="checkbox" class="select-courses" name="course_ids[]" value="' . $courseId . '"></td>'; // Print a checkbox inside a table data (td) element
}

// function to delete a course given its id. This course could have multiple data values in multiple tables. Delete all of them.
function deleteCourse($conn, $courseId)
{
    $prefix = PREFIX;

    // Delete records from {$prefix}courses_content table
    $SQL = "DELETE FROM {$prefix}courses_content WHERE course_id = '$courseId'";
    $conn->query($SQL);

    // Delete records from {$prefix}modules table
    $SQL = "DELETE FROM {$prefix}modules WHERE course_id = '$courseId'";
    $conn->query($SQL);

    // Delete records from {$prefix}courses table
    $SQL = "DELETE FROM {$prefix}courses WHERE course_id = '$courseId'";
    $conn->query($SQL);
}

// handle post request for deleting selected courses.
if (isset($_POST['delete']) && $is_admin) {
    // Delete action
    if (isset($_POST['course_ids'])) {
        $selectedCourseIds = $_POST['course_ids']; // Store the selected course IDs

        foreach ($selectedCourseIds as $courseId) { // Iterate over each selected course ID
            deleteCourse($conn, $courseId); // Delete the course using the deleteCourse function
        }

        // Display success message
        echo "Courses successfully deleted.";
    }
}

// this is the main function that creates the html.
function getMergedTableDataAsHtml($conn) // Pass the connection as a parameter
{
    global $is_admin; // Access the global variable to check if the user is an admin
    $prefix = PREFIX;

    $SQL = "SELECT c.course_id, c.name, c.overview, c.key_facts, c.highlights, c.image_url, 
              GROUP_CONCAT(cc.tab_content SEPARATOR '<br>') AS content
              FROM {$prefix}courses AS c
              LEFT JOIN {$prefix}courses_content AS cc ON c.course_id = cc.course_id
              GROUP BY c.course_id";
    $result = $conn->query($SQL); // Execute the query to retrieve the merged table data

    // create a form. Form is needed to post delete and post data into charts.
    echo "<form id=\"myForm\" method=\"post\">";
    echo "<div class=\"buttons\">";

    // if user is not admin, hide delete button.
    if ($is_admin){
        echo "<div class=\"delete-button\">";
        echo "<input type=\"submit\" name=\"delete\" value=\"Delete Selected Courses\" onclick=\"checkSelection(event)\">"; // Show the delete button for admin users
        echo "</div>";
    }
    echo "<div class=\"charts-button\">";
    // Tried multiple ways to have 2 submits into a single form. The following link helped me to solve this with html5 formaction
    // https://stackoverflow.com/questions/547821/two-submit-buttons-in-one-form formaction
    echo "<input type=\"submit\" formaction=\"course_charts.php\" name=\"view_charts\" value=\"View Charts\" onclick=\"checkSelection(event)\">"; // Show the view charts button
    echo "</div>";
    echo "</div>";

    // if there are courses to show
    if ($result->num_rows > 0) {
        // create headers.
        echo "<div class=\"table-container\">";
        echo "<table class=\"merged-table\" border='1' style=\"max-height: 400px; overflow: auto;\">";
        echo "<tr>";
        echo "<th><input type=\"checkbox\" id=\"select-all\"></th>"; // Add a checkbox to select all courses
        echo "<th>Name</th>";
        echo "<th>Overview</th>";
        echo "<th>Key Facts</th>";
        echo "<th>Highlights</th>";
        echo "<th>Content</th>";
        echo "<th>Image</th>";
        echo "</tr>";

        // print all the data found calling the function created at the top
        while ($row = $result->fetch_assoc()) {
            echo "<tr>";
            echo "<td>";
            // i have 2 checkboxes.  1 for delete and 1 for charts. I did this because i had issues at first posting course array ids. 
            // Later solved it with an other way but decided not to reconstruct this part. I use javascript to link these 2. When checking select-courses, select-charts is selected aswell.
            echo '<input type="checkbox" class="select-courses" name="course_ids[]" value="' . $row['course_id'] . '">'; // Add a checkbox for selecting the course to delete
            echo '<input type="checkbox" class="select-charts" name="chart_course_ids[]" value="' . $row['course_id'] . '" style="display: none;">'; // Add a hidden checkbox for selecting the course for charts
            echo "</td>";
            printHtmlEncodedTextAsTd($row['name']); // Print the course name inside a table data (td) element
            printHtmlEncodedTextAsTd($row['overview']); // Print the course overview inside a table data (td) element
            printHtmlEncodedTextAsTd($row['key_facts']); // Print the course key facts inside a table data (td) element
            printHtmlEncodedTextAsTd($row['highlights']); // Print the course highlights inside a table data (td) element
            printHtmlEncodedTextAsTd($row['content']); // Print the course content inside a table data (td) element
            echo "<td><img src='". $row['image_url']. "'></td>"; // Display the course image inside a table data (td) element
            echo "</tr>";
        }

        echo "</table>";
        echo "</div>";
    } else {
        echo "No data found in the tables."; // Display a message if no courses are found
    }

    echo "</form>";
}

// call the function to start the
getMergedTableDataAsHtml($conn); // Call the main function to generate the merged table data as HTML

include '../elements/footer.php';
?>
