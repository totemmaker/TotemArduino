/* 
 * Copyright 2024 Totem Technology, UAB
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_PRIVATE_BLE_TOTEM_BLE_CONTROL_BOARD
#define LIB_PRIVATE_BLE_TOTEM_BLE_CONTROL_BOARD

#include "totem-ble-module.h"

namespace _Totem::BLE {

class TotemBLEControlBoard {
    const uint8_t boardID;
protected:
    TotemBLEModule ble;
    TotemBLEControlBoard(int boardID) : boardID(boardID) { }
public:

    /// @brief Register connection state change event
    /// @param onConnectionChange void onConnectionChange()
    void addOnConnectionChange(void (*onConnectionChange)()) {
        ble.addOnConnectionChange(onConnectionChange);
    }
    /// @brief Register connection state change event with "arg" pointer
    /// @param onConnectionChange void onConnectionChange(void *arg)
    /// @param arg pointer passed to function
    void addOnConnectionChange(void (*onConnectionChange)(void*), void *arg) {
        ble.addOnConnectionChange(onConnectionChange, arg);
    }

    /// @brief Initiate Bluetooth connection to board
    /// @return [true] connected, [false] failed
    bool connect() { return ble.connectName(boardID, nullptr); }
    /// @brief Initiate Bluetooth connection to board. Filter by name
    /// @param name matching board name
    /// @return [true] connected, [false] failed
    bool connectName(const char *name) { return ble.connectName(boardID, name); }
    /// @brief Initiate Bluetooth connection to board. Filter by BLE address
    /// @param address matching BLE address
    /// @return [true] connected, [false] failed
    bool connectAddress(const char *address) { return ble.connectAddress(address); }
    /// @brief Is connection active
    /// @return [true] connected, [false] disconnected
    bool isConnected() { return ble.isConnected(); }
    /// @brief Disconnect
    void disconnect() { return ble.disconnect(); }
    /// @brief Get connected board BLE address
    /// @return BLE address
    String getAddress() { return ble.getAddress(); }

    /// @brief Restart board
    void restart() { ble.cmdWrite("restart"); }
    /// @brief Reset stored configuration
    void resetConfig() { ble.cmdWrite("cfg/reset"); }
    /// @brief Invert all DC motor ports (change is saved to memory)
    /// @param state [true] invert, [false] not inverted
    void setInvertDC(bool state) { ble.cmdWrite("cfg/motorABCD/invert", state?0x01010101:0x0); }
    /// @brief Brake all DC motors when stop (change is saved to memory)
    /// @param state [true] brake, [false] coast
    void setAutobrakeDC(bool state) { ble.cmdWrite("cfg/motorABCD/autobrake", state?0x01010101:0x0); }
    /// @brief Get if all DC motor ports are inverted
    /// @return [true] invert, [false] not inverted
    bool getInvertDC() { return ble.cmdReadValue("cfg/motorABCD/invert")!=0; }
    /// @brief Get if all DC motor autobrake is enabled
    /// @return [true] brake, [false] coast
    bool getAutobrakeDC() { return ble.cmdReadValue("cfg/motorABCD/autobrake")!=0; }

    /// @brief Change board name (change is saved to memory)
    /// @param name board discovery name (30 bytes max)
    void setName(const char *name) { ble.cmdWrite("cfg/robot/name", name); }
    /// @brief Change board initial color (change is saved to memory)
    /// @param hex [0:0xFFFFFF] HEX color
    void setColor(uint32_t hex) { ble.cmdWrite("cfg/robot/color", hex); }
    /// @brief Change board initial color (change is saved to memory)
    /// @param red [0:255] red
    /// @param green [0:255] green
    /// @param blue [0:255] blue
    void setColor(uint8_t red, uint8_t green, uint8_t blue) { setColor(red<<16|green<<8|blue); }
    /// @brief Assign type of robot board is installed in
    /// @param model [0:0xFFFF] identifier
    void setModel(uint16_t model) { ble.cmdWrite("cfg/robot/model", model); }
    /// @brief Get board name
    /// @return board name
    String getName() { return ble.cmdReadString("cfg/robot/name"); }
    /// @brief Get type of robot board is installed in
    /// @return 16-bit identifier
    uint16_t getModel() { return ble.cmdReadValue("cfg/robot/model"); }
    /// @brief Get board color
    /// @return [0:0xFFFFFF] HEX color
    uint32_t getColor() { return ble.cmdReadValue("cfg/robot/color"); }

    /// @brief Read battery voltage
    /// @return voltage in millivolts
    int getBattery() { return ble.cmdReadValue("battery"); }

    /// @brief Get firmware version
    /// @return version string
    String getVersion() {
        uint32_t version = ble.cmdReadValue("version");
        char buffer[20];
        if (boardID == 0x03) {
            snprintf(buffer, sizeof(buffer), "%d.%d", (int)(version/100), (int)(version%100));
        }
        else {
            snprintf(buffer, sizeof(buffer), "%d.%d.%d-totem.%d",
                (int)((version >> 24) & 0xFF), (int)((version >> 16) & 0xFF),
                (int)((version >> 8) & 0xFF), (int)((version >> 0) & 0xFF)
            );
        }
        return String(buffer);
    }
    /// @brief Get board revision
    /// @return revision string
    String getRevision() {
        uint32_t revision = ble.cmdReadValue("revision");
        if (boardID == 0x03) revision /= 10;
        char buffer[4];
        buffer[0] = '0'+ (revision / 10);
        buffer[1] = '.';
        buffer[2] = '0'+ (revision % 10);
        buffer[3] = '\0';
        return String(buffer);
    }

    /// @brief Spin servo A
    /// @param pos [-100:100]% position. [0] center
    void servoSpinA(int pos) { ble.cmdWrite("servoA", pos); }
    /// @brief Spin servo B
    /// @param pos [-100:100]% position. [0] center
    void servoSpinB(int pos) { ble.cmdWrite("servoB", pos); }

    /// @brief Spin DC motor A
    /// @param power [-100:100]% power. [0] stop
    void dcSpinA(int power) { ble.cmdWrite("motorA", power); }
    /// @brief Spin DC motor B
    /// @param power [-100:100]% power. [0] stop
    void dcSpinB(int power) { ble.cmdWrite("motorB", power); }
    /// @brief Spin DC motor C
    /// @param power [-100:100]% power. [0] stop
    void dcSpinC(int power) { ble.cmdWrite("motorC", power); }
    /// @brief Spin DC motor D
    /// @param power [-100:100]% power. [0] stop
    void dcSpinD(int power) { ble.cmdWrite("motorD", power); }
    /// @brief Brake DC motor A
    /// @param power [0:100]% power. Default 100%
    void dcBrakeA(int power = 100) { ble.cmdWrite("motorA/brake", power); }
    /// @brief Brake DC motor B
    /// @param power [0:100]% power. Default 100%
    void dcBrakeB(int power = 100) { ble.cmdWrite("motorB/brake", power); }
    /// @brief Brake DC motor C
    /// @param power [0:100]% power. Default 100%
    void dcBrakeC(int power = 100) { ble.cmdWrite("motorC/brake", power); }
    /// @brief Brake DC motor D
    /// @param power [0:100]% power. Default 100%
    void dcBrakeD(int power = 100) { ble.cmdWrite("motorD/brake", power); }
    /// @brief Spin all DC motors with singe command
    /// @param motorA [-100:100]% power. [0] stop
    /// @param motorB [-100:100]% power. [0] stop
    /// @param motorC [-100:100]% power. [0] stop
    /// @param motorD [-100:100]% power. [0] stop
    void dcSpinABCD(int motorA, int motorB, int motorC, int motorD) { ble.cmdWrite("motorABCD", ((uint8_t)motorA)<<24|((uint8_t)motorB)<<16|((uint8_t)motorC)<<8|((uint8_t)motorD)); }
    /// @brief Brake all DC motors with single command
    /// @param motorA [0:100]% power. Default 100%
    /// @param motorB [0:100]% power. Default 100%
    /// @param motorC [0:100]% power. Default 100%
    /// @param motorD [0:100]% power. Default 100%
    void dcBrakeABCD(int motorA=100, int motorB=100, int motorC=100, int motorD=100) { ble.cmdWrite("motorABCD/brake", ((uint8_t)motorA)<<24|((uint8_t)motorB)<<16|((uint8_t)motorC)<<8|((uint8_t)motorD)); }

    /// @brief Set color to all RGB lights
    /// @param hex [0:0xFFFFFF] HEX color
    void rgbColor(uint32_t hex) { ble.cmdWrite("rgbAll", (0xFF<<24)|hex); }
    /// @brief Set color to RGB light A
    /// @param hex [0:0xFFFFFF] HEX color
    void rgbColorA(uint32_t hex) { ble.cmdWrite("rgbA", (0xFF<<24)|hex); }
    /// @brief Set color to RGB light B
    /// @param hex [0:0xFFFFFF] HEX color
    void rgbColorB(uint32_t hex) { ble.cmdWrite("rgbB", (0xFF<<24)|hex); }
    /// @brief Set color to RGB light C
    /// @param hex [0:0xFFFFFF] HEX color
    void rgbColorC(uint32_t hex) { ble.cmdWrite("rgbC", (0xFF<<24)|hex); }
    /// @brief Set color to RGB light D
    /// @param hex [0:0xFFFFFF] HEX color
    void rgbColorD(uint32_t hex) { ble.cmdWrite("rgbD", (0xFF<<24)|hex); }
    /// @brief Set color to all RGB lights
    /// @param red [0:255] red
    /// @param green [0:255] green
    /// @param blue [0:255] blue
    void rgbColor(uint8_t red, uint8_t green, uint8_t blue) { rgbColor((red<<16)|(green<<8)|(blue)); }
    /// @brief Set color to RGB light A
    /// @param red [0:255] red
    /// @param green [0:255] green
    /// @param blue [0:255] blue
    void rgbColorA(uint8_t red, uint8_t green, uint8_t blue) { rgbColorA((red<<16)|(green<<8)|(blue)); }
    /// @brief Set color to RGB light B
    /// @param red [0:255] red
    /// @param green [0:255] green
    /// @param blue [0:255] blue
    void rgbColorB(uint8_t red, uint8_t green, uint8_t blue) { rgbColorB((red<<16)|(green<<8)|(blue)); }
    /// @brief Set color to RGB light C
    /// @param red [0:255] red
    /// @param green [0:255] green
    /// @param blue [0:255] blue
    void rgbColorC(uint8_t red, uint8_t green, uint8_t blue) { rgbColorC((red<<16)|(green<<8)|(blue)); }
    /// @brief Set color to RGB light D
    /// @param red [0:255] red
    /// @param green [0:255] green
    /// @param blue [0:255] blue
    void rgbColorD(uint8_t red, uint8_t green, uint8_t blue) { rgbColorD((red<<16)|(green<<8)|(blue)); }
    /// @brief Set Totem color to all RGB lights
    void rgbColorTotem() { ble.cmdWrite("rgbAll/totem"); }
    /// @brief Reset RGB lights to board color
    void rgbColorReset() { ble.cmdWrite("rgbAll/reset"); }
};

} // namespace _Totem::BLE

#endif /* LIB_PRIVATE_BLE_TOTEM_BLE_CONTROL_BOARD */
