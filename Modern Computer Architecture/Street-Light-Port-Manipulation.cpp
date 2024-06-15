/*--**************************************************************************************-
-- Produced By: Modern Computer Architecture
-- URL: 		https://mypad.northampton.ac.uk/smartsc/
-- Author: 		Charalampos Nikolaidis
-- Date: 		{February-March}/2024
-- Purpose: Make a simple pedestrial crossroad utilizing traffic lights. ( rbg / led )
************************************************************************************* --*/ 

/*
  -- 
  General optimisation principles:
  1. String literals can consume lots of storage.
  2. Optimize variables. Remove unused variables and use long variables ( 4 bytes ) only if necessary.
  3. Declare variables as locally as possible.
  4. Avoid recursions.
  5. Avoid including large libraries.
  6. Store constant data in storage instead of memory with PROGMEM.
  7. Skip the bootloader.
  -- 
  SRAM (Static Random-Access Memory) is used for storing variables and data structures while an Arduino program is running.
*/

// ap, N. (n.d.). digitalWriteFast. GitHub. Available at: https://github.com/NicksonYap/digitalWriteFast [Accessed date: 06/03/2024].
// defined(__AVR_ATmega328__) is defined in the library so arduino uno R3 will work with this.
#include <digitalWriteFast.h>

/*
-- This library optimizes/replaces Arduino I/O operations via direct port manipulation, reducing execution time and resource usage.
-- The regular digitalWrite() in Arduino Uno core (16MHz) takes about 6280nS while digitalWriteFast() port manipulation takes 125nS.
-- It enhances energy efficiency minimizing CPU active cycles and RAM footprint.
-- Requires compile time constants for pin assignments that bypasses the Arduino layer resulting in reduction in exec times.
-- Its tailored for AVR microcontrollers, compatible with our atmega328 chipset.
*/

#include <LiquidCrystal_I2C.h>

// Defined pins this way so the library digitalWriteFast can utilize them properly as documented.
#define BUTTON_PIN 2
#define MOTION_SENSOR_PIN 3
#define RED_LIGHT_CAR_PIN 4
#define YELLOW_LIGHT_CAR_PIN 5
#define GREEN_LIGHT_CAR_PIN 6
#define RED_LIGHT_PED_PIN 7
#define BUZZER_PIN 9
#define RGB_RED_PIN 10
#define RGB_GREEN_PIN 11
#define CROSSWALK_LEDS 12

#define LCD_ADDRESS_0x20 0x20
#define LCD_ADDRESS_0x26 0x26

#define YELLOW_TO_RED_DELAY 2500
#define RED_TO_GREEN_DELAY 3000
#define PEDESTRIAN_CROSSING_TIME 8000
#define SPEEDUP_TIME 5000
#define MOTION_DETECTED_DELAY 5000

enum TrafficLightState {
  RED,
  RED_YELLOW,
  GREEN,
  YELLOW
};

/*
 * Due to a known issue with older Arduino IDE versions (in 1.7.x series from Arduino.org),
 * the compiler incorrectly places autogenerated function prototypes before custom type definitions,
 * causing build errors when enums are used as function arguments. This issue is discussed in detail
 * on the Arduino Stack Exchange (Majenko, 2016). The workaround involves manually declaring
 * function prototypes and custom types (enums, in this case) before their first use in the code.
 * This approach ensures compatibility across different versions of the Arduino IDE, including
 * Tinkercad's Arduino simulation environment, which may use an older build system similar to Arduino 1.7.x.
 * https://arduino.stackexchange.com/questions/28133/cant-use-enum-as-function-argument
 */

void setTrafficLightState(TrafficLightState state);
void updateTrafficLightState(unsigned long currentMillis);
void checkMotionSensor();
void setPedestrianRGB(bool isRed);
void updateAudibleVisualCues(unsigned long currentMillis);
void updateLCDStatus(unsigned long currentMillis);
String getStateText(TrafficLightState state);
unsigned long getRemainingTime(unsigned long currentMillis);
void handlePedestrianButtonPress();
void processPedestrianRequest(unsigned long currentMillis);

// Global usefull vars i will be using later.
volatile bool pedestrianRequest = false;
volatile unsigned long buttonPressTimestamp = 0;
bool motionDetected = false;
unsigned long lastDisplayedTime = 0;
bool isStaticTextInitialized = false;
TrafficLightState trafficLightState = GREEN;
unsigned long stateChangeTimestamp = 0;
unsigned long previousMillis = 0; // Stores the last time the display was updated
const long interval = 300; // Interval at which to refresh the display (300ms)
int position = 0; // Current position of the scrolling text
bool scrollingForward = true; // Direction of scroll

