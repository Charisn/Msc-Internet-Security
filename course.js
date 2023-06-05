// Load the course.json file
fetch('course.json')
  .then(response => response.json())
  .then(data => validateCourseData(data))
  .catch(error => console.error('Error loading course.json:', error));

// Validate the course data against the schema
function validateCourseData(data) {
  // Define the JSON schema
  const schema = {
    $schema: 'http://json-schema.org/draft-04/schema#',
    type: 'object',
    properties: {
      courses: {
        type: 'array',
        items: {
          type: 'object',
          properties: {
            id: { type: 'string' },
            name: { type: 'string' },
            type: { type: 'string' },
            clevel: { type: 'string' },
            description: { type: 'string' },
            entry_requirements: { type: 'string' },
            start_date: { type: 'string' },
            duration: { type: 'number' },
            full_time: { type: 'boolean' },
            tuition_fees: {
              type: 'object',
              properties: {
                GBP: { type: 'number' },
                EUR: { type: 'number' },
                USD: { type: 'number' },
              },
              required: ['GBP', 'EUR', 'USD'],
            },
            image_url: { type: 'string' },
            modules: {
              type: 'object',
              properties: {
                'Year 1': {
                  type: 'object',
                  properties: {
                    mandatory: {
                      type: 'array',
                      items: { type: 'string' },
                    },
                    optional: {
                      type: 'array',
                      items: { type: 'string' },
                    },
                  },
                },
                'Year 2': {
                  type: 'object',
                  properties: {
                    mandatory: {
                      type: 'array',
                      items: { type: 'string' },
                    },
                    optional: {
                      type: 'array',
                      items: { type: 'string' },
                    },
                  },
                },
                'Year 3': {
                  type: 'object',
                  properties: {
                    mandatory: {
                      type: 'array',
                      items: { type: 'string' },
                    },
                    optional: {
                      type: 'array',
                      items: { type: 'string' },
                    },
                  },
                },
              },
            },
            funding_available: { type: 'boolean' },
          },
          required: [
            'id',
            'name',
            'type',
            'clevel',
            'description',
            'entry_requirements',
            'start_date',
            'duration',
            'full_time',
            'tuition_fees',
            'image_url',
            'modules',
            'funding_available',
          ],
        },
      },
    },
    required: ['courses'],
  };

  // Validate the data against the schema
  const valid = validateWithSchema(data, schema);

  // if (!valid) {
  //   console.error('Invalid course.json:', validateWithSchema.errors);
  //   return;
  // }

  // Data is valid, create the course table
  createCourseTable(data);
}

// Validate data against a JSON schema
function validateWithSchema(data, schema) {
  try {
    const jsonData = JSON.stringify(data);
    const jsonSchema = JSON.stringify(schema);
    const parsedData = JSON.parse(jsonData);
    const parsedSchema = JSON.parse(jsonSchema);
    return JSONValidator(parsedData, parsedSchema);
  } catch (error) {
    console.error('Error during JSON schema validation:', error);
    return false;
  }
}

// Create the course table with divs
function createCourseTable(data) {
  // Get the course table container element
  const courseTableContainer = document.querySelector('#course-table');

  // Create the table container div
  const tableContainer = document.createElement('div');
  tableContainer.classList.add('table-container');

  // Create the table header row div
  const headerRow = document.createElement('div');
  headerRow.classList.add('header-row');

  // Create the table header cells
  const headerCells = Object.keys(data.courses[0]);
  headerCells.forEach(headerCell => {
    const headerCellDiv = document.createElement('div');
    headerCellDiv.classList.add('header-cell');
    headerCellDiv.textContent = headerCell;
    headerRow.appendChild(headerCellDiv);
  });

  // Add the header row to the table container
  tableContainer.appendChild(headerRow);

  // Iterate over each course and add a row to the table
  data.courses.forEach(course => {
    const courseRow = document.createElement('div');
    courseRow.classList.add('course-row');

    Object.values(course).forEach(courseValue => {
      const courseCell = document.createElement('div');
      courseCell.classList.add('course-cell');

      // Check if the course value is HTML data
      if (typeof courseValue === 'string' && courseValue.startsWith('<')) {
        courseCell.innerHTML = courseValue; // Render as HTML
      } else {
        courseCell.textContent = courseValue; // Render as text
      }

      courseRow.appendChild(courseCell);
    });

    tableContainer.appendChild(courseRow);
  });

  // Add the table container to the course table container
  courseTableContainer.appendChild(tableContainer);
}

