/* 
 * Copyright 2024 Totem Technology, UAB
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_MODULE_TOTEM_MINI_CONTROL_BOARD
#define LIB_MODULE_TOTEM_MINI_CONTROL_BOARD

#ifndef ESP_PLATFORM
#pragma GCC error "TotemMiniControlBoard.h is only supported in ESP32 boards"
#endif

#include "private/ble/totem-ble-control-board.h"

class TotemMiniControlBoard : public _Totem::BLE::TotemBLEControlBoard {
public:
    TotemMiniControlBoard() : _Totem::BLE::TotemBLEControlBoard(0x03) { }

    /// @brief Set LED strip state (boards v1.4, v1.5)
    /// @param map [0:0b111111111111] LED state map
    void setLED(uint16_t map) { ble.cmdWrite("leds", map); }
    /// @brief Turn full brightness RGB mode (change color to take effect)
    /// @param state [true] full, [false] medium
    void rgbBrightMode(bool state) { ble.cmdWrite("rgbAll/bright", state); }
    /// @brief Check if board has RGB lights (revision 2.0)
    /// @return [true] has RGB, [false] has LED strip
    bool hasRGB() { return getRevision().equals("2.0"); }
};

#endif /* LIB_MODULE_TOTEM_MINI_CONTROL_BOARD */
