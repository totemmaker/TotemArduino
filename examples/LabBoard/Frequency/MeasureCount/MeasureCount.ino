/**
 * Signal counter demonstration.
 * This example outputs 1kHz frequency signal at pin D9 and
 * shows pin DIG1 counter value on display.
 * Counter value is incremented when signal goes from LOW to HIGH.
 * Connect wire from pin D9 to DIG1 and observe number increment
 * by 1000 each 1 second.
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
  // Reset count to 0
  LB.rxd.resetCount();
}

void loop() {
  // Print measured count to display (align right)
  Serial.println(LB.rxd.getCount());
  delay(50);
}
