/*
 * Copyright (c) 2021 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example code to show functionality of Totem Distance board (number 11).
 * Module will light it's LED accordingly to distance.
 * Red - very close.
 * Green - far away.
 * Spinning - no object detected in distance.
 * 
 * INSTRUCTIONS:
 * 1. Connect Totem Distance board to X4 over TotemBUS connector.
 * 2. Upload code to X4.
 */
// Totem Library include
#include <Totem.h>
// Dinstance sensor module instance (number 11)
TotemModule sensor(11);
// Function called on module connect (over TotemBUS)
void onModuleConnect(uint16_t number, uint16_t serial) {
    // Print connected module information
    Serial.print("Module connected. Number: ");
    Serial.print(number);
    Serial.print(" Serial: ");
    Serial.println(serial);
}
// Hold current sensor distance
int distance = 0;
// Function called on module event
void onModuleData(ModuleData data) {
    // Check if sensor sent distance update
    if (data.is("distance")) {
        // Get value
        distance = data.getInt();
    }
}
// Arduino setup function.
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Initialize X4 module
    Totem.X4.begin();
    // Register function to receive events when module is connected
    Totem.X4.attachOnModuleConnected(onModuleConnect);
    // Register sensor data receiver function "onModuleData"
    sensor.attachOnData(onModuleData);
    // Subscribe to distance command
    // This will start to broadcast distance at maximum rate.
    sensor.subscribe("distance");
}
// Arduino loop function
void loop() {
    // Process if new distance value received
    if (distance) {
        // Check if sensor does not detect anything
        if (distance == 2000) {
            // Play LED spinning animation
            static int ledNum = 0;
            sensor.write("rgbAll", 0, 0, 0); // Turn off all LED
            sensor.write("rgbX", ledNum, 0, 255, 0); // Turn on next LED
            ledNum = (ledNum + 1) % 8; // Iterate between LED numbers (0-7)
            delay(50); // Wait 50ms between LED switch
        }
        // Sensor detected something
        else {
            // Compute color output according to dinstance parameter
            // Led color starts changing if distance is less than 500mm.
            uint16_t color = map(constrain(distance, 20, 500), 20, 500, 0, 510);
            distance = 0; // Reset distance value and wait for next update
            uint8_t r = (color < 255) ? 255-color : 0; // Set Red color
            uint8_t g = (color > 255) ? color-255 : 0; // Set Green color
            sensor.write("rgbAll", r, g, 0); // Write sensor LED
        }
    }
}