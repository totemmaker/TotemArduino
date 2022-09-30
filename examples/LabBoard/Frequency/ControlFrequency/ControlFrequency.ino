/**
 * Varying frequency of 50% duty cycle.
 * Outputs on TXD pin and changes between 1000Hz and 2000Hz.
 */
#include <Totem.h>

void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Turn all LED off
  LB.led.off();
  // Print info
  LB.display.print("1000-2000");
  // Enable frequency output on TXD pin
  LB.txd.setFrequency(1000); // set 1000Hz
  LB.txd.setDutyCycle(10); // Set 10% duty cycle
  LB.txd.start(); // Start output on TXD pin
}

void loop() {
  // Iterate from 1000 to 2000
  for (int i=1000; i<2000; i+=50) {
    // Update frequency
    LB.txd.setFrequency(i);
    delay(15);
  }
  // Iterate from 2000 to 1000
  for (int i=2000; i>1000; i-=50) {
    // Update frequency
    LB.txd.setFrequency(i);
    delay(15);
  }
}
