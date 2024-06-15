// As soon as the html loads, it loads the javascript
loadJson("course.json", "courseschema.json"); // When this js file is being loaded the first this it is being done is to call this function instantly.
// This is being done to satisfy the request of the first load, data should show without any delay.
//  refreshData(); // I also call this function to start the counter to refresh the tables data every 4 seconds. 

// This function loads 2 paths of a json and a json schema and fetches it's contents.
async function loadJson(jsonPath, schemaPath) {
  try { // try catch in case of error
    // Fetch the JSON data and the JSON schema
    const jsonDataPromise = fetch(jsonPath).then((response) => response.json()) // fetch the json and when fetched make it a json objet.
    const schemaDataPromise = fetch(schemaPath).then((response) => response.json()) // fetch the jsonSchema and when fetched make it a json objet.

    // calling the promise.all static method to because jsonDataPromise, schemaDataPromise are fetch requests which return promises
    // by using promise.all i create a new promise that awais both requests to complete.
    // await pauses exection until the promise.all is resolved.
    //  This is usefull since i cannot continue with return validating 2 promises and will end up in an error.
    const [jsonData, schemaData] = await Promise.all([jsonDataPromise, schemaDataPromise]);

    // i could also make a new function instead of re-setting the cookie to default every timg, that reads the current cookie and delivers content with 
    // the previously selected currency, or read the location of the user to deliver him instantly his preffered currency but i decided to keep it simple. 
    setCurrencyCookie('GBP'); // the first load contains gbp currency, so i am setting a cookie with value gbp and use it accordingly in currency exchange.
    return validateJson(jsonData, schemaData);  // function returns a call of an other function with parameters 2 json objects
  } catch (error) {
    window.alert("There was an error while reading json file.");
    console.log(error);
    return false;
  }
}

// While everybody is using AJV library to do the validation i tried a new way to make the validation with the help of stackoverflow and chatgpt
// This is a function that makes the validation of json into json schema. 
function validateJson(json, schema) { // take json and jsonShema as parameters to compare.

  // loop over the entire json object
  for (let key in json) { // key is the values of the json file ex: id, name, overview etc.
    if (!schema.properties.hasOwnProperty(key)) { // checks whether the object schema.properties does not have a property with the name specified by the variable key.
      return false;
    }

    if (Array.isArray(json[key])) { // Check if the value associated with the current key in the json is an array
      for (let i = 0; i < json[key].length; i++) { // Loop through array

         // call validateJson to validate each element in the array
        // Pass the current array element as the json parameter and schema for that array as the schema parameter
        if (!validateJson(json[key][i], schema.properties[key].items)) {
          return false;  // If the loop completes without returning false, it means all properties in the JSON object were validated successfully.
        }
      }
    }
  }
  // and if until this point it does not satisfy any of the above ifs, means that it is validated and we can now use that json 'safely'.
  // JSON object is valid against the schema, call fillCourseData function
  fillCourseData(json);
  return true;
}
// This functions is being used to insert data from json into the html.
function fillCourseData(jsonData) {

  // Last loop returned the whole json object so i am running a necessary check to skip that last function call.
  if ("courses" in jsonData) {
    return;
  }
  const data = Object.values(jsonData); // this line could be skipped -- object.values returns an array of an object. This makes later steps easier.

  // using let instead of var because of efficiency, variable visibility and lifetime
  // In my html i have created 6 divs with certain classes. Those classes are used to identify where the data will be inserted.
  let elementName = document.querySelector('.name-data'); // selecting div with class name-data. All jsons data[1] (name) will be inserted there.
  let elementOverview = document.querySelector('.overview-data'); // selecting div with class overview-data. All jsons data[2] (name) will be inserted there.
  let elementKeyFacts = document.querySelector('.key-facts-data'); // repeating the same idea as above
  let elementHighlights = document.querySelector('.highlights-data'); // repeating the same idea as above
  let elementContent = document.querySelector('.content-data'); // repeating the same idea as above
  let elementImage = document.querySelector('.image-data') // repeating the same idea as above

  // Creating a new div each time, so every time the function is called, data will be inserted into a new div instead of overwriting the previous data.
  let divName = document.createElement('div'); // creating the div
  divName.innerHTML = '<div class="data-cell data-name"><p>' + data[1] + '</p></div>'; // inserting the html i want into that new div
  elementName.appendChild(divName); // appending that new div + html into the selected divs i specified above.
  divName.classList.add('course-data'); // giving this div a specific class so it will be more manageable later on.

  // repeating the same procedure 
  let divOverview = document.createElement('div');
  divOverview.innerHTML = '<div class="data-cell data-overview">' + data[2] + '</div>';
  elementOverview.appendChild(divOverview);
  divOverview.classList.add('course-data');

  // repeating the same procedure 
  let divKeyFacts = document.createElement('div');
  divKeyFacts.innerHTML = '<div class="data-cell data-key-facts">' + data[3] + '</div>';
  elementKeyFacts.appendChild(divKeyFacts);
  divKeyFacts.classList.add('course-data');

  // repeating the same procedure 
  let divHighlights = document.createElement('div');
  divHighlights.innerHTML = '<div class="data-cell data-highlights">' + data[4] + '</div>';
  elementHighlights.appendChild(divHighlights);
  divHighlights.classList.add('course-data');

  // since on my schema and json, tab_content is an object, there can be multiple values into it. So i have to treat it differently from a simple string.
  let divContent = document.createElement('div'); // Again creating a new div 
  for (let tab in data[5]) { // This is a for loop that loops through the entirety of tab_content. If i have 5 tabs in it, it will loop 4 times.
    if (data[5][tab]) { // Here i am checking if the value of tab_content exists and is not empty.
      divContent.innerHTML += data[5][tab]; // I insert the strings / values of tab_content inside the created div
      // operator += is used to not replace the previous data, just add the new one to the div.
    }
  }
  elementContent.appendChild(divContent); // inserting the newly created div into the selected div
  divContent.classList.add('course-data'); // add the class to it for later ease of use.

  // repeating the same procedure with the previous data
  let divImage = document.createElement('div');
  divImage.innerHTML = '<div class="data-cell data-image"><img src="' + data[6] + '"></div>';
  elementImage.appendChild(divImage);
  divImage.classList.add('course-data');

  document.body.classList.remove("loading");
}

