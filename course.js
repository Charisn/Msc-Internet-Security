 loadJson("course.json", "courseschema.json");
//  refreshData();
 
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
  
  // While everybody is using ajv library to do the validation i tried a new way to make the validation with the help of stackoverflow and chatgpt to follow
  // the concept i originally had in my mind of vanila javascript result.
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
  // Last loop was the whole json object so i am checking and skipping it
  if ("courses" in jsonData){
    return;
  }
  const data = Object.values(jsonData);
  // using let instead of var because of efficiency, variable visibility and lifetime

  let elementName = document.querySelector('.name-data');
  let elementOverview = document.querySelector('.overview-data');
  let elementKeyFacts = document.querySelector('.key-facts-data');
  let elementHighlights = document.querySelector('.highlights-data');
  let elementContent = document.querySelector('.content-data');
  let elementImage = document.querySelector('.image-data')

  let divName = document.createElement('div');
  divName.innerHTML = '<div class="data-cell data-name"><p>' + data[1] + '</p></div>';
  elementName.appendChild(divName);
  divName.classList.add('course-data');

  let divOverview = document.createElement('div');
  divOverview.innerHTML = '<div class="data-cell data-overview">' + data[2] + '</div>';
  elementOverview.appendChild(divOverview);
  divOverview.classList.add('course-data');

  let divKeyFacts = document.createElement('div');
  divKeyFacts.innerHTML = '<div class="data-cell data-key-facts">' + data[3] + '</div>';
  elementKeyFacts.appendChild(divKeyFacts);
  divKeyFacts.classList.add('course-data');

  let divHighlights = document.createElement('div');
  divHighlights.innerHTML = '<div class="data-cell data-highlights">' + data[4] + '</div>';
  elementHighlights.appendChild(divHighlights);
  divHighlights.classList.add('course-data');

  let divCOntent = document.createElement('div');  
  for (let tab in data[5]) {
    if (data[5].hasOwnProperty(tab)) {
      divCOntent.innerHTML += data[5][tab];
    }
  }
  elementContent.appendChild(divCOntent);
  divCOntent.classList.add('course-data');

  let divImage = document.createElement('div');
  divImage.innerHTML = '<div class="data-cell data-image"><img src="' + data[6] + '"></div>';
  elementImage.appendChild(divImage);
  divImage.classList.add('course-data');

}

async function refreshData() {
  try {
    clearData();
    await loadJson("course.json", "courseschema.json"); // Call the async function and wait for it to complete
  } catch (error) {
    console.error('An error occurred:', error);
  }
  setTimeout(refreshData, 4000); // Then schedule the next call in 4 seconds
}

function clearData(){
  var elements = document.querySelectorAll(".course-data");
  elements.forEach(function(element) {
    element.remove();
  });

}

// -------------------------------------------------------------------------------
// TODO 1: fix same site cookie
// TODO 2: add 4 more courses
// TODO 3: create multi currency logic. ( idea: track pound and numbers and replace them with js )
// TODO 4: make header more pleasant
// TODO 5: maybe some more paddings
// TODO 6: Add comments everywhere
// TODO 7: Add references
// TODO 8: Make video and word