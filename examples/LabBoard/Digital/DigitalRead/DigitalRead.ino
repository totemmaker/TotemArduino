/**
 * Display digital state of DIG1 and DIG2 pins.
 * DIG1 and DIG2 LED also represents pin state.
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
  // Read state of DIG pins
  int dig1 = LB.getDIG1();
  int dig2 = LB.getDIG2();
  // Write state to DIG LED
  LB.led.set(LabBoard::LED_DIG1, dig1);
  LB.led.set(LabBoard::LED_DIG2, dig2);
  // Print DIG pins state on display (align left)
  Serial.print("d2 ");
  Serial.print(dig2);
  Serial.print(" ");
  Serial.print(dig1);
  Serial.println(" d1");
}
