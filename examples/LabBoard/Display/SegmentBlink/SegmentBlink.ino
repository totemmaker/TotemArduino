/**
 * Blink individual display segments at different rates
 */
#include <TotemLabBoard.h>
TotemLabBoard LB;
void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Turn all LED off
  LB.led.off();
  // Display text
  LB.display.print("123444555");
  // Set blink parameters
  LB.display.setBlink(500); // Set all to 500ms
  LB.display.setBlink(6, 400); // Set 444 to 400ms
  LB.display.setBlink(5, 400); // Set 444 to 400ms
  LB.display.setBlink(4, 400); // Set 444 to 400ms
  LB.display.setBlink(3, 300); // Set 3 to 300ms
  LB.display.setBlink(2, 200); // Set 2 to 200ms
  LB.display.setBlink(1, 100); // Set 1 to 100ms
  // Can also use numbers. E.g. LabBoard::SEG1 -> 1.
}

void loop() {

}
