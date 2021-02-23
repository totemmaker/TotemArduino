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
#ifndef LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMBLENETWORK
#define LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMBLENETWORK

#include <FreeRTOS.h>

#include "lib/TotemNetwork.h"

namespace TotemLib {

class TotemBLENetwork : public TotemNetwork {
    TotemBUS::Memory<1, 256> memory;
    TotemBUS totemBUS;
    volatile struct {
        uint16_t number;
        int32_t serial;
        bool detected = true;
    } pingMonitor;

public:
    
    TotemBLENetwork() :
    totemBUS(memory, this, onTotemBUSCANSend, onTotemBUSMessageReceive)
    { 
        sendPacketsQueue = xRingbufferCreate(sizeof(TotemBUSProtocol::CanPacket)*100, RINGBUF_TYPE_BYTEBUF);
        FreeRTOS::startTask(canPacketsSendTask, "network_send", this, 3072);
    }
    ~TotemBLENetwork() {
        taskRunning = false;
        moduleListMainReset();
        vRingbufferDelete(sendPacketsQueue);
    }

    bool isConnected(uint16_t moduleNumber, uint16_t moduleSerial = 0) {
        return isModuleConnected(50, 2, moduleNumber, moduleSerial);
    }    

    bool networkSend(TotemBUS::Frame &frame, int number, int serial) override {
        return frame.send(totemBUS, number, serial);
    }
protected:
    
    // Called from parent
    void processCANPacket(uint32_t id, uint8_t *data, uint8_t len) {
        if (TotemBUSProtocol::Packet::isV2(id)) {
            /*auto result = */totemBUS.processCAN(id, data, len);
            // if (result != TotemBUSProtocol::Result::OK)
            //     log_e("TotemBUS Error: %d", result);
        }
    }
    virtual void onCANPacketWrite(uint32_t id, uint8_t *data, uint8_t len) = 0;
    // virtual void onModuleFound(uint16_t number, uint16_t serial) {}

    void onBUSMessageReceive(TotemBUS::Message &message) {
        // If received ping
        if (message.type == TotemBUS::MessageType::ResponsePing) {
            // Process ping monitor
            if (!pingMonitor.detected) {
                if ((pingMonitor.serial == -1 || pingMonitor.serial == message.serial)
                && pingMonitor.number == message.number) {
                    pingMonitor.detected = true;
                }
                // If monitor is enabled - block output to application
                return;                
            }
            // onModuleFound(message.number, message.serial);
            return;
        }
        onMessageReceive(message);
    }

private:
    volatile bool taskRunning = true;
    RingbufHandle_t sendPacketsQueue;

    bool isModuleConnected(int timeout, int retries, uint16_t number, uint16_t serial, int32_t serialFilter = -1) {
        pingMonitor.number = number;
        pingMonitor.serial = (serial == 0) ? serialFilter : serial;
        pingMonitor.detected = false;
        for (int ret=0; ret<retries; ret++) {
            TotemBUS::ping().send(totemBUS, number, serial);
            uint32_t start = millis()+timeout;
            while (start > millis()) {
                if (pingMonitor.detected) {
                    return true;
                }
            }
        }
        pingMonitor.detected = true;
        return false;
    }
    static void canPacketsSendTask(void *context) {
        TotemBLENetwork *network = static_cast<TotemBLENetwork*>(context);

        TotemBUSProtocol::CanPacket *packet;
        size_t packetSize;
        while (network->taskRunning) {
            packet = (decltype(packet))xRingbufferReceiveUpTo(network->sendPacketsQueue, 
            &packetSize, pdMS_TO_TICKS(250), sizeof(TotemBUSProtocol::CanPacket));

            if (packet) {
                network->onCANPacketWrite(packet->id, packet->data, packet->len);
                vRingbufferReturnItem(network->sendPacketsQueue, packet);
            }
        }
        FreeRTOS::deleteTask(nullptr);
    }
    static bool onTotemBUSCANSend(void *context, TotemBUSProtocol::CanPacket &packet) {
        return xRingbufferSendFromISR(static_cast<TotemBLENetwork*>(context)->sendPacketsQueue, 
            &packet, sizeof(packet), nullptr) == pdTRUE;
    }
	static bool onTotemBUSMessageReceive(void *context, TotemBUS::Message message) {
        static_cast<TotemBLENetwork*>(context)->onBUSMessageReceive(message);
        return true;
    }
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMBLENETWORK */
