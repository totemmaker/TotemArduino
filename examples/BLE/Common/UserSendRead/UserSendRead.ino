#include <Arduino.h>
#include <TotemRoboBoardX3.h>
#include <TotemRoboBoardX4.h>
/*
  ESP32 board ====> (BLE) ====> RoboBoard
  Send and read user defined values and strings to RoboBoard remotely.
  Supported boards: Any ESP32 board running Arduino.
  
  Run this example on RoboBoard:
  https://github.com/totemmaker/TotemArduinoBoards/blob/master/libraries/TotemRB/examples/TotemApp/TotemLibrary/TotemLibrary.ino
*/
// Select RoboBoard X3 or X4
TotemRoboBoardX3 roboboard;
// TotemRoboBoardX4 roboboard;

// Detect connection state change event
void onConnectionChange() {
  if (roboboard.isConnected()) Serial.println("Connected to RoboBoard");
  else Serial.println("Disconnected from RoboBoard");
}
// Intercept value sent by TotemApp.sendValue() (from RoboBoard)
void onReceiveValue(int id, int value) {
  switch (id) {
    case 0: Serial.printf("Servo A pos: %d\n", value); break;
    // Print if other ID is received
    default: Serial.printf("Got ID: %d, value: %d\n", id, value);
  }
}
// Intercept string sent by TotemApp.sendString() (from RoboBoard)
void onReceiveString(int id, String string) {
  switch (id) {
    case 1: Serial.printf("millis(): %s\n", string.c_str()); break;
    // Print if other ID is received
    default: Serial.printf("Got ID: %d, string: %s\n", id, string.c_str());
  }
}
// Initialize program
void setup() {
  Serial.begin(115200);
  // Register connection state event
  roboboard.addOnConnectionChange(onConnectionChange);
  // Register value receive functions
  roboboard.addOnReceive(onReceiveValue);
  roboboard.addOnReceive(onReceiveString);
  // Connect to RoboBoard over Bluetooth
  Serial.println("Looking for RoboBoard...");
  if (roboboard.connect()) {
    Serial.print("Connected to: ");
    Serial.println(roboboard.getName());
  }
  else {
    Serial.println("Connection failed...");
    while (1) {delay(1);}
  }
  // Print battery voltage
  Serial.print("Battery (mV): ");
  Serial.println(roboboard.getBattery());
  delay(3000);
}
// Loop program
void loop() {
  // Stop looping if connection is lost
  if (!roboboard.isConnected()) return;
  // Send values to RoboBoard
  Serial.println("Spin DC motors A:50, B:-50");
  roboboard.sendValue(0, 50);  // A to 50%
  roboboard.sendValue(1, -50); // B to -50%
  delay(1000);
  Serial.println("Stop DC motors A:0, B:0");
  roboboard.sendValue(0, 0); // A to 0%
  roboboard.sendValue(1, 0); // B to 0%
  delay(3000);
  Serial.println("Spin Servo A to 100 at 10 RPM");
  roboboard.sendValue(20, 10);  // Servo A RPM to 10
  roboboard.sendValue(10, 100); // Servo A pos to 100
  // Wait for motor to reach position
  while (roboboard.readValue(10) != 100) { delay(10); }
  Serial.printf("Motor at position: %d, angle: %d, pulse: %d\n",
    roboboard.readValue(10),
    roboboard.readValue(11),
    roboboard.readValue(12));
  delay(3000);
  Serial.println("Spin Servo A to 0");
  roboboard.sendValue(20, 0); // Servo A disable RPM limit
  roboboard.sendValue(10, 0); // Servo A pos to 0
  delay(3000);
}
