/**
 * Varying duty cycle of 1000Hz frequency.
 * Outputs on TXD pin and changes between 10% and 90%.
 */
#include <Totem.>

void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Turn all LED off
  LB.led.off();
  // Print info
  LB.display.print("10\% - 90\%");
  // Enable frequency output on TXD pin
  LB.txd.setFrequency(1000); // set 1000Hz
  LB.txd.setDutyCycle(10); // Set 10% duty cycle
  LB.txd.start(); // Start output on TXD pin
}

void loop() {
  // Iterate from 10 to 90
  for (int i=10; i<90; i++) {
    // Update duty cycle
    LB.txd.setDutyCycle(i);
    delay(15);
  }
  // Iterate from 90 to 10
  for (int i=90; i>10; i--) {
    // Update duty cycle
    LB.txd.setDutyCycle(i);
    delay(15);
  }
}
