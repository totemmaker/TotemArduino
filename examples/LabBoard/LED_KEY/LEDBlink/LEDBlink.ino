/**
 * LED control demonstration. 3 different animations:
 * - Wave from left to right
 * - Blink all LED
 * - Increment and display binary number
 */
#include <TotemLabBoard.h>
TotemLabBoard LB;
void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Turn all LED off
  LB.led.off();
  // Clear display
  LB.display.clear();
}

void loop() {
  // Play wave 3 times
  for (int cnt=0; cnt<3; cnt++) {
    for (int i=1; i<=11; i++) {
      LB.led.set(i, 1);
      delay(50);
    }
    for (int i=1; i<=11; i++) {
      LB.led.set(i, 0);
      delay(50);
    }
  }
  // Blink all 3 times
  for (int cnt=0; cnt<3; cnt++) {
    LB.led.set(LabBoard::LED_ALL, 1);
    delay(300);
    LB.led.set(LabBoard::LED_ALL, 0);
    delay(300);
  }
  // Binary count. Increment cnt number and display on LEDs
  for (int cnt=0; cnt!=0x7FF; cnt++) {
    LB.led.setBinary(cnt);
    delay(20);
  }
}
