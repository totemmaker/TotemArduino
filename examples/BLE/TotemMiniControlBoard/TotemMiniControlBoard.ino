#include <Arduino.h>
#include <TotemMiniControlBoard.h>
/*
  ESP32 board ====> (BLE) ====> Mini Control Board
  Control Mini Control Board remotely over Bluetooth Low Energy.
  Supported boards: Any ESP32 board running Arduino.
*/
TotemMiniControlBoard miniboard;
// Initialize program
void setup() {
  Serial.begin(115200);
  Serial.println("Looking for Mini Control Board...");
  // Connect to Mini Control Board
  // if (miniboard.connectName("My Robot")) { // Connect only if name "My Robot" matches
  if (miniboard.connect()) {
    Serial.print("Connected to: ");
    Serial.println(miniboard.getName());
  }
  else {
    Serial.println("Connection failed...");
    while (1) {delay(1);}
  }
  // Print board information
  Serial.print("Firmware: ");
  Serial.println(miniboard.getVersion());
  Serial.print("Revision: ");
  Serial.println(miniboard.getRevision());
  Serial.print("Battery: ");
  Serial.println(miniboard.getBattery());
}
// Loop program
void loop() {
  if (miniboard.hasRGB()) {
    // Control RGB (v2.0 board with RGB lights)
    miniboard.rgbColor(125,0,0); // Red
    delay(200);
    miniboard.rgbColor(0,125,0); // Green
    delay(200);
    miniboard.rgbColor(0,0,125); // Blue
    delay(200);
    miniboard.rgbColorTotem(); // Totem
    delay(1000);
  }
  else {
    // Control LED (v1.x board with LED strip)
    for (int times=0; times<2; times++) {
      for (int i=0; i<12; i++) {
        miniboard.setLED((1<<i));
        delay(50);
      }
      for (int i=11; i>=0; i--) {
        miniboard.setLED((1<<i));
        delay(50);
      }
    }
  }
  // Control motors
  miniboard.dcSpinA(40); // Spin A 40% power forward
  miniboard.dcSpinB(-40); // Spin B 40% power backward
  delay(500);
  miniboard.dcSpinA(-40); // Spin A 40% power backward
  miniboard.dcSpinB(40); // Spin B 40% power forward
  delay(500);
  miniboard.dcBrakeA(); // Brake port A
  miniboard.dcBrakeB(); // Brake port B
  delay(1000);
  // Control servo
  miniboard.servoSpinA(-50); // Spin A to 50% left
  miniboard.servoSpinB(50); // Spin B to 50% right
  delay(700);
  miniboard.servoSpinA(50); // Spin A to 50% right
  miniboard.servoSpinB(-50); // Spin B to 50% left
  delay(700);
  miniboard.servoSpinA(0); // Spin A to center
  miniboard.servoSpinB(0); // Spin B to center
  delay(1000);
}
