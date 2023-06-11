loadJson("course.json", "courseschema.json");
//  refreshData();

async function loadJson(jsonPath, schemaPath) {
  try {
    // Fetch the JSON data and the JSON schema
    const jsonDataPromise = fetch(jsonPath).then((response) => response.json())
    const schemaDataPromise = fetch(schemaPath).then((response) => response.json())

    const [jsonData, schemaData] = await Promise.all([jsonDataPromise, schemaDataPromise]);
    setCurrencyCookie('GBP');
    return validateJson(jsonData, schemaData);
  } catch (error) {
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
  if ("courses" in jsonData) {
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

function clearData() {
  var elements = document.querySelectorAll(".course-data");
  elements.forEach(function (element) {
    element.remove();
  });
}

function convertToCurrency(from_currency, to_currency) {
  getCurrencyRates(from_currency, to_currency).then((exchange_rate) => {

    const from_symbol = new Intl.NumberFormat('en', { style: 'currency', currency: from_currency })
      .format(0)
      .replace(/[\d.,]/g, '')
      .trim();

    const to_symbol = new Intl.NumberFormat('en', { style: 'currency', currency: to_currency })
      .format(0)
      .replace(/[\d.,]/g, '')
      .trim();

    var data_cell_div = document.getElementsByClassName('course-data');

    for (let i = 0; i < data_cell_div.length; i++) {
      var paragraphs = data_cell_div[i].getElementsByTagName('p');

      for (let j = 0; j < paragraphs.length; j++) {
        if (paragraphs[j].textContent.includes(from_symbol)) {
          let regex = new RegExp(`${from_symbol}\\s*([0-9,]+)`, 'g');
          let beforePrices = paragraphs[j].textContent.match(regex);

          if (beforePrices) {
            for (let k = 0; k < beforePrices.length; k++) {
              let beforePrice = parseFloat(beforePrices[k].replace(from_symbol, '').replace(',', ''));

              if (!isNaN(beforePrice)) {
                var afterPrice = beforePrice * exchange_rate;
                let newCurrencyNode = document.createTextNode(" " + to_symbol + afterPrice.toFixed(2) + " ");

                let beforeText = document.evaluate('.//text()[contains(., "' + beforePrices[k] + '")]', paragraphs[j], null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;

                if (beforeText) {
                  let textContent = beforeText.textContent;
                  let priceIndex = textContent.indexOf(beforePrices[k]);

                  // Split the original text around the old price
                  let textBefore = document.createTextNode(textContent.slice(0, priceIndex).trim());
                  let textAfter = document.createTextNode(textContent.slice(priceIndex + beforePrices[k].length).trim());

                  // Create a new fragment with the text before the price, the new price, and the text after the price
                  let newNodes = document.createDocumentFragment();
                  newNodes.appendChild(textBefore);
                  newNodes.appendChild(newCurrencyNode);
                  newNodes.appendChild(textAfter);

                  // Replace the old text node with the new fragment
                  beforeText.parentNode.replaceChild(newNodes, beforeText);
                }
              }
            }
          }
        }
      }
    }
  });
}





// I followed instuctions i found from the following api. The documentation is the following which i took their base layout and edit it to my needs.
// https://apilayer.com/marketplace/exchangerates_data-api?live_demo=show

async function getCurrencyRates(from_currency, to_currency) {

  try {
    const myHeaders = new Headers();
    myHeaders.append("apikey", "ongniIioNKlSWs6Pr29L7eY12yIuAHz6");

    const requestOptions = {
      method: 'GET',
      redirect: 'follow',
      headers: myHeaders
    };

    const current_date = getCurrentDateFormatted();
    const response = await fetch(`https://api.apilayer.com/exchangerates_data/fluctuation?start_date=${current_date}&end_date=${current_date}&base=${from_currency}&symbols=GBP,USD,EUR`, requestOptions);
    const result = await response.json();

    // set the cookie as the current new currency so the next time we change currency rates, we know what the current currency is to make the compare.
    setCurrencyCookie(to_currency);
    return result.rates[to_currency].start_rate;

  } catch (error) {
    console.error('Error:', error);
  }
}

// https://www.freecodecamp.org/news/javascript-date-now-how-to-get-the-current-date-in-javascript/
function getCurrentDateFormatted() {
  const date = new Date();

  const year = date.getFullYear();

  // getMonth() returns month index starting from 0 (January), so i add +1
  const month = (date.getMonth() + 1).toString().padStart(2, '0');

  const day = date.getDate().toString().padStart(2, '0');

  return `${year}-${month}-${day}`;
}

// This cookie code i took from this website. Author is me.
// https://kritharis.digital4u-eshops.gr/
function setCurrencyCookie(cookieValue) {
  // Set the cookie with an expiration time of 1 hour from now
  const expires = new Date(Date.now() + 60 * 30 * 1000).toUTCString();
  document.cookie = `currency=${JSON.stringify(cookieValue)}; expires=${expires}`;
}

// Read the cookie
function getCookie(name) {
  const cookies = document.cookie.split(';');
  for (let i = 0; i < cookies.length; i++) {
    const cookie = cookies[i].trim();
    if (cookie.startsWith(name + '=')) {
      const value = cookie.substring((name + '=').length);
      try {
        return JSON.parse(decodeURIComponent(value));
      } catch (e) {
        return decodeURIComponent(value);
      }
    }
  }
  return null;
}

// CODE FROM https://www.w3schools.com/howto/howto_js_scroll_to_top.asp
// Get the button:
let mybutton = document.getElementById("btnTop");

// When the user scrolls down 20px from the top of the document, show the button
window.onscroll = function() {scrollFunction()};

function scrollFunction() {
  if (document.body.scrollTop > 20 || document.documentElement.scrollTop > 20) {
    mybutton.style.display = "block";
  } else {
    mybutton.style.display = "none";
  }
}

// When the user clicks on the button, scroll to the top of the document
function topFunction() {
  document.body.scrollTop = 0; // For Safari
  document.documentElement.scrollTop = 0; // For Chrome, Firefox, IE and Opera
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