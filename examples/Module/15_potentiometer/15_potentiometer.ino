/*
 * Copyright (c) 2021 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example code to show functionality of Totem Potentiometer module (number 15).
 * Module will show knob position and button status
 * 
 * INSTRUCTIONS:
 * 1. Connect Totem Potentiometer module to X4 over TotemBUS connector.
 * 2. Upload code to X4.
 */
// Totem Library include
#include "Totem.h"
// Define sensor module object
TotemModule module(15);
// When set to true will send commands subscription to module
bool sendSubscribe = false;
// When set to true will print updated values
bool valueUpdated = false;
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
  {.name = "knobA"},
  {.name = "knobB"},
  {.name = "knobC"},
  {.name = "buttonA"},
  {.name = "buttonB"},
  {.name = "buttonC"},
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
      valueUpdated = true;
      break;
    }
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
      // Subscribe update on every value change
      module.subscribe(cmd.name);
      delay(1); // Workaround for Totem Library v1.0.5
    }
  }
  // Print data stored in command list
  if (valueUpdated) {
    valueUpdated = false;
    Serial.printf("Knob A: %3d B: %2d C: %3d, Button A: %d B: %d C: %d\n", 
      read("knobA"), read("knobB"), read("knobC"),
      read("buttonA"), read("buttonB"), read("buttonC")
    );
  }
}