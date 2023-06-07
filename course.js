// Load the course.json file
// fetch('course.json')
//   .then(response => response.json())
//   .then(data => validateJson(data))
//   .catch(error => console.error('Error loading course.json:', error));

 loadJson("course.json", "courseschema.json")

  async function loadJson(jsonPath, schemaPath) {
    try {
      // Fetch the JSON data and the JSON schema concurrently
      const [jsonData, schemaData] = await Promise.all([
        fetch(jsonPath).then((response) => response.json()),
        fetch(schemaPath).then((response) => response.json())
      ]);
  
      return validateJson(jsonData, schemaData);
    } catch(error){
      window.alert("Something went wrong.");
      console.log(error);
      return false;
    }
  }
  
  function validateJson(json, schema) {
    for (let key in json) {
      if (!schema.properties.hasOwnProperty(key)) {
        return false;
      }
      if (Array.isArray(json[key])) {
        for (let i = 0; i < json[key].length; i++) {
          if (!validateJson(json[key][i], schema.properties[key].items)) {
            return false;
          }
        }
      }
    }
    return createCourseTable(json);
}

// Create the course table with divs
function createCourseTable(jsonData) {

var tableData = '';
const data = Object.values(jsonData);
    tableData += '<div class="data-row">';
    tableData += '<div class="data-cell">' + data[1] + '</div>'; // Name
    tableData += '<div class="data-cell">' + data[2] + '</div>'; // Overview
    tableData += '<div class="data-cell">' + data[3] + '</div>'; // Key Facts
    tableData += '<div class="data-cell">' + data[4] + '</div>'; // Highlights
    tableData += '<div class="data-cell">' + data[5] + '</div>'; // Content
    tableData += '<div class="data-cell">' + data[6] + '</div>'; // Image URL
    tableData += '</div>';
debugger
  var dataContainer = document.querySelector('.data');
  dataContainer.innerHTML = tableData;

  // Append the SVG elements to the respective data cells
  var svgElements = document.querySelectorAll('.data-cell svg');
  svgElements.forEach(function(svgElement) {
    var cell = svgElement.closest('.data-cell');
    cell.appendChild(svgElement);
  });
  
}