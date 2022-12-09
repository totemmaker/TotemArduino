/* 
 * This file is part of the TotemArduino distribution (https://github.com/totemmaker/TotemArduino).
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
#ifndef LIB_TOTEM_SRC_INTERFACES_BLE_FOUNDROBOT
#define LIB_TOTEM_SRC_INTERFACES_BLE_FOUNDROBOT

#include "interfaces/ble/TotemRobotInfo.h"

class TotemRobot {
    TotemLib::TotemRobotInfo **info;
public:
    TotemRobot(TotemLib::TotemRobotInfo **info = nullptr) : info(info) { }
    // Robot name    
    String getName() {
        if (*info == nullptr) return "";
        return (*info)->name;
    }
    // Robot MAC address "00:11:22:33:44:55"
    String getAddress() {
        if (*info == nullptr) return "";
        return String((*info)->address.toString().c_str());
    }
    // Robot product type number (Minitrooper, Truck, ...)
    uint16_t getModel() {
        if (*info == nullptr) return 0;
        return (*info)->advData.model;
    }
    // Robot color set by application. 24-bit encoding
    uint32_t getColor() {
        if (*info == nullptr) return 0;
        return (*info)->advData.color;
    }
    // Get module number which is advertising
    uint32_t getNumber() {
        if (*info == nullptr) return 0;
        return (*info)->advData.number;
    }
    // Check if we are connected to Totem Robot over Bluetooth Low Energy
    bool isConnected() {
        if (*info == nullptr) return false;
        return (*info)->isConnected();
    }
    // Connect to Totem Robot. Should be called inside onFoundRobot() function during findRobot()
    bool connect() {
        if (*info == nullptr) return false;
        return (*info)->connect();
    }
    // Disconnect from currently connected robot
    void disconnect() {
        if (*info == nullptr) return;
        if (!isConnected()) return;
        (*info)->remoteRobot.client->disconnect();
    }
    // Attach provided module to this robot connection
    void attach(TotemLib::ModuleObject &module) {
        if (*info == nullptr) return;
        (*info)->remoteRobot.attach(module);
    }
    // Detach provided module from this robot connection
    void detach(TotemLib::ModuleObject &module) {
        if (*info == nullptr) return;
        (*info)->remoteRobot.detach(module);
    }
    // Check if TotemRobot == TotemRobot
    bool operator == (const TotemRobot &ref) const {
        return(this->info == ref.info && this->info != nullptr);
    }
};

#endif /* LIB_TOTEM_SRC_INTERFACES_BLE_FOUNDROBOT */
