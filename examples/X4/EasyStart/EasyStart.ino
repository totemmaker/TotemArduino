/*
 * Copyright (c) 2021 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Code template to get started with X4 programming
 * 
 * INSTRUCTIONS:
 * 1. Connect X4 board to PC.
 * 2. Upload code.
 */
// Totem Library include
#include <Totem.h>
// Function called on module connect (over TotemBUS)
void onModuleConnect(uint16_t number, uint16_t serial) {
    // Print connected module information
    Serial.print("Module connected. Number: ");
    Serial.print(number);
    Serial.print(" Serial: ");
    Serial.println(serial);
}
// Function called on module event
void onModuleData(ModuleData data) {
    // Check if sensor sent distance update
    if (data.is("button")) {
        // Get button state
        if (data.getInt()) {
            Serial.println("Button is pressed");
        }
        else {
            Serial.println("Button is released");
        }
    }
}
// Arduino setup function.
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Initialize X4 module
    Totem.X4.begin(); // Select startup mode between "begin" and "beginNoBluetooth".
    //Totem.X4.beginNoBluetooth(); // Disable Bluetooth. Increase upload speed
    // Register function to receive events when module is connected
    Totem.X4.attachOnModuleConnected(onModuleConnect);
    // Register X4 data receiver function "onModuleData"
    Totem.X4.attachOnData(onModuleData);
    // Subscribe to button press event
    Totem.X4.subscribe("button");
}
// Arduino loop function
void loop() {
    // Light up LED at random color
    Totem.X4.write("rgbAll", 255, random(0, 255), random(0, 255), random(0, 255));
    // Wait 1 second
    delay(1000);
}