/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to connect and control Totem Mini Trooper https://totemmaker.net/product/mini-trooper/
 * over Bluetooth Low Energy (BLE). This code should run on MCU that supports BLE feature (like ESP32).
 * 
 * INSTRUCTIONS:
 * 0. Make sure X3-FBI board is updated to the latest firmware (using Totem App)
 * 1. Select supported Board (ESP32 Dev Module)
 * 2. Compile and upload sketch
 * 3. Open Serial monitor at speed 115200 (For PlatformIO add "monitor_speed = 115200" to project platformio.ini file)
 * 4. Power on robot
 * 5. In a few seconds connection should be established and robot start to move
 */
// Totem Library include
#include <Totem.h>
// Mini Trooper control implementation
class MiniTrooper {
    // Helper object to control driving logic
    MotorDriver driver;
public:
    MiniTrooper() {
        // Configure MotorDriver to setup Mini Trooper wheels
        driver.addFrontLeft("motorA", 10, 100, true);
        driver.addFrontRight("motorB", 10, 100);
        driver.setTurnIntensity(50);
    }
    // Set Mini Trooper flipper possition. [0 - closed, 100 - fully open]
    void setFlipper(int position) {
        static int lastPossition = 0;
        // Only write value if it's changed
        if (lastPossition == position) return;
        driver.getModule().write("servoA", map(position, 0, 100, -100, 100));
        lastPossition = position;
    }
    // Set Mini Trooper light bar [each bit represents a led]
    void setLightBar(int ledBits) {
        static int lastBitSet = 0;
        // Only write value if it's changed
        if (lastBitSet == ledBits) return;
        driver.getModule().write("leds", ledBits);
        lastBitSet = ledBits;
    }
    // Set robot move parameters [drive: -100:100, turn: -100:100]
    void move(int drive, int turn) {
        // Set motor driver to move
        driver.move(drive, turn);
    }
    // Print battery voltage
    void printBattery() {
        int volt = driver.getModule().readWait("battery").getInt();
        // Display battery level
        Serial.print("Battery: ");
        Serial.print(volt/1000);
        Serial.print(".");
        Serial.print(volt%1000);
        Serial.print("V");
        if (volt > 3800) Serial.println(" (FULL)");
        else if (volt > 3100) Serial.println(" (OK)");
        else Serial.println(" (LOW) (charge Mini Trooper)");
    }
} miniTrooper;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Setup Totem Library
    Totem.BLE.begin(); // Start Bluetooth Low Energy interface
    Serial.println("Searching for Totem robot...");
    TotemRobot robot = Totem.BLE.findRobot(); // Wait until connected to first found Totem robot
    // Print connected robot name
    Serial.print("Connected to: ");
    Serial.println(robot.getName());
    // Print battery status
    miniTrooper.printBattery();
    // Proceed to loop
}

void loop() {
    // put your main code here, to run repeatedly:
    miniTrooper.move(60, 0); // Drive forward at 60% power
    miniTrooper.setFlipper(20);
    miniTrooper.setLightBar(0b111001100111); // Set led bar
    delay(1000); // Wait 1 second
    miniTrooper.move(0, 80); // Turn right at 80% power
    miniTrooper.setFlipper(0);
    miniTrooper.setLightBar(0b111000000000); // Set led bar
    delay(200); // Wait 200 milliseconds
}