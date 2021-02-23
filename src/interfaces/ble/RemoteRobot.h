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
#ifndef LIB_TOTEM_SRC_INTERFACES_BLE_REMOTEROBOT
#define LIB_TOTEM_SRC_INTERFACES_BLE_REMOTEROBOT

#include "TotemBLENetwork.h"
#include "TotemCANService.h"

namespace TotemLib {

class RemoteRobot : public TotemBLENetwork, protected TotemCANServiceReceiver {
public:
    BLEClient *client;
    TotemCANService canService;

    RemoteRobot() : 
    canService(client, *this) 
    {
        client = BLEDevice::createClient(); 
    }
    ~RemoteRobot() {
        delete client;
    }
    bool connect(BLEAddress address, esp_ble_addr_type_t type) {
        if (client->isConnected()) return false;
        BLEDevice::getScan()->stop();
        bool result = client->connect(address, type);
        if (result) {
            if (!canService.initService()) {
                client->disconnect();
                return false;
            }
            // setAsMainNetwork();
            moduleListMainSet();
        }
        return result;
    }
    void reset() {
        moduleListMainReset();
    }
private:
    // TotemNetwork:
    // TotemBUS request to send CAN packet to physical interface
    void onCANPacketWrite(uint32_t id, uint8_t *data, uint8_t len) override {
        // Send requested packet to CAN service
        canService.send(id, data, len);
    }
    // TotemCANService:
    // Received CAN packet from BLE CAN service
    void onServiceReceive(uint32_t id, uint8_t *data, uint8_t len) override {
        // Pass received CAN packet to TotemBUS for processing
        processCANPacket(id, data, len);
    }
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_INTERFACES_BLE_REMOTEROBOT */
