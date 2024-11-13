#include <Servo.h>
#include <SoftwareSerial.h>

//#include <Stepper.h>
//const int stepsPerRevolution = 2048;  // Number of steps per revolution for 28BYJ-48
//const int stepPin1 = 2;  // IN1 on the ULN2003 driver
//const int stepPin2 = 3;  // IN2 on the ULN2003 driver
//const int stepPin3 = 4;  // IN3 on the ULN2003 driver
//const int stepPin4 = 5;  // IN4 on the ULN2003 driver
//Stepper myStepper(stepsPerRevolution, stepPin1, stepPin2, stepPin3, stepPin4);

Servo servo1;  // Create servo object to control a servo on pin 5
Servo servo2;  // Create servo object to control a servo on pin 6

SoftwareSerial bluetooth(10, 11); // RX, TX for communication with first Arduino

//int currentXAngle = 0;  // Current angle of the X axis

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud
  bluetooth.begin(9600); // Start software serial communication with first Arduino
  //myStepper.setSpeed(10);  // Set the speed to 10 RPM
  servo1.attach(5);    // Attach servo2 to pin 5
  servo2.attach(6);   // Attach servo2 to pin 6
}

void loop() {
  // Check for incoming data from the first Arduino
  if (bluetooth.available() > 0) {
    String input = bluetooth.readStringUntil('\n');
    input.trim();  // Remove any leading or trailing whitespace

    // Split the input into x and y values
    int spaceIndex = input.indexOf(' ');
    String xString = input.substring(0, spaceIndex);
    String yString = input.substring(spaceIndex + 1);

    int x = xString.toInt();
    int y = yString.toInt();

    // Calculate the difference in angle for the X axis
    //int angleDifference = x - currentXAngle;
    //currentXAngle = x;  // Update the current angle

    // Calculate the number of steps needed
    //int steps = (angleDifference * stepsPerRevolution) / 360;

    // Rotate the stepper motor to the specified steps
    //myStepper.step(steps);

    // Rotate the servo to the specified angle
    servo1.write(x);
    delay(1000);
    servo2.write(y);

    // Send confirmation back to the first Arduino
    bluetooth.println("Satellite aligned: X" + String(x) + " Y" + String(y));
  }
}
