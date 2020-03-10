/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to connect and control Totem Gripper Bot https://totemmaker.net/product/gripper-bot-smartphone-app-controlled-car/
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
// Gripper Bot control implementation
class GripperBot {
    // Helper object to control driving logic
    MotorDriver driver;
public:
    int gripperPos = 0;
    GripperBot() {
        // Configure MotorDriver to setup Gripper Bot motors
        driver.addFrontLeft("motorC", 9, 100, true);
        driver.addFrontRight("motorD", 9, 100);
        driver.setTurnIntensity(70);
        driver.addServo(0, "servoA", 10, 55, 94); // Gripper
        driver.addServo(1, "servoB", -70, 50, 90, true); // Arm
    }
    // Set Gripper Bot Arm possition. [-100 - fully retracted, 100 - fully extended]
    void setArm(int position) {
        driver.moveServo(1, position);
        // Set light bar according to arm position
        position = constrain(position, -100, 100);
        position = map(position, -100, 100, 0, 11);
        setLightBar((1<<position));
    }
    // Set Gripper Bot Gripper possition. [-100 - fully open, 100 - fully closed]
    void setGripper(int position) {
        driver.moveServo(0, position);
        gripperPos = constrain(position, -100, 100);
    }
    // Set Gripper Bot light bar [each bit represents a led]
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
        else Serial.println(" (LOW) (charge Gripper Bot)");
    }
} gripperBot;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Setup Totem Library
    Totem.BLE.begin(); // Start Bluetooth Low Energy interface
    // Find robot
    TotemRobot robot = Totem.BLE.findRobot();
    // Print robot name
    Serial.print("Connected to: ");
    Serial.println(robot.getName());
    // Reset led bar on X3-FBI board
    gripperBot.setLightBar(0);
    // Display battery level
    gripperBot.printBattery();
    // Open gripper
    gripperBot.setGripper(-60);
    // Proceed to loop
}

void loop() {
    // Control Totem Gripper Bot
    gripperBot.move(0, 0); // Stop
    delay(300);
    gripperBot.setArm(90); // Extend arm
    delay(500);
    gripperBot.setGripper(100); // Close gripper
    delay(500);
    gripperBot.setArm(-60); // Retract arm
    delay(500);
    gripperBot.move(0, 70); // Turn right
    delay(700);
    gripperBot.move(0, 0); // Stop
    delay(300);
    gripperBot.setArm(90); // Extend arm
    delay(500);
    gripperBot.setGripper(-60); // Open gripper
    delay(500);
    gripperBot.setArm(-60); // Retract arm
    delay(500);
    gripperBot.move(0, 70); // Turn right
    delay(700);
}