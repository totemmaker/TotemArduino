/**
 * Control LabBoard display brightness
 */
#include <TotemLabBoard.h>
TotemLabBoard LB;
void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Prepare display
  LB.display.print("888   888");
  LB.led.off(); // Off all LED
  LB.led.set(LabBoard::LED_5V, 1); // Turn 5V LED on
  LB.led.set(LabBoard::LED_DAC2, 1); // Turn DAC2 LED on
  LB.led.set(LabBoard::LED_VREG, 1); // Turn VREG LED on
}

void loop() {
  // Fade to bright
  for (int i=0; i<16; i++) {
    // Print brightness number at middle of the screen
    LB.display.print(i > 9 ? 4 : 5, i);
    // Write brightness
    LB.display.setBrightness(i);
    // Wait 0.2 seconds
    delay(200);
  }
  // Wait 0.7 seconds
  delay(700);
  // Fade to dim
  for (int i=15; i>=0; i--) {
    // Clean display if 'i' goes below 10
    if (i == 9) LB.display.print(3, "   ");
    // Print brightness number at middle of the screen
    LB.display.print(i > 9 ? 4 : 5, i);
    // Write brightness
    LB.display.setBrightness(i);
    // Wait 0.2 seconds
    delay(200);
  }
  // Wait 0.7 seconds
  delay(700);
}
