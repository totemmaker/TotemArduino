/**
 * Signal frequency measure demonstration.
 * This example outputs 1kHz frequency signal at pin D9 and
 * shows pin DIG1 measurement on display.
 * Connect wire from pin D9 to DIG1 and observe display showing
 * number 1000 - measured frequency of pin D9.
 */
#include <Totem.h>

void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Output 1kHz frequency to pin D9
  tone(9, 1000);
  // Turn all LED off
  LB.led.off();
  // Clear display
  LB.display.clear();
  // Start frequency measure on DIG1 pin
  LB.rxd.start();
}

void loop() {
  // Print measured frequency to display (align right)
  Serial.println(LB.rxd.getFrequency());
  delay(50);
}
