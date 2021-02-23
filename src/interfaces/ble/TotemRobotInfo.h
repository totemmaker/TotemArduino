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
#ifndef LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMROBOTINFO
#define LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMROBOTINFO

#include <BLEAdvertisedDevice.h>

#include "RemoteRobot.h"

namespace TotemLib {

class TotemRobotInfo {
public:
    RemoteRobot remoteRobot;
    esp_ble_addr_type_t addressType;
    BLEAddress address;
    String name;
    struct __attribute__((__packed__)) TotemAdvData {
        uint32_t color : 24; // 3 bytes
        uint16_t model;      // 2 bytes
        uint8_t number;      // 1 byte
    } advData;

    TotemRobotInfo() : address("") { memset(&advData, 0, sizeof(advData)); }
    
    bool isConnected() {
        return remoteRobot.client->isConnected();
    }
    bool connect() {
        return remoteRobot.connect(address, addressType);
    }
    
    void setAdvertisingData(BLEAdvertisedDevice &advert) {
        // Read manufacturer data from advertisement
        std::string manufData = advert.getManufacturerData();
        if (!manufData.empty()) {
            if (manufData.size() == 2+sizeof(advData)-1) {
                memcpy(&advData, &manufData.data()[2], sizeof(advData)-1);
                advData.number = 3;
            }
            else if (manufData.size() == 2+sizeof(advData))
                memcpy(&advData, &manufData.data()[2], sizeof(advData));
        }

        name = String(advert.getName().c_str());
        address = advert.getAddress();
        addressType = advert.getAddressType();
    }
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMROBOTINFO */
