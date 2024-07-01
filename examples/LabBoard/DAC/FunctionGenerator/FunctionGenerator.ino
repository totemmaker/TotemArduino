/**
 * Demonstration on how to control DAC outputs. This example draws:
 * DAC1 - sine function
 * DAC2 - inverted sine function
 * DAC3 - triangle
 */
#include <TotemLabBoard.h>
TotemLabBoard LB;
void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Turn all LED off
  LB.led.off();
  // Display each DAC channel representation: SIN, -SIN, TRIANGLE
  LB.display.print("SIN-SITRI");
}
// Variable to increment x axis
float x = 0;

void loop() {
  // Increment
  x += 0.05;
  // If overlaps PI*2 - reset to zero
  if (x > PI*2) x = 0.0;
  // Calculate sine
  float sine = sin(x);
  // Map sinus value to DAC1 output
  LB.volt.setDAC1(((sine+1)/2) * 3.25);
  // Map inverted sinus value to DAC2 output
  LB.volt.setDAC2((((-sine)+1)/2) * 3.25);
  // Map x value to DAC3 output
  LB.volt.setDAC3(x > PI ? ((PI*2)-x) : x);
}
