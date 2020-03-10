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
#ifndef LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMCANSERVICE
#define LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMCANSERVICE

#include <BLERemoteCharacteristic.h>

#include "TotemCANbus.h"

class TotemCANServiceReceiver {
public:
    virtual ~TotemCANServiceReceiver() {}
    virtual void onServiceReceive(uint32_t id, uint8_t *data, uint8_t len) = 0;
};

class TotemCANService : TotemCANbus {
public:
    static BLEUUID TOTEM_CAN_SERVICE;
    static BLEUUID TOTEM_CAN_TX;
    static BLEUUID TOTEM_CAN_RX;
private:
    BLERemoteCharacteristic *tx_char = nullptr;
    BLERemoteCharacteristic *rx_char = nullptr;

    BLEClient *&client;
    // uint8_t _buffer[250];
    // ByteBuffer txBuffer;
    TotemCANServiceReceiver &receiver;
    static TotemCANService *instance;
public:
    TotemCANService(BLEClient *&client, TotemCANServiceReceiver &receiver) : 
    client(client), /*txBuffer(_buffer, sizeof(_buffer)),*/ receiver(receiver) 
    {
        instance = this;
    }
    bool initService() {
        BLERemoteService *service = client->getService(TOTEM_CAN_SERVICE);
        if (service == nullptr) return false;
        tx_char = service->getCharacteristic(TOTEM_CAN_TX);
        rx_char = service->getCharacteristic(TOTEM_CAN_RX);
        if (tx_char == nullptr || rx_char == nullptr) return false;
        rx_char->registerForNotify(TotemCANService::onDataReceive, true);
        
        // txBuffer.limit(client->getMTU()-3);
        return true;
    }
    void send(uint32_t id, uint8_t *data, uint8_t len, bool haltTransmission = false) {
        if (!client->isConnected()) return;
        writeCANPacket(id, data, len);
        // CanPacket packet(id, data, len);
        // appendTxBuffer(packet);
        // sendPendingData();
    }
private:
    // Bluetooth received data
    static void onDataReceive(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
        if (instance == nullptr) return;
        if (instance->rx_char != pBLERemoteCharacteristic) return;
        // Pass received data to TotemCANbus
        instance->processReceivedData(pData, length);
    }
    // TotemCANbus request max length of packet
    int getPacketLength() override {
        return client->getMTU()-3;
    }
    // TotemCANbus requests to send data to Bluetooth
    bool onWriteData(uint8_t *data, uint32_t len) override {
        tx_char->writeValue(data, len); 
        return true;
    }
    // TotemCANbus received CAN packet
    void onCANPacketReceive(uint32_t id, uint8_t *data, uint8_t len) override {
        receiver.onServiceReceive(id, data, len);
    }
    // void processReceivedData(uint8_t *data, uint32_t len) {
    //     ByteBuffer stream(data, len);
    //     CanPacket packet;
    //     while (CanPacket::fromPackedStream(stream, packet)) {
    //         receiver.onServiceReceive(packet.id(), packet.data(), packet.len());
    //     }
    // }

    // bool appendTxBuffer(CanPacket &packet) { 
    //     if (!txBuffer.hasRemaining()) return false;
    //     CanPacket::Data<13> packetArray;
    //     if (!packet.arrayPacked(packetArray)) return false;
    //     if (txBuffer.remaining() < packetArray.length) {
    //        txBuffer.limit(txBuffer.position());
    //         return false;
    //     }
    //     txBuffer.put(packetArray.data, packetArray.length);
    //     return true;
    // }
    
    // void sendPendingData() { 
    //     while (/*!txQueue.isEmpty() || */txBuffer.position() != 0) {
    //         /*if (txBuffer.hasRemaining()) {
    //             CanPacket packet;
    //             while ((packet = txQueue.peek()) != null) {
    //                 if (!appendTxBuffer(packet)) break;
    //                 txQueue.poll();
    //             }
    //         }*/
    //         if (/*ble.isPacketsPending() || */txBuffer.position() == 0) return;
    //         tx_char->writeValue(txBuffer.array(), txBuffer.position()); 
    //         txBuffer.clear();
    //     }
        
    //     if (txBuffer.limit() != client->getMTU()-3)
    //         txBuffer.limit(client->getMTU()-3);
    // }
};

#endif /* LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMCANSERVICE */
