#define RGB_GREEN_PIN 8
#define RGB_BLUE_PIN 9
#define RGB_RED_PIN 10
#define ON_OFF_BUTTON_PIN 2
#define CONTINUOUS_MODE_BUTTON_PIN 5
#define TREE1_MODE_CHANGE_BUTTON_PIN 6
#define TREE2_MODE_CHANGE_BUTTON_PIN 6
#define RGB_MODE_CHANGE_BUTTON_PIN 4
#define MOTION_SENSOR_PIN 12
#define LIGHT_SENSOR_PIN A0
#define DURATION_POTENTIOMETER_PIN A2
#define DAYLIGHT_THRESHOLD_PIN A3
#define ULTRASONIC_TRIG_PIN 11
#define ULTRASONIC_ECHO_PIN 13
#define NUM_MODES 5 // only for rgb including off

// LEDs for trees are connected to these pins through NMOS transistors
#define TREE1_LED_PIN 7
#define TREE2_LED_PIN 3
#define DEBOUNCE_DELAY 50 // milliseconds

unsigned long lastDebounceTime1 = 0; // for TREE1_MODE_CHANGE_BUTTON_PIN
unsigned long lastDebounceTime2 = 0; // for TREE2_MODE_CHANGE_BUTTON_PIN
unsigned long lastDebounceTime3 = 0; // for RGB_MODE_CHANGE_BUTTON_PIN
volatile bool systemOn = false;
bool continuousMode = false;
int tree1Mode = 0; // Example modes: 0-normal, 1-breathing, 2-random, etc.
int tree2Mode = 0;
bool motionDetected = false;
bool initialActivationDone = false;
int rgbMode = 0;

int colorIndex = 0; // For jump mode
int gradualStep = 0; // For gradual and smooth mode