let lastModified = null; // set last modified with a null value so i can later set it to current modified date for comparison.
refreshData(); // call the function

// Once this function is called it will be on an infinite loop of calling it self every 4 seconds
// And every time it is being called, it clears previous data in tables and re creates the table with new data.
async function refreshData() {
  try { // try catch any errors
    const response = await fetch("course.json"); // get the file
    // https://developer.mozilla.org/en-US/docs/Web/API/Headers
    const newLastModified = response.headers.get("Last-Modified"); // request header last-modified
    // If the file has been modified since the last fetch
    if (lastModified != newLastModified) { // the first time we always refresh once to set lastModified and then we have a comparison
      lastModified = newLastModified; // if it is modified we need to update the variable we are storing the date
      clearData(); // this is a function that is declared below, that clears all data before re-inserts them
      await loadJson("course.json", "courseschema.json");
    }
  } catch (error) {
    console.error('An error occurred:', error);
  }
  setTimeout(refreshData, 4000);
}

// This is a functions that clears my table's data. If called only heading titles will appear. Table's data will be cleared.
function clearData() {
  let tableData = document.querySelectorAll(".course-data"); // find all divs there are in dom and inserts it into a variable
  tableData.forEach(function (element) { // since tableData is an array of divs, i create a loop to remove all elements found in my selector above
    element.remove(); // this line calls the remove() method javascript gives us as a tool. This line removes the element.
  });
}

