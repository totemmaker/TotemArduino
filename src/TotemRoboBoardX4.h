/* 
 * Copyright 2024 Totem Technology, UAB
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_MODULE_TOTEM_ROBOBOARD_X4
#define LIB_MODULE_TOTEM_ROBOBOARD_X4

#ifndef ESP_PLATFORM
#pragma GCC error "TotemRoboBoardX4.h is only supported in ESP32 boards"
#endif

#include "private/ble/totem-ble-control-board.h"

class TotemRoboBoardX4 : public _Totem::BLE::TotemBLEControlBoard {
public:
    TotemRoboBoardX4() : _Totem::BLE::TotemBLEControlBoard(0x04) { }

    /// @brief Get motor driver firmware version
    /// @return version string
    String getDriverVersion() {
        uint32_t version = ble.cmdReadValue("driver/version");
        static char buffer[5];
        buffer[0] = '0'+ (version / 100 % 10);
        buffer[1] = '.';
        buffer[2] = '0'+ (version / 10 % 10);
        buffer[3] = '0'+ (version % 10);
        buffer[4] = '\0';
        return String(buffer);
    }

    /// @brief Spin servo C
    /// @param pos [-100:100]% position. [0] center
    void servoSpinC(int pos) { ble.cmdWrite("servoC", pos); }
    /// @brief Spin all servo motors with single command
    /// @param motorA [-100:100]% position. [0] center
    /// @param motorB [-100:100]% position. [0] center
    /// @param motorC [-100:100]% position. [0] center
    void servoSpinABC(int motorA, int motorB, int motorC) { ble.cmdWrite("servoABC", ((uint8_t)motorA)<<24|((uint8_t)motorB)<<16|(((uint8_t)motorC)<<8)); }

    /// @brief Turn LED
    /// @param state [0] off, [1] on
    void setLED(bool state) { ble.cmdWrite("led", state?1:0); }
    /// @brief Read button (BUTTON) state
    /// @return [true] is pressed, [false] not pressed
    bool getButton() { return ble.cmdReadValue("button"); }

    /// @brief Send function command A
    /// @param value [0:0xFFFFFFFF]
    void functionA(int value) { ble.cmdWrite("functionA", value); }
    /// @brief Send function command B
    /// @param value [0:0xFFFFFFFF]
    void functionB(int value) { ble.cmdWrite("functionB", value); }
    /// @brief Send function command C
    /// @param value [0:0xFFFFFFFF]
    void functionC(int value) { ble.cmdWrite("functionC", value); }
    /// @brief Send function command D
    /// @param value [0:0xFFFFFFFF]
    void functionD(int value) { ble.cmdWrite("functionD", value); }

    /// @brief Send 32-bit value to remote board
    /// @param id identifier
    /// @param value 32-bit value
    /// @return [true] success, [false] error
    bool sendValue(int id, int value) { return ble.cmdSendValue(id, value); }
    /// @brief Send data array to remote board
    /// @param id identifier
    /// @param data data array
    /// @param len data length
    /// @return [true] success, [false] error
    bool sendString(int id, const void *data, uint32_t len) { return ble.cmdSendString(id, (const char*)data, len); }
    /// @brief Send string (text) to remote board
    /// @param id identifier
    /// @param string String object
    /// @return [true] success, [false] error
    bool sendString(int id, String string) { return ble.cmdSendString(id, string.c_str(), string.length()); }

    /// @brief Read 32-bit value from remote board
    /// @param id identifier
    /// @return [0:0xFFFFFFFF] value
    int readValue(uint32_t id) { return ble.cmdRequestValue(id); }
    /// @brief Read string (text) from remote board
    /// @param id identifier
    /// @return String object
    String readString(uint32_t id) { return ble.cmdRequestString(id); }

    /// @brief Register event to intercept value sent from remote board
    /// @param onValue void onValue(int id, int value)
    void addOnReceive(void (*onValue)(int id, int value)) { ble.addOnValue(onValue); }
    /// @brief Register event to intercept value sent from remote board with "arg" pointer
    /// @param onValue void onValue(int id, int value, void *arg)
    /// @param arg pointer passed to function
    void addOnReceive(void (*onValue)(int id, int value, void *arg), void *arg) { ble.addOnValueArg(onValue, arg); }
    /// @brief Register event to intercept string sent from remote board
    /// @param onString void onString(int id, String string)
    void addOnReceive(void (*onString)(int id, String string)) { ble.addOnString(onString); }
    /// @brief Register event to intercept string sent from remote board with "arg" pointer
    /// @param onString void onString(int id, String string, void *arg)
    /// @param arg pointer passed to function
    void addOnReceive(void (*onString)(int id, String string, void *arg), void *arg) { ble.addOnStringArg(onString, arg); }
};


#endif /* LIB_TOTMODULE_EM_ROBOBOARDX4 _*/
