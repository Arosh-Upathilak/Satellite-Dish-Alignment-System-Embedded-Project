#include <Keypad.h>
#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include <SoftwareSerial.h>
#include <math.h>

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
        lcd.print("Enter dish lat:");
        float dishLat = getCoordinate(-90.0, 90.0); // Pass range for latitude
        if (dishLat == -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Invalid latitude");
          delay(2000); // Wait for 2 seconds
          resetDisplay();
          return;
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter dish lon:");
        float dishLon = getCoordinate(-180.0, 180.0); // Pass range for longitude
        if (dishLon == -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Invalid longitude");
          delay(2000); // Wait for 2 seconds
          resetDisplay();
          return;
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter sat lat:");
        float satLat = getCoordinate(-90.0, 90.0); // Pass range for latitude
        if (satLat == -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Invalid latitude");
          delay(2000); // Wait for 2 seconds
          resetDisplay();
          return;
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter sat lon:");
        float satLon = getCoordinate(-180.0, 180.0); // Pass range for longitude
        if (satLon == -1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Invalid longitude");
          delay(2000); // Wait for 2 seconds
          resetDisplay();
          return;
        }

        // Calculate azimuth and elevation
        float azimuth, elevation;
        calculateAngles(dishLat, dishLon, satLat, satLon, azimuth, elevation);

        // Display angles
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Azimuth:");
        lcd.print(azimuth);
        lcd.setCursor(0, 1);
        lcd.print("Elevation:");
        lcd.print(elevation);
        delay(2000);
        resetDisplay();

        // Send angles to the second Arduino
        bluetooth.print(azimuth);
        bluetooth.print(" ");
        bluetooth.println(elevation);

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

float getCoordinate(float minRange, float maxRange) {
  String input = "";
  char key;
  bool isNegative = false;
  while (true) {
    key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        input += key;
        lcd.setCursor(input.length() - 1, 1);
        lcd.print(key);
      } else if (key == 'B') { // 'B' toggles negative sign
        isNegative = !isNegative;
        lcd.setCursor(0, 1);
        lcd.print(isNegative ? "-" : " ");
        lcd.setCursor(input.length(), 1);
      } else if (key == 'C') { // 'C' adds decimal point
        if (!input.contains(".")) {
          input += ".";
          lcd.setCursor(input.length() - 1, 1);
          lcd.print(".");
        }
      } else if (key == '#') { // '#' acts as Enter key
        float coord = input.toFloat();
        if (isNegative) coord = -coord;
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

void calculateAngles(float dishLat, float dishLon, float satLat, float satLon, float &azimuth, float &elevation) {
  // Convert degrees to radians
  dishLat = radians(dishLat);
  dishLon = radians(dishLon);
  satLat = radians(satLat);
  satLon = radians(satLon);

  // Convert to Cartesian coordinates
  float dishX = 6371 * cos(dishLat) * cos(dishLon);
  float dishY = 6371 * cos(dishLat) * sin(dishLon);
  float dishZ = 6371 * sin(dishLat);
  float satX = 6371 * cos(satLat) * cos(satLon);
  float satY = 6371 * cos(satLat) * sin(satLon);
  float satZ = 6371 * sin(satLat);

  // Vector from dish to satellite
  float vectorX = satX - dishX;
  float vectorY = satY - dishY;
  float vectorZ = satZ - dishZ;

  // Azimuth angle
  azimuth = degrees(atan2(vectorY, vectorX));

  // Elevation angle
  float distanceXY = sqrt(vectorX * vectorX + vectorY * vectorY);
  elevation = degrees(atan2(vectorZ, distanceXY));
}
