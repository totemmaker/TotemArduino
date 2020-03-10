/* 
 * This file is part of TotemArduino library (https://github.com/totemmaker/TotemArduino).
 * 
 * Copyright (c) 2020 TotemMaker.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */
#ifndef LIB_TOTEM_SRC_UTILS_MOTORDRIVER
#define LIB_TOTEM_SRC_UTILS_MOTORDRIVER

#include "TotemModule.h"

class MotorDriver {
    enum Names {
        FL, FR, RL, RR
    };
public:
    struct Motor {
        uint32_t cmdHashPower;
        uint32_t cmdHashBrake;
        uint8_t minPower;
        uint8_t maxPower;
        bool invert;
        int8_t power;
        int8_t brake;
        int8_t powerComputed;
        int8_t brakeComputed;
    };
    struct Servo {
        uint32_t cmdHash;
        int8_t minPos;
        int8_t centerPos;
        int8_t maxPos;
        bool invert;
        int8_t posComputed;
    };
    MotorDriver(bool singleCommandUpdate = true) : singleCommand(singleCommandUpdate), module(0) { }
    // Set sensitivity of robot turning [0 - small; 100 - high]
    void setTurnIntensity(int intensity) {
        turnIntensity = constrain(intensity, 0, 100);
    }
    // Configure front left wheel motor
    void addFrontLeft(const char *command, int minPower, int maxPower, bool inverted = false) { 
        setABCDChannel(command, motors[FL]);
        motors[FL].minPower = minPower;
        motors[FL].maxPower = maxPower;
        motors[FL].invert = inverted;
    }
    // Configure front right wheel motor
    void addFrontRight(const char *command, int minPower, int maxPower, bool inverted = false) { 
        setABCDChannel(command, motors[FR]);
        motors[FR].minPower = minPower;
        motors[FR].maxPower = maxPower;
        motors[FR].invert = inverted;
    }
    // Configure rear left wheel motor
    void addRearLeft(const char *command, int minPower, int maxPower, bool inverted = false) { 
        setABCDChannel(command, motors[RL]);
        motors[RL].minPower = minPower;
        motors[RL].maxPower = maxPower;
        motors[RL].invert = inverted;
    }
    // Configure rear right wheel motor
    void addRearRight(const char *command, int minPower, int maxPower, bool inverted = false) { 
        setABCDChannel(command, motors[RR]);
        motors[RR].minPower = minPower;
        motors[RR].maxPower = maxPower;
        motors[RR].invert = inverted;
    }
    // Configure servo motor
    void addServo(size_t ch, const char *command, int minPos, int centerPos, int maxPos, bool inverted = false) {
        if (ch > 2) return;
        servos[ch].cmdHash = TotemBUS::hash(command);
        servos[ch].minPos = minPos;
        servos[ch].centerPos = centerPos;
        servos[ch].maxPos = maxPos;
        servos[ch].invert = inverted;
    }
    // Set servo arm to position
    void moveServo(size_t ch, int position) {
        if (ch > 2) return;
        if (servos[ch].cmdHash == 0) return;
        updateServo(servos[ch], constrain(position, -100, 100));
    }
    // Set motor move parameters
    void move(int drive, int turn = 0) {
        // Validate input values range
        turn = constrain(turn, -100, 100);
        drive = constrain(drive, -100, 100);
        // Calculate turn intensity
        int intensity = turnIntensity + ((100-turnIntensity)*abs(drive)/100);
        turn = (turn*intensity)/100;
        // Calculate motors power
        motors[FL].power = constrain(drive+turn, -100, 100);
        motors[FR].power = constrain(drive-turn, -100, 100);
        motors[RL].power = motors[FL].power;
        motors[RR].power = motors[FR].power;
        // Update motors
        updateMotor();
    }
    // Brake individual wheels
    void brake(int fl, int fr, int rl, int rr) {
        // Set brake values
        motors[FL].brake = constrain(fl, 0, 100);
        motors[FR].brake = constrain(fr, 0, 100);
        motors[RL].brake = constrain(rl, 0, 100);
        motors[RR].brake = constrain(rr, 0, 100);
        // Update motors
        updateMotor();
    }
    // Brake all wheels
    void brakeAll(int power) {
        brake(power, power, power, power);
    }
    // Brake rear wheels
    void brakeRear(int power) {
        brake(motors[FL].brake, motors[FR].brake, power, power);
    }
    // Brake front wheels
    void brakeFront(int power) {
        brake(power, power, motors[RL].brake, motors[RR].brake);
    }
    // Set to control specific module
    void setModule(int number, int serial = 0) {
        this->module.setNumber(number);
        this->module.setNumber(serial);
    }
    // Get TotemModule object
    TotemModule& getModule() {
        return module;
    }
private:
    bool singleCommand;
    TotemModule module;
    Motor motors[4] = {};
    Servo servos[3] = {};
    uint8_t turnIntensity = 100;
    int8_t dummyValue = 0;
    struct ABCDVal{
        int8_t *A, *B, *C, *D;
    } abcdPower = {&dummyValue, &dummyValue, &dummyValue, &dummyValue}, 
    abcdBrake = {&dummyValue, &dummyValue, &dummyValue, &dummyValue};