void setup() {
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(ON_OFF_BUTTON_PIN, INPUT_PULLUP);
  pinMode(CONTINUOUS_MODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(TREE1_MODE_CHANGE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(TREE2_MODE_CHANGE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RGB_MODE_CHANGE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(DURATION_POTENTIOMETER_PIN, INPUT);
  pinMode(DAYLIGHT_THRESHOLD_PIN, INPUT);
  pinMode(TREE1_LED_PIN, OUTPUT);
  pinMode(TREE2_LED_PIN, OUTPUT);
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

  // Correctly placing the attachInterrupt call within the setup function
  attachInterrupt(digitalPinToInterrupt(ON_OFF_BUTTON_PIN), ISR_toggleSystemState, CHANGE);
}

void loop() {
  static unsigned long lastDebounceTimeRGB = 0;
  static int lastButtonStateRGB = HIGH;
  static unsigned long lastMotionTime = 0;
  unsigned long currentMillis = millis();
  const long DISTANCE_THRESHOLD = 100;
  long distance = readUltrasonicDistance();
  int currentButtonStateRGB = digitalRead(RGB_MODE_CHANGE_BUTTON_PIN);
  int currentButtonStateContinuous = digitalRead(CONTINUOUS_MODE_BUTTON_PIN);
  static unsigned long lastDebounceTimeContinuous = 0;
  static int lastButtonStateContinuous = HIGH;
  
  // Read the light level and determine if it's night time
  int lightLevel = readPotentiometer(LIGHT_SENSOR_PIN);
  int daylightThreshold = readPotentiometer(DAYLIGHT_THRESHOLD_PIN);

  // Read the potentiometer and map the value to the range of 5 to 20 seconds
  int duration = readPotentiometer(DURATION_POTENTIOMETER_PIN);
  long minDuration = map(duration, 0, 1023, 5000, 20000); // Converts to milliseconds (5-20 seconds)

  handleModeChange();
  // Debounce the button press for the continuous mode toggle
  if (currentMillis - lastDebounceTimeContinuous > DEBOUNCE_DELAY) {
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
      if (digitalRead(MOTION_SENSOR_PIN) == HIGH && distance <= DISTANCE_THRESHOLD) {
        if (!motionDetected) { // If it's a new motion, update the lastMotionTime
          lastMotionTime = currentMillis;
          motionDetected = true;
        }
      }      
      // Keep the lights on if motion was detected and we are within the min duration
      if (motionDetected) {
        if (currentMillis - lastMotionTime <= minDuration) {
          // Only control lights if we're within the min duration period
          controlTreeLights(TREE1_LED_PIN, tree1Mode);
          controlTreeLights(TREE2_LED_PIN, tree2Mode);
          changeRGBMode(rgbMode); // Change the RGB mode
        } else {
          // Once min duration has elapsed, turn off the lights and reset motionDetected flag
          digitalWrite(TREE1_LED_PIN, LOW);
          digitalWrite(TREE2_LED_PIN, LOW);
          setColor(0, 0, 0);
          motionDetected = false; // Reset motion detected flag for the next motion event
        }
      }
    }
  } else {
    // Ensure lights are off during the day
    digitalWrite(TREE1_LED_PIN, LOW);
    digitalWrite(TREE2_LED_PIN, LOW);
    setColor(0, 0, 0);
  }
}

void toggleSystemState() {
  systemOn = !systemOn;
  if (!systemOn) {
    digitalWrite(TREE1_LED_PIN, LOW);
    digitalWrite(TREE2_LED_PIN, LOW); 
    setColor(0, 0, 0);
  }
}

// ISR for toggling system state
void ISR_toggleSystemState() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce check
  if (interruptTime - lastInterruptTime > 200) {
    toggleSystemState(); // Toggle the system state
    lastInterruptTime = interruptTime;
  }
}

void handleModeChange() {
  static int lastButtonState1 = HIGH;
  static int lastButtonState2 = HIGH;
  static int lastButtonState3 = HIGH;

  int currentButtonState1 = digitalRead(TREE1_MODE_CHANGE_BUTTON_PIN);
  int currentButtonState2 = digitalRead(TREE2_MODE_CHANGE_BUTTON_PIN);
  int currentButtonState3 = digitalRead(RGB_MODE_CHANGE_BUTTON_PIN);

  // Check for tree 1 mode change button
  if (currentButtonState1 != lastButtonState1) {
    if (millis() - lastDebounceTime1 > DEBOUNCE_DELAY) {
      if (currentButtonState1 == LOW) { // Button press confirmed
        tree1Mode = (tree1Mode + 1) % 5; // Cycle through the modes
        lastDebounceTime1 = millis(); // Reset the debounce timer
      }
    }
    lastButtonState1 = currentButtonState1; // Update the last button state
  }

  if (currentButtonState2 != lastButtonState2) {
    if (millis() - lastDebounceTime2 > DEBOUNCE_DELAY) {
      if (currentButtonState2 == LOW) { // Button press confirmed
        tree2Mode = (tree2Mode + 1) % 5; // Cycle through the modes
        lastDebounceTime2 = millis(); // Reset the debounce timer
      }
    }
    lastButtonState2 = currentButtonState2; // Update the last button state
  }
  
  if (currentButtonState3 != lastButtonState3) {
    if (millis() - lastDebounceTime3 > DEBOUNCE_DELAY) {
      if (currentButtonState3 == LOW) { // Button press confirmed
        rgbMode = (rgbMode + 1) % 5; // Cycle through the modes
        lastDebounceTime3 = millis(); // Reset the debounce timer
      }
    }
    lastButtonState3 = currentButtonState3; // Update the last button state
  }
}

void controlTreeLights(int treePin, int mode) {
  switch (mode) {
    case 0: // Faster Simple blinking
      digitalWrite(treePin, HIGH); // Turn on the light
      delay(100); // On for 100 milliseconds
      digitalWrite(treePin, LOW); // Turn off the light
      delay(100); // Off for 100 milliseconds
      break;
    case 1: // Faster Breathing pattern
      for (int i = 0; i <= 255; i += 5) { // Increase brightness more quickly
        analogWrite(treePin, i);
        delay(5); // Reduce delay for faster transition
      }
      for (int i = 255; i >= 0; i -= 5) { // Decrease brightness more quickly
        analogWrite(treePin, i);
        delay(5); // Reduce delay for faster transition
      }
      break;
    case 2: // Faster Stroboscopic effect
      for (int i = 0; i < 10; i++) { // Flash the light more times
        digitalWrite(treePin, HIGH);
        delay(10); // On for 10 milliseconds
        digitalWrite(treePin, LOW);
        delay(10); // Off for 10 milliseconds
      }
      break;
    case 3: // Faster Gradual on and off
      for (int i = 0; i <= 255; i += 10) { // Increase increment for quicker transition
        analogWrite(treePin, i);
        delay(15); // Adjust delay for faster transition
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
      digitalWrite(treePin, LOW); // Off by default
      break;
  }
}

int readPotentiometer(int pin) {
    return analogRead(pin); // Reads the potentiometer value directly
}

long readUltrasonicDistance() {
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and return)
  return distance;
}

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

  unsigned long currentMillis = millis(); // Grab current time

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
      setColor(isOn ? 255 : 0, isOn ? 255 : 0, isOn ? 255 : 0); // White strobe
      if (currentMillis - previousMillis >= intervalStrobe) {
        previousMillis = currentMillis; // Update timing for consistent flashing
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
        
        // Enhanced smooth transition effect
        // Use a slower transition between colors by adjusting the sine wave frequency
        float frequency = 0.5; // Adjust frequency for smoother transitions
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


// Utility function to set the color of the RGB LED
void setColor(int red, int green, int blue) {
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);
}
