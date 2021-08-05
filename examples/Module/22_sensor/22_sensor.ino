/*
 * Copyright (c) 2021 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example code to show functionality of Totem Environment sensor module (number 22).
 * Module will show various environment data (temperature, pressure, humidity, light).
 * 
 * INSTRUCTIONS:
 * 1. Connect Totem Environment sensor module to X4 over TotemBUS connector.
 * 2. Upload code to X4.
 */
// Totem Library include
#include "Totem.h"
// Define sensor module object
TotemModule module(22);
// When set to true will send commands subscription to module
bool sendSubscribe = false;
// Will print data when it has been updated
bool dataUpdated = false;
// Will receive event of each module connection and ping response
void onModuleConnected(uint16_t number, uint16_t serial) {
  // Print connected module information
  Serial.print("Module connected. Number: ");
  Serial.print(number);
  Serial.print(" Serial: ");
  Serial.println(serial);
  // If module is connected prepare to send subscriptions
  if (number == module.getNumber())
    sendSubscribe = true;
}
// List of commands of this example. Also contains received data
struct CmdData {
  const char *name;
  int32_t data;
} commands[] = {
  {.name = "color"},
  {.name = "color/R"},
  {.name = "color/G"},
  {.name = "color/B"},
  {.name = "light/IR"},
  {.name = "light/lumen"},
  {.name = "ntc"},
  {.name = "ntc/R"},
  {.name = "temperature"},
  {.name = "temperature/F"},
  {.name = "humidity"},
  {.name = "pressure/mbar"},
  {.name = "pressure/psi"},
  {.name = "altitude"},
};
// Read stored data from commands list
int read(const char *name) {
  for (CmdData cmd : commands) {
    if (strcmp(cmd.name, name) == 0) {
      return cmd.data;
    }
  }
  return -1;
}
// When data from module is received, store it to commands list
void onModuleData(ModuleData data) {
  // Iterate trough commands list
  for (CmdData &cmd : commands) {
    // Find command match in the list
    if (data.getHashCmd() == TotemBUS::hash(cmd.name)) {
      // Set data of received command
      cmd.data = data.getInt();
      dataUpdated = true;
      break;
    }
  }
  if (data.is("color")) {
    Totem.X4.write("rgbAll", data.getInt());
  }
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  // Start X4 without Bluetooth advertisement
  Totem.X4.beginNoBluetooth();
  // Register for module connection event
  Totem.X4.attachOnModuleConnected(onModuleConnected); // Register function "onModuleConnected"
  // Register function to receive module subscriptions
  module.attachOnData(onModuleData);
}

void loop() {
  // When set to true, subscribe all commands from the list
  if (sendSubscribe) {
    sendSubscribe = false;
    for (CmdData cmd : commands) {
      // Subscribe update every 100ms
      module.subscribe(cmd.name, 100);
      delay(1); // Workaround for Totem Library v1.0.5
    }
    // Disable LED for more precise color reading
    module.write("led", false);
  }
  // Only print data if it has been updated in onModuleData event
  if (!dataUpdated)
    return; // exit loop() function
  // Print data stored in command list
  Serial.print("LIGHT: ");
  Serial.printf("Color: %3d %3d %3d",
    (read("color") >> 16) & 0xFF,
    (read("color") >> 8) & 0xFF,
    (read("color") >> 0) & 0xFF
  );
  Serial.printf(" Precise R: %5d G: %5d B: %5d | ", read("color/R"), read("color/G"), read("color/B"));
  Serial.printf("Infrared %5d Luminosity: %5d\n", read("light/IR"), read("light/lumen"));
  Serial.print("HUMID: ");
  Serial.printf("Temp: %.2fC %.2fF Humidity: %3d%% | ", 
    ((float)read("temperature"))/100, ((float)read("temperature/F"))/100, read("humidity"));
  Serial.printf("NTC Temp: %.2fC NTC resistance: %5dR\n", ((float)read("ntc"))/100, read("ntc/R"));
  Serial.print("BARO:  ");
  Serial.printf("Pressure: %.3fBar %2dpsi Altitude: %3dm\n", 
    ((float)read("pressure/mbar"))/1000, read("pressure/psi"), read("altitude"));
  dataUpdated = false;
  // Delay printing for 200ms
  delay(200);
}
