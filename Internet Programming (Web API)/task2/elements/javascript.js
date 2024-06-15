
// This simple js funtion adds an event listener to the th checkbox. If it is selected, selects all other checkboxes
document.addEventListener('DOMContentLoaded', function () {
    var selectAllCheckbox = document.getElementById('select-all');
    
    if (selectAllCheckbox) {
        selectAllCheckbox.addEventListener('click', function(e) {
            var found_checkboxes = document.getElementsByClassName('select-courses');
            var chartCheckboxes = document.getElementsByClassName('select-charts');
            
            for (var i = 0; i < found_checkboxes.length; i++) {
                found_checkboxes[i].checked = e.target.checked;
                chartCheckboxes[i].checked = e.target.checked;
            }
        });
    }
});


// Get all checkboxes with the class "select-courses"
document.addEventListener('DOMContentLoaded', function () {
  const selectCoursesCheckboxes = document.querySelectorAll('.select-courses');
  // For each "select-courses" checkbox, add a change event listener
  selectCoursesCheckboxes.forEach(function (checkbox) {
    checkbox.addEventListener('change', function () {
      // Get the parent row of the changed checkbox
      const parentRow = checkbox.closest('tr');
      // Get the "select-charts" checkbox in the same row
      const selectChartsCheckbox = parentRow.querySelector('.select-charts');
      // Set the "checked" property of the "select-charts" checkbox to match the "checked" property of the "select-courses" checkbox
      selectChartsCheckbox.checked = checkbox.checked;
    });
  });
});

// Found this usefull js function that generates a random hex color
// https://css-tricks.com/snippets/javascript/random-hex-color/
function getRandomColors(length) {
  var colors = [];
  for (var i = 0; i < length; i++) {
    var color = '#' + Math.floor(Math.random() * 16777215).toString(16);
    colors.push(color);
  }
  return colors;
}

  function checkSelection(event) {
    // Check if any checkboxes are selected
    var found_checkboxes = document.querySelectorAll('input[type="checkbox"]:checked');
    if (found_checkboxes.length === 0) {
      // Prevent form submission
      event.preventDefault();
      // Display error message
      alert("Please select at least one course.");
    }
  }
  