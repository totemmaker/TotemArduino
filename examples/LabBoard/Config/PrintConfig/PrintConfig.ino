/**
 * Print all LabBoard internal configuration (settings).
 * Flash this example to TotemDuino.
 * Open Arduino IDE serial monitor to view printed information.
 */
#include <TotemLabBoard.h>
TotemLabBoard LB;
// Function to print number with decimal point
void printlnDot(long value, int dot) {
  int power = 1;
  for (int i=0; i<dot; i++) { power *= 10; }
  Serial.println(float(value)/power, dot);
}

void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Turn all LED off
  LB.led.off();
  // Collect all configuration values
  long config[14];
  config[0] = LB.config.get("REV");
  config[1] = LB.config.get("VER");
  config[2] = LB.config.get("SBAUD");
  config[3] = LB.config.get("SMODE");
  config[4] = LB.config.get("SON");
  config[5] = LB.config.get("DISP");
  config[6] = LB.config.get("VREG");
  config[7] = LB.config.get("DAC1");
  config[8] = LB.config.get("DAC2");
  config[9] = LB.config.get("DAC3");
  config[10] = LB.config.get("VIN");
  config[11] = LB.config.get("50V");
  config[12] = LB.config.get("5V");
  config[13] = LB.config.get("05V");
  // Display all configuration values
  Serial.println("LabBoard configuration:");
  Serial.println("-----------------");
  Serial.print("Revision:\tv."); printlnDot(config[0], 1);
  Serial.print("Firmware:\t"); printlnDot(config[1], 2);
  Serial.print("Serial baud:\t"); Serial.println(config[2]);
  Serial.print("Serial mode:\t"); Serial.println(config[3] ? "Arduino" : "PC");
  Serial.print("Serial always:\t"); Serial.println(config[4] ? "On" : "Off");
  Serial.print("Brightness:\t"); Serial.print(config[5]); Serial.println("/15");
  Serial.print("VREG offset:\t"); printlnDot(config[6], 3);
  Serial.print("DAC1 offset:\t"); printlnDot(config[7], 3);
  Serial.print("DAC2 offset:\t"); printlnDot(config[8], 3);
  Serial.print("DAC3 offset:\t"); printlnDot(config[9], 3);
  Serial.print("VIN offset:\t"); printlnDot(config[10], 3);
  Serial.print("50V offset:\t"); printlnDot(config[11], 3);
  Serial.print("5V offset:\t"); printlnDot(config[12], 3);
  Serial.print("05V offset:\t"); printlnDot(config[13], 3);
  Serial.println("-----------------");
  Serial.println("Done");
}

void loop() {
  
}
