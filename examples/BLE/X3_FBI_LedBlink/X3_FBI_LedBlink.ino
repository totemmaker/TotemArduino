/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to connect and control Totem X3-FBI board over Bluetooth Low Energy (BLE).
 * This code should run on ESP32 MCU.
 * 
 * INSTRUCTIONS:
 * 0. Make sure X3-FBI board is updated to firmware version 1.2 or later (using Totem App)
 * 1. Compile and upload sketch
 * 3. Open Serial monitor at speed 115200 (For PlatformIO add "monitor_speed = 115200" in platformio.ini file)
 * 4. Power on X3-FBI board
 * 5. In few seconds a connection should be established and ledbar start to flash
 */
// Totem Library include
#include <Totem.h>
// Define object that will be used to access X3-FBI board.
// It will become active when BLE connection is established.
TotemModule module(00);
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
}

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
    module.subscribeWait("battery");
    // Proceed to loop
}
// Variable holding state of ledbar. It will show binary representation of decimal value
int leds;

void loop() {
    // put your main code here, to run repeatedly:
    // Increment value and display binary representation on ledbar
    leds = (leds + 1) % 0b111111111111;
    // Generate random sequence for ledbar (uncomment to make it active)
    // leds = random(0b111111111111);
    module.write("leds", leds); // Set sequence for X3-FBI ledbar
    delay(100); // Wait 100 milliseconds
}