// This function scans the html inserted from fillCourseData() and replaces old prices and symbols to new price and symbol
// current currency is retreived from the cookie.
// convert_currency is retrieved from select option value.
function convertToCurrency(current_currency, convert_currency) {
  getCurrencyRates(current_currency, convert_currency).then((exchange_rate) => {
    // I used NumberFormat object to create a currency symbol for the current currency. I could also create a function with an array doing the same thing
    // calling it with USD parameter and returning $, but this way is a more professional and covers all possible combinations, not just the 3 that were asked
    const current_symbol = new Intl.NumberFormat('en', { style: 'currency', currency: current_currency }).format(0).replace(/[\d.,]/g, '').trim();
    const to_symbol = new Intl.NumberFormat('en', { style: 'currency', currency: convert_currency }).format(0).replace(/[\d.,]/g, '').trim();
    
    // Grab all the elements with the class name 'course-data'.
    var data_cell_div = document.getElementsByClassName('course-data');

    for (let i = 0; i < data_cell_div.length; i++) {  //loop through each data cell.
      var paragraphs = data_cell_div[i].getElementsByTagName('p'); //  get all p elements found
      var listItems = data_cell_div[i].getElementsByTagName('li');  // get all li elements found

      const contentElements = [...paragraphs, ...listItems];  // combining all 'p' and 'li' elements

      for (let j = 0; j < contentElements.length; j++) { // Start of a loop through each content element.
        if (contentElements[j].textContent.includes(current_symbol)) { // If the content of the current element includes the current symbol
          // escaping dollar symbol since it is used as a string literal in regex and with, it is being escaped
          // This is being done because after selecting USD as current currency i could not find with current regex the prices.
          let escapedSymbol = current_symbol === '$' ? '\\$' : current_symbol;
          // i use regex to find the prices that start with the current symbol ending in a number
          let regex = new RegExp(`${escapedSymbol}\\s*([0-9,]+)`, 'g');
          let beforePrices = contentElements[j].textContent.match(regex); // collect all the prices that match the regex.

          if (beforePrices) { // If there are any prices
            for (let k = 0; k < beforePrices.length; k++) {  // Start of a loop through each price.
              // This line removes the current_symbol and any commas from the price, then converts it to a number.
              let beforePrice = parseFloat(beforePrices[k].replace(current_symbol, '').replace(',', ''));
              if (!isNaN(beforePrice)) { // If the price is a valid number
                // Calculates converted price by multiplying the current price by the exchange rate.
                var afterPrice = beforePrice * exchange_rate; 

                // This line creates a new text node with the new currency symbol and the converted price.
                // Find the exact text node that contains the current price.
                let beforeText = document.evaluate // evaluate of an XPath expression string and return something
                ('.//text()[contains(., "' + beforePrices[k] + '")]', // the expression for the evaluate -> check if the value of node contains the price
                contentElements[j], // xpath expression will be compared to this 
                null, // name space resolver - i dont have a namespace
                XPathResult.FIRST_ORDERED_NODE_TYPE, // return the first node
                null // optional arguement no result object is provided, so a new one is created
                ).singleNodeValue; // property that returns a single note

                if (beforeText) { // If the text node was found
                  let textContent = beforeText.textContent; // Get the content of the text node.
                  let priceIndex = textContent.indexOf(beforePrices[k]); //Find the index at which the price starts in the text content.
                  // Build the new text content by taking everything before the price, adding the new currency and price, and then everything after the price.
                  let newTextContent = 
                  textContent.slice(0, priceIndex).trim() 
                  + " " 
                  + to_symbol 
                  + Math.round(afterPrice) 
                  + " " 
                  + textContent.slice(priceIndex + beforePrices[k].length).trim();
                  // Replace the old text content with the new text content.
                  beforeText.textContent = newTextContent;
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
async function getCurrencyRates(currency_before, cuyrrency_after) {

  try { // try catch for error logging.
    const apiHeaders = new Headers();
    apiHeaders.append("apikey", "ongniIioNKlSWs6Pr29L7eY12yIuAHz6"); // send api key in header 180/250 requests already done with this key.

    const requestOptions = {
      method: 'GET',
      redirect: 'follow',
      headers: apiHeaders
    };
    // current date calls a function declared below that returns a string of the current date with the format documentation wanted.
    const current_date = getCurrentDateFormatted();

    // request into the url below, with request options and dynamic settings set during the function call. 
    const response = await fetch(`https://api.apilayer.com/exchangerates_data/fluctuation?start_date=${current_date}&end_date=${current_date}&base=${currency_before}&symbols=GBP,USD,EUR`, requestOptions);
    const currency_result = await response.json();

    // set the cookie as the current new currency so the next time we change currency rates, we know what the current currency is to make the compare.
    setCurrencyCookie(cuyrrency_after);

    // return the rate
    return currency_result.rates[cuyrrency_after].start_rate;

  } catch (error) {
    console.error('Error:', error);
  }
}

// the following link inspired me with the end result of the function.
// https://www.freecodecamp.org/news/javascript-date-now-how-to-get-the-current-date-in-javascript/
function getCurrentDateFormatted() {
  const current_date = new Date(); // create a date object 

  const current_year = current_date.getFullYear(); // get the current full year 

  // getMonth() returns month index starting from 0 (January), so i add +1
  const current_month = (current_date.getMonth() + 1).toString().padStart(2, '0');
  // gets date object, makes it a string and padStart converts numbers from 2 to 02 given the arguements below.
  const current_day = current_date.getDate().toString().padStart(2, '0');

  return `${current_year}-${current_month}-${current_day}`;
}

// This cookie code i took from this website. Author is me.
// https://kritharis.digital4u-eshops.gr/
function setCurrencyCookie(cookieValue) {
  // Set the cookie with an expiration time of 1 hour from now
  const expires = new Date(Date.now() + 60 * 30 * 1000).toUTCString();
  document.cookie = `currency=${JSON.stringify(cookieValue)}; expires=${expires}`;
}

// CODE FROM https://www.w3schools.com/howto/howto_js_scroll_to_top.asp
let btnTop = document.getElementById("btnTop");
// When scrolling from px 20px and below, show the button
window.onscroll = function () {
  showButton()
};

function showButton() {
  if (document.body.scrollTop > 20
    || document.documentElement.scrollTop > 20) {
    btnTop.style.display = "block";
  } else {
    btnTop.style.display = "none";
  }
}

// When the user clicks on the button, scroll to the top of the document
function topFunction() {
  document.documentElement.scrollTop = 0;
}

// This cookie code i took from this website.
// https://kritharis.digital4u-eshops.gr/
// Author is me in march of 2023.
// function that reads a cookie given its name
function getCookie(name) {
  // split method separates the cookie string at each semicolon (';').
  const cookies = document.cookie.split(';');
  // A for loop that goes through each cookie in cookies array
  for (let i = 0; i < cookies.length; i++) {    
    // This line trims leading and trailing whitespace from the current cookie (cookies[i])
    const cookie = cookies[i].trim();
    // 'startsWith' checks if the current cookie string begins with the 'name' parameter followed by an equals sign ('=')
    // If it does, it means we've found the cookie we're looking for
    if (cookie.startsWith(name + '=')) {
      // This line gets the value of the found cookie.
      // It does this by creating a substring of 'cookie' that starts from the end of the 'name' parameter, effectively cutting out the name of the cookie.
      const value = cookie.substring((name + '=').length);      
      try {
        // This line tries to parse the value as JSON and decode it from URI format.
        // If successful, it returns the parsed and decoded value.
        // This would be useful if the cookie value is a complex data type that's been JSON.stringified and then URI encoded
        return JSON.parse(decodeURIComponent(value));
      } catch (e) {
        // If parsing as JSON and decoding fails, it decodes the value from URI format and then returns it.
        // This would be the case if the cookie value is a simple data type like a string or number
        return decodeURIComponent(value);
      }
    }
  }

  // If the 'name' parameter doesn't match any cookie names in the document, the function returns null
  return null;
}
// -------------------------------------------------------------------------------
// TODO 1: fix same site cookie - MAYBE LAST/NOT
// TODO 2: add 4 more courses - OK
// TODO 3: create multi currency logic. ( idea: track pound and numbers and replace them with js ) - OK
// TODO 4: make header more pleasant- OK { COULD ADD TEXT ON LEFT }
// TODO 5: maybe some more paddings - OK
// TODO 6: Add comments everywhere - OK
// TODO 7: Add references - OK
// TODO 8: Make video and word
// TODO 9: Write all the restructures i have made in the code into WORD. + Difficulties.
// TODO: 10: Try to make $ into html encode before - OK