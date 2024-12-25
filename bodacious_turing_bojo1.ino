#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

const int incrementButtonPin = 9;
const int cursorButtonPin = 10;
const int setResetButtonPin = 11; // New button for set/reset functionality

int numbers[4] = {0, 0, 0, 0}; // Array to store digits for 4 positions
int cursorPosition = 0;
int lastIncrementButtonState = HIGH;
int lastCursorButtonState = HIGH;
int lastSetResetButtonState = HIGH;
unsigned long lastSetResetPressTime = 0;
bool cursorMoved = false;
bool setResetActivated = false;
bool longPressTriggered = false;

void setup() {
  lcd.init(); // Use init() instead of begin()
  lcd.backlight();

  // Set button pins as input with pull-up resistors
  pinMode(incrementButtonPin, INPUT_PULLUP);
  pinMode(cursorButtonPin, INPUT_PULLUP);
  pinMode(setResetButtonPin, INPUT_PULLUP); // Initialize the set/reset button

  // Initialize serial monitor for debugging
  Serial.begin(9600);

  // Display the initial numbers
  displayNumbers();
}

void loop() {
  int incrementButtonState = digitalRead(incrementButtonPin);
  int cursorButtonState = digitalRead(cursorButtonPin);
  int setResetButtonState = digitalRead(setResetButtonPin);

  // Check if the increment button is pressed
  if (incrementButtonState == LOW && lastIncrementButtonState == HIGH) {
    numbers[cursorPosition] = (numbers[cursorPosition] + 1) % 10; // Increment and wrap around to 0 after 9
    displayNumbers();
    delay(200); // Debounce delay
  }

  // Check if the cursor button is pressed
  if (cursorButtonState == LOW && lastCursorButtonState == HIGH) {
    lastSetResetPressTime = millis(); // Record the time of cursor button press
    cursorMoved = true;
    cursorPosition = (cursorPosition + 1) % 4; // Move cursor and wrap around after 4 positions
    setResetActivated = false; // Reset the set/reset state
    longPressTriggered = false; // Reset long press state
    displayNumbers();
    delay(200); // Debounce delay
  }

  // Check if the set/reset button is pressed
  if (setResetButtonState == LOW && lastSetResetButtonState == HIGH) {
    lastSetResetPressTime = millis(); // Record the time of set/reset button press
    delay(200); // Debounce delay
  }

  // Check if the set/reset button has been held for more than 2000ms (long press)
  if (setResetButtonState == LOW && millis() - lastSetResetPressTime > 2000) {
    if (!longPressTriggered) {
      longPressTriggered = true;
      resetDigits(); // Long press will reset digits
    }
  }

  // If the set/reset button is released before the long press time, it sets the digits
  if (setResetButtonState == HIGH && millis() - lastSetResetPressTime <= 2000 && !longPressTriggered) {
    if (!setResetActivated) {
      setResetActivated = true; // Single press to set digits
      setDigits(); // You can add a message or perform other actions here
    }
  }

  lastIncrementButtonState = incrementButtonState;
  lastCursorButtonState = cursorButtonState;
  lastSetResetButtonState = setResetButtonState;
}

void displayNumbers() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Digits:");
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(i, 1);
    lcd.print(numbers[i]);
  }
  lcd.setCursor(cursorPosition, 1); // Set the cursor to the current position

  if (longPressTriggered) {
    lcd.setCursor(0, 1);
    lcd.print("Digits Reset!");
    delay(1000);
    //lcd.clear();
  }
}

void resetDigits() {
  // Reset digits to 0000
  for (int i = 0; i < 4; i++) {
    numbers[i] = 0;
  }
  displayNumbers(); // Update the display to show reset digits
}

void setDigits() {
  // Set the digits in the array, or add additional behavior here for setting
  displayNumbers(); // Just updating display as a placeholder for setting behavior
    // If set/reset activated, show a confirmation or status message
  if (setResetActivated) {
    lcd.setCursor(5, 1);
    lcd.print("Digits Set!");
    delay(1000);
    //lcd.clear();
  }
}
