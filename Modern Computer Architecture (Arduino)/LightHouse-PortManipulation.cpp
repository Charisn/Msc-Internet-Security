/*--**************************************************************************************-
-- Produced By: Modern Computer Architecture
-- URL: 		https://mypad.northampton.ac.uk/smartsc/
-- Author: 		Charalampos Nikolaidis
-- Date: 		{February-March}/2024
-- Purpose: Make an efficient light system
************************************************************************************* --*/ 

// ap, N. (n.d.). digitalWriteFast. GitHub. Available at: https://github.com/NicksonYap/digitalWriteFast [Accessed date: 06/03/2024].
// defined(__AVR_ATmega328__) is defined in the library so arduino uno R3 will work with this.
#include <digitalWriteFast.h>

/*
-- This library optimizes/replaces Arduino I/O operations via direct port manipulation, reducing execution time and resource usage.
-- The regular digitalWrite() in Arduino Uno core (16MHz) takes about 6280nS while digitalWriteFast() port manipulation takes 125nS.
-- It enhances energy efficiency minimizing CPU active cycles and RAM footprint.
-- Requires compile time constants for pin assignments that bypasses the Arduino layer resulting in reduction in exec times.
-- its tailored for AVR microcontrollers, compatible with our atmega328 chipset.
*/

#define RGB_GREEN_PIN 9
#define RGB_BLUE_PIN 10
#define RGB_RED_PIN 11
#define ON_OFF_BUTTON_PIN 2
#define CONTINUOUS_MODE_BUTTON_PIN 5
#define TREE1_MODE_CHANGE_BUTTON_PIN 6
#define TREE2_MODE_CHANGE_BUTTON_PIN 6
#define RGB_MODE_CHANGE_BUTTON_PIN 4
#define MOTION_SENSOR_PIN 12
#define LIGHT_SENSOR_PIN A0
#define DURATION_POTENTIOMETER_PIN A2
#define DAYLIGHT_THRESHOLD_PIN A3
#define ULTRASONIC_TRIG_PIN 8
#define ULTRASONIC_ECHO_PIN 13
#define NUM_MODES 5 // only for rgb including off

// LEDs for trees are connected to these pins through NMOS transistors
#define TREE1_LED_PIN 7
#define TREE2_LED_PIN 3
#define UTILITY_DELAY 50 // milliseconds ( utility )

unsigned long lastDebounceTime1 = 0; // for TREE1_MODE_CHANGE_BUTTON_PIN
unsigned long lastDebounceTime2 = 0; // for TREE2_MODE_CHANGE_BUTTON_PIN
unsigned long lastDebounceTime3 = 0; // for RGB_MODE_CHANGE_BUTTON_PIN
volatile bool systemOn = false;
bool continuousMode = false;
bool firstActivation = true;
int tree1Mode = 0; // Example modes: 0-normal, 1-breathing, 2-random etc
int tree2Mode = 0;
bool motionDetected = false;
bool initialActivationDone = false;
int rgbMode = 0;

int colorIndex = 0; // For jump mode
int gradualStep = 0; // For gradual and smooth mode

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Initializes pins, serial communication, and interrupts for system setup.
-- Called by:       Arduino runtime on start.
-- Modifications:   --
1. Added Pins as progressed.
2. Changed to pinModeFast.
************************************************************************************* --*/

