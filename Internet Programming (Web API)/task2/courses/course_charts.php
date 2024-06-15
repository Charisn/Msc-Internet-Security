<?php
// Include the configuration file, which establishes a connection to the database.
require_once('../config.php');

// Include the header file, containing the top part of the HTML page.
include '../elements/header.php';

// Output a script tag that includes the Chart.js library, which is used to generate charts.
echo '<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>';

// Check if the form has been submitted by checking POST parameters
if (isset($_POST['view_charts']) && isset($_POST['chart_course_ids'])) {
    // Get the selected course IDs
    $chartCourseIds = $_POST['chart_course_ids'];

    echo "<div class=\"course-charts\">"; // Output the opening tag for the course charts container

    foreach ($chartCourseIds as $courseId) {
        // Fetch course details
        $courseQuery = "SELECT name FROM csym_courses WHERE course_id = $courseId"; // Query to fetch course name
        $courseResult = $conn->query($courseQuery); // Execute the course query
        $courseRow = $courseResult->fetch_assoc(); // Fetch the course details from the result
        $courseName = $courseRow['name']; // Get the course name

        // Fetch module details for the course
        $moduleQuery = "SELECT name, credits FROM csym_modules WHERE course_id = $courseId"; // Query to fetch module details
        $moduleResult = $conn->query($moduleQuery); // Execute the module query

        echo "<div class=\"course-container\">"; // Output the opening tag for the course container
        echo "<h1>$courseName</h1>"; // Output the course name as the heading
        echo "<div class=\"course-details\">"; // Output the opening tag for the course details container
        echo "<div class=\"course-description\">"; // Output the opening tag for the course description container

        while ($moduleRow = $moduleResult->fetch_assoc()) {
            $moduleName = $moduleRow['name']; // Get the module name
            $moduleCredits = $moduleRow['credits']; // Get the module credits

            echo "<div class=\"course-information\">"; // Output the opening tag for the course information container
            echo "<p>$moduleName: </p>"; // Output the module name
            echo "<span> &nbsp; Credits: $moduleCredits</span>"; // Output the module credits
            echo "</div>"; // End of course-information div
        }

        echo "</div>"; // End of course-description div
        echo "</div>"; // End of course-details div

         $creditsQuery = "SELECT credits FROM csym_modules WHERE course_id = $courseId"; // Query to fetch module credits
        $creditsResult = $conn->query($creditsQuery); // Execute the credits query
    
        $creditsData = array(); // Initialize an array to store the module credits
        $moduleNames = array(); // Initialize an array to store the module names
    
        while ($creditsRow = $creditsResult->fetch_assoc()) {
            $creditsData[] = $creditsRow['credits']; // Add the module credits to the array
        }
    
        // Fetch module names for the pie chart
        $namesQuery = "SELECT name FROM csym_modules WHERE course_id = $courseId"; // Query to fetch module names
        $namesResult = $conn->query($namesQuery); // Execute the names query
    
        while ($namesRow = $namesResult->fetch_assoc()) {
            $moduleNames[] = $namesRow['name']; // Add the module name to the array
        }
    
        // Generate labels and data for the chart
        $labels = $moduleNames; // Assign moduleNames as the labels array
        $data = $creditsData; // Assign creditsData as the data array

        echo "<div class=\"course-pie\">"; // Output the opening tag for the course pie container
        echo "<canvas id=\"coursePieChart$courseId\"></canvas>"; // Output the canvas element for the pie chart
        echo "<script>"; // Output the opening tag for the script tag

        // Generate JavaScript code to create and customize the pie chart
        echo "var ctx = document.getElementById('coursePieChart$courseId').getContext('2d');"; // Get the chart context
        echo "var chart = new Chart(ctx, {"; // Create a new Chart object
        echo "  type: 'pie',"; // Set the chart type to pie
        echo "  data: {"; // Set the chart data
         echo "    labels: " . json_encode($labels) . ","; // Set the chart labels
        echo "    datasets: [{"; // Set the chart datasets
        echo "      data: " . json_encode($data) . ","; // Set the chart data
        echo "      backgroundColor: getRandomColors(" . count($data) . ")"; // Set the chart background colors
        echo "    }]"; // End of datasets
        echo "  },"; // End of data
        echo "  options: {}"; // Set the chart options
        echo "});"; // End of Chart object creation
        echo "</script>"; // End of script tag
        echo "</div>"; // End of course-pie div
        echo "</div>"; // End of course-container div
    }

    echo "</div>"; // End of course-charts div

    // If there is more than one course, display a stacked scale chart comparing the courses.
    if (count($chartCourseIds) > 1) {
        // Prepare the data for the stacked scale chart.
        $courseNames = array(); // Initialize array to store course names
        $courseCredits = array(); // Initialize array to store course credits

        foreach ($chartCourseIds as $courseId) {
            // Fetch course details
            $courseQuery = "SELECT name FROM csym_courses WHERE course_id = $courseId"; // Query to fetch course name
            $courseResult = $conn->query($courseQuery); // Execute the course query
            $courseRow = $courseResult->fetch_assoc(); // Fetch the course details from the result
            $courseName = $courseRow['name']; // Get the course name

            // Fetch module details for the course
            $moduleQuery = "SELECT name, credits FROM csym_modules WHERE course_id = $courseId"; // Query to fetch module details
            $moduleResult = $conn->query($moduleQuery); // Execute the module query

            $moduleNames = array(); // Initialize array to store module names
            $moduleCredits = array(); // Initialize array to store module credits

            while ($moduleRow = $moduleResult->fetch_assoc()) {
                $moduleName = $moduleRow['name']; // Get the module name
                $moduleCredit = $moduleRow['credits']; // Get the module credits

                $moduleNames[] = $moduleName; // Add module name to array
                $moduleCredits[] = $moduleCredit; // Add module credit to array
            }

            $courseNames[] = $courseName; // Add course name to array
            $courseCredits[] = $moduleCredits; // Add module credits to array
        }
        
        // Stacked scale chart
        echo "<div class=\"stacked-scale\">"; // Output the opening tag for the stacked scale chart container
        echo "<canvas id=\"stackedScaleChart\"></canvas>"; // Output the canvas element for the stacked scale chart
        echo "<script>"; // Output the opening tag for the script tag
        echo "var ctx =  document.getElementById('stackedScaleChart').getContext('2d');"; // Get the chart context
        echo "var stackedScaleChart = new Chart(ctx, {"; // Create a new Chart object
        echo "  type: 'bar',"; // Set the chart type to bar
        echo "  data: {"; // Set data
        echo "    labels: " . json_encode($courseNames) . ","; // Set the chart labels
        echo "    datasets: ["; // Start of datasets

        foreach ($chartCourseIds as $courseId) {
            $moduleCreditsQuery = "SELECT credits FROM csym_modules WHERE course_id = $courseId";
            $moduleCreditsResult = $conn->query($moduleCreditsQuery);
            
            $moduleCredits = array();
            while ($creditsRow = $moduleCreditsResult->fetch_assoc()) {
                $moduleCredits[] = $creditsRow['credits'];
            }
    
            // Start of dataset
            echo "{";
            
            // Set the dataset label
            echo "  label: " . json_encode($courseName) . ",";
            
            // Set the dataset data
            echo "  data: " . json_encode($moduleCredits) . ",";
            
            // Set the dataset background color
            echo "  backgroundColor: getRandomColors(" . count($moduleCredits) . ")";
            
            // End of dataset
            echo "}"; 
    
            // Add a comma if it's not the last dataset
            if ($courseId !== end($chartCourseIds)) {
                echo ","; 
            }
        }


        echo "    ]"; // End of datasets
        echo "  },"; // End of data
        echo "  options: {"; // Start of options
        echo "    scales: {"; // Start of scales
        echo "      x: {"; // Start of x-axis scale
        echo "        stacked: false,"; // Set stacked property to false
        echo "        ticks: {"; // Start of x-axis ticks
        echo "          beginAtZero: true"; // Set beginAtZero property to true
        echo "        }"; // End of x-axis ticks
        echo "      },"; // End of x-axis scale
        echo "      y: {"; // Start of y-axis scale
        echo "        stacked: false,"; // Set stacked property to false
        echo "        display: true,"; // Set display property to true
        echo "        scaleLabel: {"; // Start of y-axis scale label
        echo "          display: true,"; // Set display property to true
        echo "          labelString: 'Credits'"; // Set the label string
        echo "        }"; // End of y-axis scale label
        echo "      }"; // End of y-axis scale
        echo "    }"; // End of scales
        echo "  }"; // End of options
        echo "});"; // End of Chart object creation
        echo "</script>"; // End of script tag
        echo "</div>"; // End of stacked-scale div
    }
} else {
    echo "No courses selected."; // Output the message if no courses are selected
}

include '../elements/footer.php'; // Include the footer file
?>
