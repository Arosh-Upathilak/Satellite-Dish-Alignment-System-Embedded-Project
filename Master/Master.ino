#include <Keypad.h>
#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include <SoftwareSerial.h>

// Keypad setup
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

SoftwareSerial bluetooth(10, 11); // RX, TX for communication with second Arduino

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Select station");
  lcd.setCursor(0, 1);
  lcd.print("to align (A-D)");
  Serial.begin(9600);
  bluetooth.begin(9600); // Start software serial communication with second Arduino
}

void loop() {
  char key = keypad.getKey(); // Read the key
  
  if (key) {
    if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
      if (key != 'A'){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Invalid station");
        delay(2000); // Wait for 2 seconds
        resetDisplay();
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter X coord:");
        int x = getCoordinate(0, 180); // Pass range for X coordinate
        if (x == -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Invalid X coord");
          delay(2000); // Wait for 2 seconds
          resetDisplay();
          return;
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter Y coord:");
        int y = getCoordinate(0, 180); // Pass range for Y coordinate
        if (y == -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Invalid Y coord");
          delay(2000); // Wait for 2 seconds
          resetDisplay();
          return;
        }
        // Display coordinates
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("X:");
        lcd.print(x);
        lcd.setCursor(0, 1);
        lcd.print("Y:");
        lcd.print(y);
        delay(2000);
        resetDisplay();

        // Send coordinates to the second Arduino
        bluetooth.print(x);
        bluetooth.print(" ");
        bluetooth.println(y);

        // Wait for confirmation from the second Arduino
        while (bluetooth.available() == 0) {
          // Wait for response
        }
        String response = bluetooth.readStringUntil('\n');
        lcd.clear();
        if (response.length() <= 16) {
          lcd.setCursor(0, 0);
          lcd.print(response);
        } else {
          lcd.setCursor(0, 0);
          lcd.print(response.substring(0, 16)); // Display the first 16 characters
          lcd.setCursor(0, 1);
          lcd.print(response.substring(16)); // Display the remaining characters
        }

        delay(5000); // Display the response for 5 seconds
        resetDisplay();
      }
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid station");
      delay(2000); // Wait for 2 seconds
      resetDisplay();
    }
  }
}

int getCoordinate(int minRange, int maxRange) {
  String input = "";
  char key;
  while (true) {
    key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        input += key;
        lcd.setCursor(input.length() - 1, 1);
        lcd.print(key);
      } else if (key == '#') { // '#' acts as Enter key
        int coord = input.toInt();
        if (coord >= minRange && coord <= maxRange && input.length() > 0) {
          return coord;
        } else {
          return -1;
        }
      } else if (key == '*') { // '*' acts as a backspace key
        if (input.length() > 0) {
          input.remove(input.length() - 1);
          lcd.setCursor(input.length(), 1);
          lcd.print(" ");
          lcd.setCursor(input.length(), 1);
        }
      }
    }
  }
}

void resetDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select station");
  lcd.setCursor(0, 1);
  lcd.print("to align (A-D)");
}
