/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example code to show basic functionality of Totem X4 board.
 * 
 * INSTRUCTIONS:
 * 1. Random colors are traveling over leds.
 * 2. Pres on-board button to hold leds on random color.
 */
// Totem Library include
#include <Totem.h>
// Structure to hold generated color values
struct Color {
    int A, R, G, B;
};
// Function to generate random color
Color getRandomColor() {
    Color color;
    color.A = 255; // Set maximum brightness (Alpha)
    color.R = random(255); // Get random red color
    color.G = random(255); // Get random green color
    color.B = random(255); // Get random blue color
    return color;
}
// Function to update RGB leds with provided color array
void updateLeds(Color colors[4]) {
    Totem.X4.write("rgbA", colors[0].A, colors[0].R, colors[0].G, colors[0].B);
    Totem.X4.write("rgbB", colors[1].A, colors[1].R, colors[1].G, colors[1].B);
    Totem.X4.write("rgbC", colors[2].A, colors[2].R, colors[2].G, colors[2].B);
    Totem.X4.write("rgbD", colors[3].A, colors[3].R, colors[3].G, colors[3].B);
}
// Function called on module event
void onModuleData(ModuleData data) {
    // Check if received button event and it's pressed
    if (data.is("button") && data.getInt()) {
        // Update all leds with random color
        Color color = getRandomColor();
        Totem.X4.write("rgbAll", color.A, color.R, color.G, color.B);
    }
}
// Array holding current color of all leds
Color colors[4];
// Arduino setup function.
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Initialize X4 module
    Totem.X4.begin();
    // Register module data receiver function "onModuleData"
    Totem.X4.attachOnData(onModuleData);
    // Subscribe to receive button events
    Totem.X4.subscribe("button");
    // Print startup message
    Serial.println("X4 board started");
}
// Arduino loop function
void loop() {
    // Skip leds updating while button is pressed
    if (Totem.X4.readWait("button").getInt())
        return;
    // Shift all colors in array forward
    for (int i=3; i>0; i--) {
        colors[i] = colors[i-1];
    }
    // Insert a new color
    colors[0] = getRandomColor();
    // Update leds with new colors
    updateLeds(colors);
    // Repeat each 100 milliseconds
    delay(100);
}
