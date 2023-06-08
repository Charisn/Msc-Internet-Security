 loadJson("course.json", "courseschema.json")

  async function loadJson(jsonPath, schemaPath) {
    try {
      // Fetch the JSON data and the JSON schema
      const jsonDataPromise = fetch(jsonPath).then((response) => response.json())
      const schemaDataPromise = fetch(schemaPath).then((response) => response.json())

      const [jsonData, schemaData] = await Promise.all([jsonDataPromise, schemaDataPromise]);

      return validateJson(jsonData, schemaData);
    } catch(error){
      window.alert("There was an error while reading json file.");
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
    
    // JSON object is valid against the schema, call fillCourseData function
    fillCourseData(json);
    return true;
  }
  

// Create the course table with divs
function fillCourseData(jsonData) {

  const data = Object.values(jsonData);
  
  var elementName = document.querySelector('.name-data');
  var elementOverview = document.querySelector('.overview-data');
  var elementKeyFacts = document.querySelector('.key-facts-data');
  var elementHighlights = document.querySelector('.highlights-data');
  var elementContent = document.querySelector('.content-data');
  var elementImage = document.querySelector('.image-data')

  var divName = document.createElement('div');
  divName.innerHTML = '<div class="data-cell data-name"><p>' + data[1] + '</p></div>';
  elementName.appendChild(divName);
  divName.classList.add('course-data');

  var divOverview = document.createElement('div');
  divOverview.innerHTML = '<div class="data-cell data-overview">' + data[2] + '</div>';
  elementOverview.appendChild(divOverview);
  divOverview.classList.add('course-data');

  var divKeyFacts = document.createElement('div');
  divKeyFacts.innerHTML = '<div class="data-cell data-key-facts">' + data[3] + '</div>';
  elementKeyFacts.appendChild(divKeyFacts);
  divKeyFacts.classList.add('course-data');

  var divHighlights = document.createElement('div');
  divHighlights.innerHTML = '<div class="data-cell data-highlights">' + data[4] + '</div>';
  elementHighlights.appendChild(divHighlights);
  divHighlights.classList.add('course-data');

  var divCOntent = document.createElement('div');  
  for (var tab in data[5]) {
    if (data[5].hasOwnProperty(tab)) {
      console.log(data[5][tab]);
      divCOntent.innerHTML += data[5][tab];
    }
  }
  elementContent.appendChild(divCOntent);
  divCOntent.classList.add('course-data');

  var divImage = document.createElement('div');
  divImage.innerHTML = '<div class="data-cell data-image"><img src="' + data[6] + '"></div>';
  elementImage.appendChild(divImage);
  divImage.classList.add('course-data');

}