#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUTTON_PIN 2
#define MOTION_SENSOR_PIN 3
#define RED_LIGHT_CAR_PIN 4
#define YELLOW_LIGHT_CAR_PIN 5
#define GREEN_LIGHT_CAR_PIN 6
#define RED_LIGHT_PED_PIN 7
#define BUZZER_PIN 9
#define RGB_RED_PIN 10
#define RGB_GREEN_PIN 11

#define LCD_ADDRESS_0x20 0x20
#define LCD_ADDRESS_0x26 0x26

#define YELLOW_TO_RED_DELAY 2500
#define RED_TO_GREEN_DELAY 3000
#define PEDESTRIAN_CROSSING_TIME 8000
#define SOUND_SPEEDUP_TIME 5000
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

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(RED_LIGHT_CAR_PIN, OUTPUT);
  pinMode(YELLOW_LIGHT_CAR_PIN, OUTPUT);
  pinMode(GREEN_LIGHT_CAR_PIN, OUTPUT);
  pinMode(RED_LIGHT_PED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
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

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

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

void handlePedestrianButtonPress() {
  pedestrianRequest = true;
  buttonPressTimestamp = millis();
}

void processPedestrianRequest(unsigned long currentMillis) {
  if (trafficLightState == GREEN && pedestrianRequest) {
    if ((!motionDetected) || (motionDetected && currentMillis - buttonPressTimestamp >= MOTION_DETECTED_DELAY)) {
      setTrafficLightState(YELLOW);
      buttonPressTimestamp = currentMillis;
      pedestrianRequest = false; // Reset request to prevent re-entry
    }
  }
}

void setTrafficLightState(TrafficLightState state) {
  trafficLightState = state;
  stateChangeTimestamp = millis();
  
  digitalWrite(RED_LIGHT_CAR_PIN, state == RED || state == RED_YELLOW ? HIGH : LOW);
  digitalWrite(YELLOW_LIGHT_CAR_PIN, state == YELLOW || state == RED_YELLOW ? HIGH : LOW);
  digitalWrite(GREEN_LIGHT_CAR_PIN, state == GREEN ? HIGH : LOW);
  
  // Manage pedestrian light directly within state transitions if needed
  if(state == RED) {
    delay(500); // Delay for 0.5 seconds
    setPedestrianRGB(false); // Turn pedestrian light green
  } else {
    setPedestrianRGB(true); // Otherwise, keep pedestrian light red
  }
  
  // Specific handling when transitioning to GREEN to clear the timer display
  if (state == GREEN) {
    lcd_0x20.setCursor(6, 1); // Adjust cursor to the position of the timer
    lcd_0x20.print("       "); // Clear the timer display area
  }
}

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
      // Transition to GREEN for cars, which implies RED for pedestrians
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

void checkMotionSensor() {
  motionDetected = digitalRead(MOTION_SENSOR_PIN) == HIGH;
}

void setPedestrianRGB(bool isRed) {
  digitalWrite(RGB_RED_PIN, isRed ? HIGH : LOW);
  digitalWrite(RGB_GREEN_PIN, !isRed ? HIGH : LOW);
}

void updateAudibleVisualCues(unsigned long currentMillis) {
  if (trafficLightState == RED || trafficLightState == RED_YELLOW) {
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
  }
}

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
      lcd_0x20.setCursor(6, 1); // Adjust based on your layout
      if (remainingTime < 10) { // Padding for single digit numbers
        lcd_0x20.print("0"); // Padding for consistency
      }
      lcd_0x20.print(String(remainingTime + 1) + "s  "); // Update time
      lastDisplayedTime = remainingTime;
    }
  } else {
    // If the state is GREEN and no pedestrian request, clear the timer display area
    if (lastDisplayedTime != 0) {
      lcd_0x20.setCursor(6, 1); // Adjust cursor to timer position
      lcd_0x20.print("       "); // Clear the timer display area
      lastDisplayedTime = 0; // Reset last displayed time
    }
  }
}

String getStateText(TrafficLightState state) {
  switch (state) {
    case RED: return "Red";
    case RED_YELLOW: return "Red+Yellow";
    case GREEN: return "Green";
    case YELLOW: return "Yellow";
    default: return "";
  }
}

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


// #include <avr/pgmspace.h>
// Global scope for PROGMEM strings
// const char greenCarMsg[] PROGMEM = "Commercial Green Car";
// const char redCarMsg[] PROGMEM = "Commercial Red Car";

// void displayStaticEfficientFromProgmem() {
//   char buffer[25]; // Buffer for copying the message from PROGMEM
//   strcpy_P(buffer, (isGreen ? greenCarMsg : redCarMsg)); // You need to pass isGreen to this function or decide which message to use before calling
//   buffer[24] = '\0'; // Explicitly null-terminate

//   lcd_0x26.clear();
//   lcd_0x26.print(buffer); // Display the static message
// }

// void updateCarDisplay(bool isGreen) {
//   unsigned long currentMillis = millis();

//   if (currentMillis - previousMillis >= interval) {
//     previousMillis = currentMillis;
//     displayStaticTestFromProgmem(); // Adjust this function to either accept isGreen or decide the message before calling
//   }
// }