void setup() {
  // setupPWMRegistryManipulation(); commented out --> explanation at the end.
  pinModeFast(RGB_GREEN_PIN, OUTPUT);
  pinModeFast(RGB_BLUE_PIN, OUTPUT);
  pinModeFast(RGB_RED_PIN, OUTPUT);
  pinModeFast(ON_OFF_BUTTON_PIN, INPUT_PULLUP);
  pinModeFast(CONTINUOUS_MODE_BUTTON_PIN, INPUT_PULLUP);
  pinModeFast(TREE1_MODE_CHANGE_BUTTON_PIN, INPUT_PULLUP);
  pinModeFast(TREE2_MODE_CHANGE_BUTTON_PIN, INPUT_PULLUP);
  pinModeFast(RGB_MODE_CHANGE_BUTTON_PIN, INPUT_PULLUP);
  pinModeFast(MOTION_SENSOR_PIN, INPUT);
  pinModeFast(LIGHT_SENSOR_PIN, INPUT);
  pinModeFast(DURATION_POTENTIOMETER_PIN, INPUT);
  pinModeFast(DAYLIGHT_THRESHOLD_PIN, INPUT);
  pinModeFast(TREE1_LED_PIN, OUTPUT);
  pinModeFast(TREE2_LED_PIN, OUTPUT);
  pinModeFast(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinModeFast(ULTRASONIC_ECHO_PIN, INPUT);

  // placing the attachInterrupt call
  // attachInterrupt(digitalPinToInterrupt(ON_OFF_BUTTON_PIN), ISR_toggleSystemState, CHANGE);
  
  EICRA |= (1 << ISC00); // Set INT0 to trigger on any logic change
  EIMSK |= (1 << INT0);  // Enable INT0

  Serial.begin(9600);
  Serial.print("Free RAM: ");
  Serial.println(freeRam());
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Manages system logic, including light and mode control based on sensor input and button states.
-- Called by:       Arduino runtime in main loop.
-- Modifications:   --
1. Added inputs and sensors as i progressed
2. Implement non-blocking debounce(utility) logic to improve system responsiveness and reduce delay times.
3. Integrate a light intensity control feature, allowing adjustment based on ambient light conditions or user inputs.
4. Changed to digitalWriteFast.
************************************************************************************* --*/
void loop() {
  static unsigned long lastDebounceTimeRGB = 0;
  static int lastButtonStateRGB = HIGH;
  static unsigned long lastMotionTime = 0;
  unsigned long currentMillis = millis();
  const long DISTANCE_THRESHOLD = 100;
  long distance = readUltrasonicDistance();
  int currentButtonStateRGB = digitalReadFast(RGB_MODE_CHANGE_BUTTON_PIN);
  int currentButtonStateContinuous = digitalReadFast(CONTINUOUS_MODE_BUTTON_PIN);
  static unsigned long lastDebounceTimeContinuous = 0;
  static int lastButtonStateContinuous = HIGH;
  
  // Read the light level and determine if it's night time
  int lightLevel = readPotentiometer(LIGHT_SENSOR_PIN);
  int daylightThreshold = readPotentiometer(DAYLIGHT_THRESHOLD_PIN);

  // Read the potentiometer and map the value to the range of 3 to 20 seconds
  int duration = readPotentiometer(DURATION_POTENTIOMETER_PIN);
  long minDuration = map(duration, 0, 1023, 3000, 20000); // Converts to milliseconds (3-20 seconds)

  handleModeChange();
  // Debounce the button press for the continuous mode toggle
  if (currentMillis - lastDebounceTimeContinuous > UTILITY_DELAY) {
    if (currentButtonStateContinuous != lastButtonStateContinuous) {
      lastDebounceTimeContinuous = currentMillis; // Reset debounce timer

      if (currentButtonStateContinuous == LOW) { // Button pressed
        continuousMode = !continuousMode; // Toggle continuous mode
      }
    }
  }
  lastButtonStateContinuous = currentButtonStateContinuous; // Update button state for next loop iteration

  if (systemOn && lightLevel <= daylightThreshold) {
    if (continuousMode){
        controlTreeLights(TREE1_LED_PIN, tree1Mode);
        controlTreeLights(TREE2_LED_PIN, tree2Mode);
        changeRGBMode(rgbMode); // Change the RGB mode
    } else {
      // Detect motion
      if (digitalReadFast(MOTION_SENSOR_PIN) == HIGH && distance <= DISTANCE_THRESHOLD) {
        if (!motionDetected) { // If it's a new motion, update the lastMotionTime
          lastMotionTime = currentMillis;
          motionDetected = true;
        }
      }      
      // Keep the lights on if motion was detected and we are within the min duration
      if (motionDetected) {
        if (firstActivation) {
            minDuration = firstActivation ? random(5000, 7001) : map(duration, 0, 1023, 3000, 20000);
        }
        if (currentMillis - lastMotionTime <= minDuration) {
          // Only control lights if we're within the min duration period
          controlTreeLights(TREE1_LED_PIN, tree1Mode);
          controlTreeLights(TREE2_LED_PIN, tree2Mode);
          changeRGBMode(rgbMode); // Change the RGB mode
        } else {
          if (firstActivation) {
            firstActivation = false;
          }
          // Once min duration has elapsed, turn off the lights and reset motionDetected flag
          digitalWriteFast(TREE1_LED_PIN, LOW);
          digitalWriteFast(TREE2_LED_PIN, LOW);
          setColor(0, 0, 0);
          motionDetected = false; // Reset motion detected flag for the next motion event
        }
      }
    }
  } else {
    // Ensure lights are off during the day ( when light sensor is above threshold )
    digitalWriteFast(TREE1_LED_PIN, LOW);
    digitalWriteFast(TREE2_LED_PIN, LOW);
    setColor(0, 0, 0);
  }
}
/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Changes the global state of the system, turning off all controlled outputs when deactivated.
-- Called by:       ISR(INT0_vect)
************************************************************************************* --*/
void toggleSystemState() {
  systemOn = !systemOn;
  if (!systemOn) {
    digitalWriteFast(TREE1_LED_PIN, LOW);
    digitalWriteFast(TREE2_LED_PIN, LOW); 
    setColor(0, 0, 0);
  }
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Handles debouncing of the system state toggle switch and calls toggleSystemState when a valid state change is detected.
-- Called by:       setup() as interrupt
************************************************************************************* --*/
// ISR for toggling system state
ISR(INT0_vect) {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce check
  if (interruptTime - lastInterruptTime > 200) {
    toggleSystemState(); // Toggle the system state
    lastInterruptTime = interruptTime;
  }
}
/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Detects button presses to cycle through different lighting modes for both tree lights and the RGB LED.
-- Called by:       loop()
-- Modifications:   --
1. Tried to implement 2 modes for each tree. Stayed with the single logic.
2. Changed to digitalWriteFast.
3. Added a functionality for first activation, to activate for 4 to 6 seconds.
************************************************************************************* --*/
void handleModeChange() {
  static int lastButtonState1 = HIGH;
  static int lastButtonState2 = HIGH;
  static int lastButtonState3 = HIGH;

  int currentButtonState1 = digitalReadFast(TREE1_MODE_CHANGE_BUTTON_PIN);
  int currentButtonState2 = digitalReadFast(TREE2_MODE_CHANGE_BUTTON_PIN);
  int currentButtonState3 = digitalReadFast(RGB_MODE_CHANGE_BUTTON_PIN);

  // Check for tree 1 mode change button
  if (currentButtonState1 != lastButtonState1) {
    if (millis() - lastDebounceTime1 > UTILITY_DELAY) {
      if (currentButtonState1 == LOW) { // Button press confirmed
        tree1Mode = (tree1Mode + 1) % 5; // Cycle through the modes
        lastDebounceTime1 = millis(); // Reset the debounce timer
      }
    }
    lastButtonState1 = currentButtonState1; // Update the last button state
  }

  if (currentButtonState2 != lastButtonState2) {
    if (millis() - lastDebounceTime2 > UTILITY_DELAY) {
      if (currentButtonState2 == LOW) { // Button press confirmed
        tree2Mode = (tree2Mode + 1) % 5; // Cycle through the modes
        lastDebounceTime2 = millis(); // Reset the debounce timer
      }
    }
    lastButtonState2 = currentButtonState2; // Update the last button state
  }
  
  if (currentButtonState3 != lastButtonState3) {
    if (millis() - lastDebounceTime3 > UTILITY_DELAY) {
      if (currentButtonState3 == LOW) { // Button press confirmed
        rgbMode = (rgbMode + 1) % 5; // Cycle through the modes up to 5
        lastDebounceTime3 = millis(); // Reset the debounce timer
      }
    }
    lastButtonState3 = currentButtonState3; // Update the last button state
  }
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Controls tree lighting effects based on the specified mode, including blinking, breathing, stroboscopic, gradual on/off, and smooth transitions.
-- Called by:       loop(), when managing light behaviors based on system state and mode settings.
-- Modifications:   --
Starting logic was originally completely different. I did not use switch case, i wanted to have different functions, instead kept the single function multiple modes logic.
Stroboscopic changed dozen times.
************************************************************************************* --*/
void controlTreeLights(int treePin, int mode) {
  switch (mode) {
    case 0: // Faster Simple blinking
      digitalWriteFast(treePin, HIGH); // Turn on the light
      delay(100); // On for 100 milliseconds
      digitalWriteFast(treePin, LOW); // Turn off the light
      delay(100); // Off for 100 milliseconds
      break;
    case 1: // Faster Breathing pattern
      for (int i = 0; i <= 255; i += 5) { // Increase brightness more quickly
        analogWrite(treePin, i);
        delay(5); // Reduce delay for faster transition ( utility )
      }
      for (int i = 255; i >= 0; i -= 5) { // Decrease brightness more quickly
        analogWrite(treePin, i);
        delay(5); // Reduce delay for faster transition ( utility )
      }
      break;
    case 2: // Faster Stroboscopic effect
      for (int i = 0; i < 10; i++) { // Flash the light more times
        digitalWriteFast(treePin, HIGH);
        delay(10); // On for 10 milliseconds ( utility )
        digitalWriteFast(treePin, LOW);
        delay(10); // Off for 10 milliseconds ( utility )
      }
      break;
    case 3: // Faster Gradual on and off
      for (int i = 0; i <= 255; i += 10) { // Increase increment for quicker transition
        analogWrite(treePin, i);
        delay(15); // Delay for transition
      }
      for (int i = 255; i >= 0; i -= 10) { // Decrease increment for quicker transition
        analogWrite(treePin, i);
        delay(5); // Quicker off transition
      }
      break;
    case 4: // Faster Smooth transition through colors
      for (int color = 0; color <= 255; color += 5) { // Faster color change
        analogWrite(treePin, color);
        delay(5); // Reduced delay for smoother, faster change
      }
      break;
    default:
      digitalWriteFast(treePin, LOW); // Off by default
      break;
  }
}

int readPotentiometer(int pin) {
    return analogRead(pin); // Reads the potentiometer value directly
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Measures the distance to an object using ultrasonic waves, by sending a pulse and calculating the time taken for the echo to return.
-- Called by:       loop(), for determining if an object is within a specific range to trigger motion-based lighting.
-- Modifications:   None -> Got it from here https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/
************************************************************************************* --*/
long readUltrasonicDistance() {
  digitalWriteFast(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWriteFast(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWriteFast(ULTRASONIC_TRIG_PIN, LOW);
  
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and return)
  return distance;
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Adjusts the RGB LED's mode and executes the corresponding visual effect based on the current mode selection.
-- Called by:       loop(), when managing RGB LED behaviors based on selected mode.
-- Modifications:   --
Just like the tree leds, the i was following the same idea here. Starting with single function for each of the modes, ending up in a single function with switch case.
************************************************************************************* --*/
void changeRGBMode(int mode) {
  // Static variables to maintain state and timing across function calls
  static unsigned long previousMillis = 0;
  static int colorIndex = 0; // Index for color array in jump mode
  static int gradualStep = 0; // Step for gradual and smooth mode transitions
  static int rgbMode = 0; // Static to maintain the mode state across calls
  
  // Constants for timing intervals
  const long intervalJump = 1000; // Interval at which to jump colors (milliseconds)
  const long intervalStrobe = 250; // Interval for strobe effect (milliseconds)
  const long intervalGradual = 20; // Speed of gradual change
  const long intervalSmooth = 10; // Speed of smooth change

  // Update the mode only if a new mode is passed in
  if (mode != rgbMode) {
    rgbMode = mode;
    // Reset state for new mode
    previousMillis = millis();
    colorIndex = 0;
    gradualStep = 0;
  }

  unsigned long currentMillis = millis(); // Get current time

  // Logic to handle each mode
  switch (rgbMode) {
    case 1: { // Jump
      if (currentMillis - previousMillis >= intervalJump) {
        previousMillis = currentMillis;
        int colors[][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {0, 255, 255}, {255, 0, 255}, {255, 255, 255}};
        setColor(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
        colorIndex = (colorIndex + 1) % 7; // Cycle through colors
      }
      break;
    }
    case 2: { // Stroboscopic
      bool isOn = (currentMillis / intervalStrobe) % 2 == 0;
      setColor(isOn ? 255 : 0, isOn ? 255 : 0, isOn ? 255 : 0); // Only white strobe
      if (currentMillis - previousMillis >= intervalStrobe) {
        previousMillis = currentMillis; // timings for flashing
      }
      break;
    }
    case 3: { // Gradual
      if (currentMillis - previousMillis >= intervalGradual) {
        previousMillis = currentMillis;
        gradualStep = (gradualStep + 1) % 360; // Increment step for a continuous cycle
        
        // Calculating RGB values based on sine wave for smooth transition
        int r = (sin(gradualStep * (PI / 180) * 0.5) * 127.5) + 127.5; // Slow down the cycle by multiplying step by 0.5
        int g = (sin((gradualStep * (PI / 180) * 0.5) + 2 * PI / 3) * 127.5) + 127.5;
        int b = (sin((gradualStep * (PI / 180) * 0.5) + 4 * PI / 3) * 127.5) + 127.5;
        
        setColor(r, g, b);
      }
      break;
    }
    case 4: { // Smooth
      if (currentMillis - previousMillis >= intervalSmooth) {
        previousMillis = currentMillis;
        gradualStep = (gradualStep + 1) % 360; // Increment and wrap the step for continuous cycles
        
        // slower transition between colors by adjusting the sine wave frequency
        float frequency = 0.5;
        int r = (sin(gradualStep * (PI / 180) * frequency) * 127.5) + 127.5;
        int g = (sin((gradualStep * (PI / 180) * frequency + 2 * PI / 3)) * 127.5) + 127.5;
        int b = (sin((gradualStep * (PI / 180) * frequency + 4 * PI / 3)) * 127.5) + 127.5;
        
        setColor(r, g, b);
      }
      break;
    }
    default: {
      setColor(0, 0, 0); // Off
      break;
    }
  }
}

/*--**************************************************************************************-
-- Date:            February-March/2024
-- Purpose:         Sets the RGB LED color by adjusting the intensity of its red, green, and blue components.
-- Called by:       changeRGBMode(), controlTreeLights() (if modified to include RGB control), and any other function requiring direct manipulation of the RGB LED color.
-- Modifications:   None
************************************************************************************* --*/
void setColor(int red, int green, int blue) { // Utility function .. sets the color of the RGB LED
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);
}

/* 
------------------------------------------------------------------
 Issues with: Timings / timing missteps. 
 analogWrite is a pre written well composed song. But with direct register manipulation i had to compose the 'music' (light effects) myself.
--------------------------------- ---------------------------------
 */ 

// void setColorRegistryManipulation(int red, int green, int blue) {
//   // Green on pin 9 (OC1A), Red on pin 11 (OC2A), and Blue on pin 10 (OC1B)
//   OCR1A = green; // Green pin 9
//   OCR2A = red;   // Red pin 11
//   OCR1B = blue;  // Blue pin 10
// }

// void setupPWMRegistryManipulation() {
//   // pins 9, 10, 11 outputs for PWM
//   DDRB |= (1 << DDB1) | (1 << DDB2); // Pin 9 (PB1/OC1A), Pin 11 (PB3/OC2A) output
//   DDRB |= (1 << DDB3); // Pin 10 (PB2/OC1B) output

//   // Configure Timer1 for Fast PWM mode for pins 9 and 11
//   TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
//   TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS10); // No prescaler, fast PWM
//   ICR1 = 0x00FF; // 8-bit resolution just as analogWrite()

//   TCCR2A = (1 << WGM21) | (1 << WGM20) | (1 << COM2A1); // Fast PWM
//   TCCR2B = (1 << CS20); // No prescaler
// }