    void updateMotor() {
        bool powerChanged = false;
        bool brakeChanged = false;
        // Iterate all motors
        for (int m=0; m<4; m++) {
            // Skip motor if it's not configured
            if (!motors[m].cmdHashPower) continue;
            int8_t power = motors[m].power;
            int8_t brake = motors[m].brake;
            // If motor has power, apply computation
            if (motors[m].power != 0) {
                // Store condition if power is negative
                bool negative = motors[m].power < 0;
                // Get absolute power
                power = abs(motors[m].power);
                // Cut power if braking more than accelerating
                if (brake >= power) power = 0;
                // Lower power if braking less than accelerating
                else if (brake != 0)  {
                    power -= brake;
                    brake = 0;
                }
                // Check if motor still has power
                if (power != 0) {
                    // Map power to min max values
                    power = map(power, 1, 100, motors[m].minPower, motors[m].maxPower);
                    // Set motor direction
                    if (negative) power *= -1;
                    // Invert motor output
                    if (motors[m].invert) power *= -1;
                }
            }
            // Check if power differs from previous calculation
            if (power != motors[m].powerComputed) {
                powerChanged = true;
                // Save previous value
                motors[m].powerComputed = power;
                // Update motor right away
                if (!singleCommand) module.write(motors[m].cmdHashPower, motors[m].powerComputed);
            }
            // Check if braking power changed
            if (brake != motors[m].brakeComputed) {
                brakeChanged = true;
                // Save previous value
                motors[m].brakeComputed = brake;
                // Update motor right away
                if (!singleCommand) module.write(motors[m].cmdHashBrake, motors[m].brakeComputed);
            }
        }
        // Update motors with single command
        if (singleCommand && powerChanged) {
            module.write("motorABCD", *abcdPower.A, *abcdPower.B, *abcdPower.C, *abcdPower.D);
        }
        // Update motors with single command
        if (singleCommand && brakeChanged) {
            module.write("motorABCD/brake", *abcdBrake.A, *abcdBrake.B, *abcdBrake.C, *abcdBrake.D);
        }
    }

    void updateServo(Servo &servo, int position) {
        // Invert motor input
        if (servo.invert) position *= -1;
        // Get center position
        int posComputed = servo.centerPos;
        // Compute negative value
        if (position < 0) {
            posComputed = map(position, -100, -1, servo.minPos, servo.centerPos-1);
        }
        // Compute positive value
        else if (position > 0) {
            posComputed = map(position, 1, 100, servo.centerPos+1, servo.maxPos);
        }
        // Check if value differs from previous calculation and requires update
        if (servo.posComputed != posComputed) {
            module.write(servo.cmdHash, posComputed);
            servo.posComputed = posComputed;
        }
    }

    void setABCDChannel(const char *command, Motor &motor) {
        // Set commands
        motor.cmdHashPower = TotemBUS::hash(command);
        motor.cmdHashBrake = 0;
        // Configure channels if belongs to "motorABCD" command
        switch (motor.cmdHashPower) {
            case 0xaba01c49: //"motorA"
            abcdPower.A = &motor.powerComputed;
            abcdBrake.A = &motor.brakeComputed;
            motor.cmdHashBrake = 0x9a486d7d; //"motorA/brake"
            break;
            case 0xa8a01790: //"motorB"
            abcdPower.B = &motor.powerComputed;
            abcdBrake.B = &motor.brakeComputed;
            motor.cmdHashBrake = 0x03ee3018; //"motorB/brake"
            break;
            case 0xa9a01923: //"motorC"
            abcdPower.C = &motor.powerComputed;
            abcdBrake.C = &motor.brakeComputed;
            motor.cmdHashBrake = 0xa984c117; //"motorC/brake"
            break;
            case 0xaea02102: //"motorD"
            abcdPower.D = &motor.powerComputed;
            abcdBrake.D = &motor.brakeComputed;
            motor.cmdHashBrake = 0x64a0ec5a; //"motorD/brake"
            break;
            default:
                // Set to individual motor update if used other command
                singleCommand = false;
        }
    }
};

#endif /* LIB_TOTEM_SRC_UTILS_MOTORDRIVER */
