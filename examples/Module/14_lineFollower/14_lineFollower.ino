/*
 * Copyright (c) 2021 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example code to show functionality of Totem Line Follower module (number 14).
 * Module will show line position with additional data
 * 
 * INSTRUCTIONS:
 * 1. Connect Totem Line Follower module to X4 over TotemBUS connector.
 * 2. Upload code to X4.
 */
// Totem Library include
#include "Totem.h"
// Define sensor module object
TotemModule module(14);
// When set to true will print updated values
bool printData = false;
// Will receive event of each module connection and ping response
void onModuleConnected(uint16_t number, uint16_t serial) {
  // Print connected module information
  Serial.print("Module connected. Number: ");
  Serial.print(number);
  Serial.print(" Serial: ");
  Serial.println(serial);
  // If module is connected prepare to send subscriptions
  if (number == module.getNumber()) {
    module.subscribe("line/pos");
    module.subscribe("line/color");
    module.subscribe("line/junction");
    module.subscribe("sensor/range", 50);
    module.subscribe("sensor/raw", 50);
  }
}
// Prepare variables to store data from module
char lineColor;
int linePos;
int lineJunction;
int sensorRange;
byte sensorRaw[8];
// When data from module is received, store it to commands list
void onModuleData(ModuleData data) {
  if (data.is("line/pos")) { // Get data of line position [-35:35]
    linePos = data.getInt();
    printData = true;
  }
  if (data.is("line/color")) { // Get line color [0;W;B]
    lineColor = data.getInt();
  }
  if (data.is("line/junction")) { // Get junction detected state [0:1]
    lineJunction = data.getInt();
  }
  if (data.is("sensor/range")) { // Get sensor detection range [0:100]
    sensorRange = data.getInt();
  }
  if (data.is("sensor/raw")) { // Get array of raw measurements [0:100]
    byte *rawArray; // Points to 8 byte array received from module
    data.getData(rawArray); // Get pointer to received data
    // Copy data in rawArray to sensorRaw
    memcpy(sensorRaw, rawArray, 8);
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
  // Print data stored in command list
  if (printData) {
    printData = false;
    Serial.printf("Position %3d Color: %c, Junction: %d, Range: %2d, Raw: ", linePos, lineColor, lineJunction, sensorRange);
    for (int i=0; i<8; i++) {
      Serial.printf("%4d", sensorRaw[i]);
    }
    Serial.println();
  }
}