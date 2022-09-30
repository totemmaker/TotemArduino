/**
 * Demonstrate serial monitor mode.
 * LabBoard will display all text, sent trough "Serial.print()"
 */
#include <Totem.h>

void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Turn all LED off
  LB.led.off();
}
// Number to display
float number = -15.0;

void loop() {
  // Increment number by 0.25
  number += 0.25;
  // Print text and number
  Serial.print("VAL ");
  Serial.println(number);
  // Wait 0.1 seconds
  delay(100);
}
