#include <Arduino.h>
#include <TotemRoboBoardX4.h>
/*
  ESP32 board ====> (BLE) ====> RoboBoard X4
  Control RoboBoard X4 remotely over Bluetooth Low Energy.
  Supported boards: Any ESP32 board running Arduino.
*/
TotemRoboBoardX4 roboboard;
// Initialize program
void setup() {
  Serial.begin(115200);
  Serial.println("Looking for RoboBoard X4...");
  // Connect to RoboBoard X4
  // if (roboboard.connectName("My Robot")) { // Connect only if name "My Robot" matches
  if (roboboard.connect()) {
    Serial.print("Connected to: ");
    Serial.println(roboboard.getName());
  }
  else {
    Serial.println("Connection failed...");
    while (1) {delay(1);}
  }
  // Print board information
  Serial.print("Software: ");
  Serial.println(miniboard.getVersion());
  Serial.print("Driver: ");
  Serial.println(miniboard.getDriverVersion());
  Serial.print("Revision: ");
  Serial.println(miniboard.getRevision());
  Serial.print("Battery: ");
  Serial.println(miniboard.getBattery());
}
// Loop program
void loop() {
  // Control RGB
  roboboard.rgbColor(125,0,0); // Red
  delay(200);
  roboboard.rgbColor(0,125,0); // Green
  delay(200);
  roboboard.rgbColor(0,0,125); // Blue
  delay(200);
  roboboard.rgbColorTotem(); // Totem
  delay(1000);
  // Blink LED
  roboboard.blink();
  delay(1000);
  // Control motors
  roboboard.dcSpinA(40); // Spin A 40% power forward
  roboboard.dcSpinB(-40); // Spin B 40% power backward
  delay(500);
  roboboard.dcSpinA(-40); // Spin A 40% power backward
  roboboard.dcSpinB(40); // Spin B 40% power forward
  delay(500);
  roboboard.dcBrakeA(); // Brake port A
  roboboard.dcBrakeB(); // Brake port B
  delay(1000);
  // Control servo
  roboboard.servoSpinA(-50); // Spin A to 50% left
  roboboard.servoSpinB(50); // Spin B to 50% right
  delay(700);
  roboboard.servoSpinA(50); // Spin A to 50% right
  roboboard.servoSpinB(-50); // Spin B to 50% left
  delay(700);
  roboboard.servoSpinA(0); // Spin A to center
  roboboard.servoSpinB(0); // Spin B to center
  delay(1000);
}
