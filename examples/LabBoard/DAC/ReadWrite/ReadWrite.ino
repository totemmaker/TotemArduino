/**
 * Read and increment DAC output.
 * Each output increments at different rate.
 */
#include <TotemLabBoard.h>
TotemLabBoard LB;
void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Turn all LED off
  LB.led.off();
}

void loop() {
  // Get current DAC values
  float dac1 = LB.volt.getDAC1();
  float dac2 = LB.volt.getDAC2();
  float dac3 = LB.volt.getDAC3();
  // Update DAC values
  dac1 += 0.05;
  dac2 += 0.10;
  dac3 += 0.15;
  if (dac1 > 3.25) dac1 = 0;
  if (dac2 > 3.25) dac2 = 0;
  if (dac3 > 3.25) dac3 = 0;
  LB.volt.setDAC1(dac1);
  LB.volt.setDAC2(dac2);
  LB.volt.setDAC3(dac3);
  // Show DAC1, DAC2, DAC3 values on display
  LB.display.print(0, LB.volt.getDAC1());
  LB.display.print(3, LB.volt.getDAC2());
  LB.display.print(6, LB.volt.getDAC3());
  // Delay 0.5 seconds
  delay(500);
}
