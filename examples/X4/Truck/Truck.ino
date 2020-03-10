/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to connect and control Totem Truck over Bluetooth Low Energy (BLE).
 * This code should run on ESP32 MCU.
 * 
 * INSTRUCTIONS:
 * 1. Compile and upload sketch to external ESP32 board.
 * 4. Power on Totem Truck
 * 5. In few seconds a connection should be established and Totem Truck start to move.
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
    void move(int drivePower, int brakePower) {
        driver.move(drivePower);
        driver.brakeAll(brakePower);
        // Light up leds according to drive and braking parameters
        if (drivePower < 0)
            setLed(brakePower != 0 ? ReverseBrake : Reverse);
        else
            setLed(brakePower != 0 ? Brake : Idle);
    }
    // Steer wheels
    void steer(int position) {
        driver.moveServo(0, position);
    }
    // Blink leds
    void indicateConnected() {
        setLed(Connected);
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
        Connected,
        Brake,
        Reverse,
        ReverseBrake
    };
    // Set led color state
    void setLed(LedState state) {
        static LedState lastLedState = Idle;
        if (state == lastLedState) return;
        switch (state) {
            case Idle:
                driver.getModule().write("rgbAll/reset");
                break;
            case Connected:
                for (int i=0; i<7; i++) {
                    driver.getModule().write("rgbAll", 255, 00, 255, 00); // Set to green
                    delay(100);
                    driver.getModule().write("rgbAll", 0); // Off leds
                }
                driver.getModule().write("rgbAll/reset");
                state = Idle;
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
    Totem.BLE.begin(); // Start Bluetooth Low Energy interface
    Serial.println("Searching for Totem robot...");
    TotemRobot robot = Totem.BLE.findRobot(); // Wait until connected to first found Totem robot
    // Print connected robot name
    Serial.print("Connected to: ");
    Serial.println(robot.getName());
    // Print connected robot battery
    truck.printBattery();
    // Proceed to loop
}

void loop() {
    // put your main code here, to run repeatedly:
    truck.steer(-100); // Steer left to max angle
    delay(500); // Wait 500ms
    truck.move(30, 0); // Drive forward at 30% power
    delay(1000);
    truck.move(0, 100); // Stop drive and brake
    delay(500);
    truck.steer(100); // Steer right to max angle
    delay(500);
    truck.move(-30, 0); // Drive backward at 30% power
    delay(1000);
    truck.move(0, 100); // Stop drive and brake
    delay(500);
}