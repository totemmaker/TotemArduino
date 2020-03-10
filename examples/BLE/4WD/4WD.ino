/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to connect and control Totem 4WD over Bluetooth Low Energy (BLE).
 * https://totemmaker.net/product/diy-smartphone-controlled-4wd-motor-wheel-kit/
 * This code should run on ESP32 MCU.
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
class Totem_4WD {
    // Helper object to control driving logic
    MotorDriver driver;
public:
    Totem_4WD() {
        // Configure MotorDriver to setup robot wheels
        driver.addFrontLeft("motorA", 10, 100, true);
        driver.addFrontRight("motorB", 10, 100);
        driver.addFrontLeft("motorC", 10, 100, true);
        driver.addFrontRight("motorD", 10, 100);
        driver.setTurnIntensity(100);
    }
    // Set X3-FBI light bar [each bit represents a led]
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
} totemWD;

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
    totemWD.printBattery();
    // Proceed to loop
}

void loop() {
    // put your main code here, to run repeatedly:
    totemWD.move(60, 0); // Drive forward at 60% power
    totemWD.setLightBar(0b111100000000); // Set led bar
    delay(1000); // Wait 1 second
    totemWD.move(0, 80); // Turn right at 80% power
    totemWD.setLightBar(0b111111111111); // Set led bar
    delay(200); // Wait 200 milliseconds
}