LiquidCrystal_I2C lcd_0x20(LCD_ADDRESS_0x20, 16, 2);
LiquidCrystal_I2C lcd_0x26(LCD_ADDRESS_0x26, 16, 2);

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Initializes pins, serial communication, and interrupts for system setup.
-- Called by:       Arduino runtime on start.
-- Modifications:   --
1. Added Pins as progressed.
2. Multiple changes to lcd.
3. Changed to pinModeFast.
************************************************************************************* --*/

void setup() {
  pinModeFast(BUTTON_PIN, INPUT_PULLUP);
  pinModeFast(MOTION_SENSOR_PIN, INPUT);
  pinModeFast(RED_LIGHT_CAR_PIN, OUTPUT);
  pinModeFast(YELLOW_LIGHT_CAR_PIN, OUTPUT);
  pinModeFast(GREEN_LIGHT_CAR_PIN, OUTPUT);
  pinModeFast(RED_LIGHT_PED_PIN, OUTPUT);
  pinModeFast(CROSSWALK_LEDS, OUTPUT);
  pinModeFast(BUZZER_PIN, OUTPUT);
  pinModeFast(RGB_RED_PIN, OUTPUT);
  pinModeFast(RGB_GREEN_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handlePedestrianButtonPress, FALLING);
  lcd_0x20.init();
  lcd_0x20.backlight();
  lcd_0x26.init();
  lcd_0x26.backlight();
  setTrafficLightState(GREEN);
  Serial.begin(9600);
  Serial.print("Free RAM: "); // in loop -4 bytes of ram
  Serial.println(freeRam());
}

// func to print the free ram
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Executes main control loop, handling sensor checks and state updates.
-- Called by:       Arduino runtime.
-- Modifications:   None
************************************************************************************* --*/

