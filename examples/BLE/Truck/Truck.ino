/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to control Totem Truck with a program running inside X4 board.
 * 
 * INSTRUCTIONS:
 * 1. Compile and upload sketch to Truck's X4 board.
 */
// Totem Library include
#include <Totem.h>
// Totem Truck control implementation
class Truck {
    // Helper object to control driving logic
    MotorDriver driver;
public:
    Truck() {
        // Configure MotorDriver to setup Truck wheels
        driver.addRearLeft("motorB", 58, 100);
        driver.addRearRight("motorA", 58, 100, true);
        driver.addServo(0, "servoA", -30, 0, 50, true);
    }
    // Set drive and brake parameters
    void drive(int drivePower) {
        driver.brakeAll(0);
        driver.move(drivePower);
        // Light up leds according to drive parameters
        setLed(drivePower < 0 ? Reverse : Idle);
    }
    void brake(int brakePower = 100) {
        driver.brakeAll(brakePower);
        // Light up leds according to braking parameters
        setLed(lastLedState == Reverse ? ReverseBrake : Brake);
    }
    // Steer wheels
    void steer(int position) {
        driver.moveServo(0, position);
    }
    // Print battery voltage
    void printBattery() {
        int volt = driver.getModule().readWait("battery").getInt();
        // Display battery level
        Serial.print("Battery: ");
        if (volt == 0) {
            Serial.println("Not connected");
            return;
        }
        Serial.print(volt/1000);
        Serial.print(".");
        Serial.print(volt%1000);
        Serial.print("V");
        if (volt > 12000) Serial.println(" (FULL)");
        else if (volt > 10000) Serial.println(" (OK)");
        else Serial.println(" (LOW) (charge Truck)");
    }
private:
    enum LedState {
        Idle,
        Brake,
        Reverse,
        ReverseBrake
    };
    LedState lastLedState = Idle;
    // Set led color state
    void setLed(LedState state) {
        if (state == lastLedState) return;
        switch (state) {
            case Idle:
                driver.getModule().write("rgbAll/reset");
                break;
            case Brake:
                driver.getModule().write("rgbAll", 255, 255, 0, 0); // Set to red
                break;
            case Reverse:
                driver.getModule().write("rgbAll", 255, 255, 255, 255); // Set to white
                break;
            case ReverseBrake:
                driver.getModule().write("rgbA", 255, 255, 255, 255); // Set led A to white
                driver.getModule().write("rgbB", 255, 255, 0, 0); // Set led B to red
                driver.getModule().write("rgbC", 255, 255, 0, 0); // Set led C to red
                driver.getModule().write("rgbD", 255, 255, 255, 255); // Set led D to white
                break;
        }
        lastLedState = state;
    }
} truck;

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
    truck.printBattery();
    // Proceed to loop
}

void loop() {
    // put your main code here, to run repeatedly:
    truck.steer(-100); // Steer left to max angle
    delay(500); // Wait 500ms
    truck.drive(30); // Drive forward at 30% power
    delay(1000);
    truck.brake(); // Stop drive and brake
    delay(500);
    truck.steer(100); // Steer right to max angle
    delay(500);
    truck.drive(-30); // Drive backward at 30% power
    delay(1000);
    truck.brake(); // Stop drive and brake
    delay(500);
}