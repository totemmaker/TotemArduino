/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to connect and control Totem X4 board over Bluetooth Low Energy (BLE).
 * This code should run on MCU that supports BLE feature. For example - ESP32.
 * 
 * INSTRUCTIONS:
 * 1. Compile and upload sketch to external ESP32 board.
 * 3. Open Serial monitor at speed 115200 (For PlatformIO add "monitor_speed = 115200" in platformio.ini file)
 * 4. Power on X4 board
 * 5. In few seconds a connection should be established and RGB leds start to flash
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
void updateLeds(TotemModule &module, Color colors[4]) {
    module.write("rgbA", colors[0].A, colors[0].R, colors[0].G, colors[0].B);
    module.write("rgbB", colors[1].A, colors[1].R, colors[1].G, colors[1].B);
    module.write("rgbC", colors[2].A, colors[2].R, colors[2].G, colors[2].B);
    module.write("rgbD", colors[3].A, colors[3].R, colors[3].G, colors[3].B);
}
// Define object that will be used to access X3-FBI board.
// It will become active when BLE connection is established.
TotemModule module(04);
// Array holding current color of all leds
Color colors[4];
// on-board button state
bool buttonIsPressed = false;
// Event function, called when data arrives for subscribed command
void onModuleData(ModuleData data) {
    // Check if received "battery" command
    if (data.is("battery")) {
        // Print X3-FBI board battery voltage
        Serial.print("Battery voltage: ");
        Serial.print(data.getInt()/1000);
        Serial.print(".");
        Serial.print(data.getInt()%1000);
        Serial.println("V");
    }
    // Check if received button event and it's pressed
    else if (data.is("button") && data.getInt()) {
        Color color = getRandomColor();
        module.write("rgbAll", color.A, color.R, color.G, color.B);
        buttonIsPressed = true;
    }
    else {
        buttonIsPressed = false;
    }
}
// Arduino setup function.
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Totem.BLE.begin(); // Start Bluetooth Low Energy interface
    Serial.println("Searching for Totem board...");
    // Start scanning for X3-FBI board. It's representing a Totem robot.
    TotemRobot robot = Totem.BLE.findRobot(); // Wait until connected to first found Totem robot
    // Print connected robot name
    Serial.print("Connected to: ");
    Serial.println(robot.getName());
    // Subscribe to battery event
    module.attachOnData(onModuleData);
    module.read("battery");
    module.subscribe("button");
    // Proceed to loop
}
// Arduino loop function
void loop() {
    // Skip leds updating while button is pressed
    if (buttonIsPressed)
        return;
    // Shift all colors in array forward
    for (int i=3; i>0; i--) {
        colors[i] = colors[i-1];
    }
    // Insert a new color
    colors[0] = getRandomColor();
    // Update leds with new colors
    updateLeds(module, colors);
    // Repeat each 100 milliseconds
    delay(100);
}