void loop() {
  unsigned long currentMillis = millis();
  checkMotionSensor();
  if (pedestrianRequest) {
    processPedestrianRequest(currentMillis);
  }
  updateTrafficLightState(currentMillis);
  updateAudibleVisualCues(currentMillis);
  updateLCDStatus(currentMillis);
}
/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Initiates pedestrian crossing request on button press.
-- Called by:       Button press interrupt.
-- Modifications:   None
************************************************************************************* --*/
void handlePedestrianButtonPress() {
  pedestrianRequest = true;
  buttonPressTimestamp = millis();
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Processes pedestrian button requests within traffic light logic.
-- Called by:       loop().
-- Modifications:   None
************************************************************************************* --*/
void processPedestrianRequest(unsigned long currentMillis) {
  if (trafficLightState == GREEN && pedestrianRequest) {
    if ((!motionDetected) || (motionDetected && currentMillis - buttonPressTimestamp >= MOTION_DETECTED_DELAY)) {
      setTrafficLightState(YELLOW);
      buttonPressTimestamp = currentMillis;
      pedestrianRequest = false; // Reset request to prevent re-entry
    }
  }
}
/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Sets the state of traffic lights and manages related device states.
-- Called by:       processPedestrianRequest(), updateTrafficLightState().
-- Modifications:   
1. Added logic to handle car light here instead of updateTrafficLightState();
************************************************************************************* --*/
void setTrafficLightState(TrafficLightState state) {
  trafficLightState = state;
  stateChangeTimestamp = millis();
  
  digitalWriteFast(RED_LIGHT_CAR_PIN, state == RED || state == RED_YELLOW ? HIGH : LOW);
  digitalWriteFast(YELLOW_LIGHT_CAR_PIN, state == YELLOW || state == RED_YELLOW ? HIGH : LOW);
  digitalWriteFast(GREEN_LIGHT_CAR_PIN, state == GREEN ? HIGH : LOW);
  
  // Manage pedestrian light directly within state transitions if needed
  if(state == RED) {
    delay(500); // Wait for 0.5 seconds
    setPedestrianRGB(false); // Turn pedestrian light green
  } else {
    setPedestrianRGB(true); // Otherwise, keep pedestrian light red
  }
  
  // Specific handling when transitioning to GREEN to clear the timer display
  if (state == GREEN) {
    lcd_0x20.setCursor(6, 1);
    lcd_0x20.print("       "); // Clear the timer display area
  }
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Updates traffic light state based on timing and current state.
-- Called by:       loop().
-- Modifications:   
1. Added seperate red yellow logic instead of handling it withing case red.
2. Added simple yellow logic to use it from green to red.
************************************************************************************* --*/

void updateTrafficLightState(unsigned long currentMillis) {
  // Extended logic to transition from YELLOW to RED for vehicles and then to make pedestrian light GREEN
  switch (trafficLightState) {
    case RED:
      updateCarDisplay(false);
      // After sufficient RED time, transition to RED_YELLOW before going to GREEN for cars (and RED for pedestrians)
      if (currentMillis - stateChangeTimestamp >= RED_TO_GREEN_DELAY) {
        setTrafficLightState(RED_YELLOW);
      }
      break;
    case RED_YELLOW:
      // Transition to GREEN for cars, which makes RED for pedestrians
      if (currentMillis - stateChangeTimestamp >= YELLOW_TO_RED_DELAY) {
        setTrafficLightState(GREEN);
      }
      break;
    case GREEN:
      updateCarDisplay(true);
      break;
    case YELLOW:
      // Transition from YELLOW to RED for cars, which will then trigger GREEN for pedestrians
      if (currentMillis - stateChangeTimestamp >= YELLOW_TO_RED_DELAY) {
        setTrafficLightState(RED);
      }
      break;
  }
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Checks motion sensor state and updates detection status.
-- Called by:       loop().
************************************************************************************* --*/
void checkMotionSensor() {
  motionDetected = digitalReadFast(MOTION_SENSOR_PIN) == HIGH;
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Controls pedestrian RGB LED based on the provided state.
-- Called by:       setTrafficLightState().
************************************************************************************* --*/
void setPedestrianRGB(bool isRed) {
  digitalWriteFast(RGB_RED_PIN, isRed ? HIGH : LOW);
  digitalWriteFast(RGB_GREEN_PIN, !isRed ? HIGH : LOW);
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Manages auditory and visual cues based on traffic light state and timing.
-- Called by:       loop().
-- Modifications:   
1. Added functionality for SPEEDUP_TIME
2. Added cross road lights
3. Fixes issue where cross road lights did not turn on for PEDESTRIAN_CROSSING_TIME
TODO: make crossroad lights flicker at speedUpElapsed.
************************************************************************************* --*/

void updateAudibleVisualCues(unsigned long currentMillis) {
  if (trafficLightState == RED || trafficLightState == RED_YELLOW) {
    unsigned long elapsedTimeSinceStateChange = currentMillis - stateChangeTimestamp;

    // Check if we are within the SPEEDUP_TIME of the pedestrian crossing period ending
    if (PEDESTRIAN_CROSSING_TIME - elapsedTimeSinceStateChange <= SPEEDUP_TIME) {
      // Calculate the proportion of the SPEEDUP_TIME that has elapsed to scale the tone frequency
      unsigned long speedUpElapsed = SPEEDUP_TIME - (PEDESTRIAN_CROSSING_TIME - elapsedTimeSinceStateChange);
      
      // Scale the frequency based on the elapsed time during the speed up period
      int frequency = 1000 + (speedUpElapsed * 1000 / SPEEDUP_TIME); // Increase frequency over time
      tone(BUZZER_PIN, frequency);

      // Flicker CROSSWALK_LEDS during speed up time
      if ((currentMillis / 250) % 2) { // control flicker speed
        digitalWriteFast(CROSSWALK_LEDS, HIGH);
      } else {
        digitalWriteFast(CROSSWALK_LEDS, LOW);
      }
    } else {
      tone(BUZZER_PIN, 1000); // Default tone frequency before SPEEDUP_TIME begins
      digitalWriteFast(CROSSWALK_LEDS, HIGH); // Keep CROSSWALK_LEDS on before the SPEEDUP_TIME starts
    }
  } else {
    noTone(BUZZER_PIN); // Turn off the tone in other states
    digitalWriteFast(CROSSWALK_LEDS, LOW); // Ensure CROSSWALK_LEDS are off when not in RED or RED_YELLOW state
  }
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Updates LCD displays with current traffic light state and timing information.
-- Called by:       loop().
-- Modifications:   
1. Fixed the refresh rate issue
2. Fixes issue where time when red -> green showed the maximum integer number.
3. Fixed issue where after the 2. fix numbers showed as 00s when in green, instead of clear display.
4. Changes to make it more energy efficient and not refresh when not needed. ( example: green for cars )
** 5. Failed attempts to make it print const char instead of string.**
** 6. Failed attempts to utilize PROGMEM. Maybe wrong buffer? ** --> see bottom
************************************************************************************* --*/

void updateLCDStatus(unsigned long currentMillis) {
  String stateText = getStateText(trafficLightState);
  unsigned long remainingTime = getRemainingTime(currentMillis) / 1000; // Convert to seconds

  if (!isStaticTextInitialized) {
    lcd_0x20.clear();
    lcd_0x20.print("State: " + stateText);
    isStaticTextInitialized = true;
  }

  // Only update the timer for states other than GREEN without a pedestrian request
  if (trafficLightState != GREEN || pedestrianRequest) {
    if (lastDisplayedTime != remainingTime) {
      lcd_0x20.setCursor(6, 1);
      if (remainingTime < 10) { // Padding for single digit numbers
        lcd_0x20.print("0"); // Padding
      }
      lcd_0x20.print(String(remainingTime + 1) + "s  "); // Update time
      lastDisplayedTime = remainingTime;
    }
  } else {
    // If the state is GREEN and no pedestrian request, clear the timer display area
    if (lastDisplayedTime != 0) {
      lcd_0x20.setCursor(6, 1);
      lcd_0x20.print("       "); // Clear the timer display area
      lastDisplayedTime = 0; // Reset last displayed time
    }
  }
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Returns textual representation of the current traffic light state.
-- Called by:       updateLCDStatus().
************************************************************************************* --*/
String getStateText(TrafficLightState state) {
  switch (state) {
    case RED: return "Red";
    case RED_YELLOW: return "Red+Yellow";
    case GREEN: return "Green";
    case YELLOW: return "Yellow";
    default: return "";
  }
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Calculates remaining time for the current traffic light phase.
-- Called by:       updateLCDStatus().
************************************************************************************* --*/
unsigned long getRemainingTime(unsigned long currentMillis) {
  switch (trafficLightState) {
    case RED:
    case GREEN:
      return PEDESTRIAN_CROSSING_TIME - (currentMillis - stateChangeTimestamp);
    case RED_YELLOW:
    case YELLOW:
      return YELLOW_TO_RED_DELAY - (currentMillis - stateChangeTimestamp);
    default:
      return 0;
  }
}
/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Updates LCD display for car state with scrolling text.
-- Called by:       updateTrafficLightState().
-- Modifications: 
1. Added interval refresh rate.
2. Added 2 commercial sentences, 1 for green and 1 for red car light state.
3. Added a horizontal scroll animation.
4. Fixed - added buffer to not stcroll endlessly.
** 5. Failed attempt to make characters and animations using bytes instead of simple words **
************************************************************************************* --*/

void updateCarDisplay(bool isGreen) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Define messages as char arrays
    const char greenMessage[] = "Commercial Green Car";
    const char redMessage[] = "Commercial Red Car";
    const char* message = isGreen ? greenMessage : redMessage;

    int messageLength = strlen(message); // Use strlen for char arrays
    int displayWidth = 16;
    int scrollRange = max(0, messageLength - displayWidth);

    lcd_0x26.clear();

    char displayBuffer[17]; // Plus one for null terminator
    memset(displayBuffer, ' ', sizeof(displayBuffer)); // Fill with spaces
    displayBuffer[16] = '\0'; // Ensure null-termination

    // Calculate start index based on current position and direction
    int startIndex = scrollingForward ? position : scrollRange - position;
    startIndex = max(0, min(startIndex, scrollRange));

    // Copy relevant part of message to displayBuffer
    for (int i = 0; i < displayWidth && (startIndex + i) < messageLength; i++) {
      displayBuffer[i] = message[startIndex + i];
    }

    // Print the displayBuffer
    lcd_0x26.print(displayBuffer);

    // Update position and change direction if needed
    if (scrollingForward && position >= scrollRange) {
      scrollingForward = false; // Change direction to scroll back
    } else if (!scrollingForward && position <= 0) {
      scrollingForward = true; // Change direction to scroll forward
    }

    // Update position for the next iteration
    position += scrollingForward ? 1 : -1;
  }
}

/* ------------------------------------------------ 
  This is a way to make lcd more efficient.
  It is incomplete because i could not retrieve the const char from program memory. 
  My final thoughts on what could it went wrong, is the buffer. Looping through it crashed the lcds.
------------------------------------------------ */

// #include <avr/pgmspace.h>
// Global scope for PROGMEM strings
// const char greenCarMsg[] PROGMEM = "Commercial Green Car";
// const char redCarMsg[] PROGMEM = "Commercial Red Car";

// void displayStaticEfficientFromProgmem() {
//   char buffer[25]; // Buffer for copying the message from PROGMEM
//   strcpy_P(buffer, (isGreen ? greenCarMsg : redCarMsg));
//   buffer[24] = '\0'; // Explicitly null-terminate

//   lcd_0x26.clear();
//   lcd_0x26.print(buffer); // Display the static message
// }

// void updateCarDisplay(bool isGreen) {
//   unsigned long currentMillis = millis();

//   if (currentMillis - previousMillis >= interval) {
//     previousMillis = currentMillis;
//     displayStaticTestFromProgmem();
//   }
// }