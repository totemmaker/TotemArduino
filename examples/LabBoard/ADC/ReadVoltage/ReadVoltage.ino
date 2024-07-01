/**
 * Display voltage readings.
 * Each display segment shows 0.5V, 5V, 50V respectively
 */
#include <TotemLabBoard.h>
TotemLabBoard LB;
void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Turn all LED off
  LB.led.off();
  // Briefly display related values
  LB.display.print("0.5  5  50");
}

void loop() {
  // Show DAC1, DAC2, DAC3 values on display
  float volt1 = LB.volt.get05V();
  float volt2 = LB.volt.get5V();
  float volt3 = LB.volt.get50V();
  LB.display.print(0, volt1 == -100 ? 0 : volt1);
  LB.display.print(3, volt2 == -100 ? 0 : volt2);
  LB.display.print(6, volt3 == -100 ? 0 : volt3);
  // Delay 0.5 seconds
  delay(100);
}
