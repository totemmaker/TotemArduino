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
#ifndef LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMCANBUS
#define LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMCANBUS

#include "CanPacket.h"
#include "ByteBuffer.h"

class TotemCANbus {
    uint8_t _buffer[520];
    ByteBuffer txBuffer;

protected:
    TotemCANbus() :
    txBuffer(_buffer, sizeof(_buffer))
    {}
    
    virtual int getPacketLength() = 0;
    virtual bool onWriteData(uint8_t *data, uint32_t len) = 0;
    virtual void onCANPacketReceive(uint32_t id, uint8_t *data, uint8_t len) = 0;
    
    bool writeCANPacket(uint32_t id, uint8_t *data, uint8_t len) {
        CanPacket packet(id, data, len);
        if (!appendTxBuffer(packet)) return false;
        if (txBuffer.limit() != getPacketLength()) {
            txBuffer.limit(getPacketLength());
        }
        sendPendingData();
        return true;
    }
    void processReceivedData(const uint8_t *data, uint32_t len) {
        ByteBuffer stream(const_cast<uint8_t*>(data), len);
        CanPacket packet;
        while (CanPacket::fromPackedStream(stream, packet)) {
            onCANPacketReceive(packet.id(), packet.data(), packet.len());
        }
    }
    void onPacketsAvailable() {
        sendPendingData();
    }

private:
    bool appendTxBuffer(CanPacket &packet) { 
        if (!txBuffer.hasRemaining()) return false;
        CanPacket::Data<13> packetArray;
        if (!packet.arrayPacked(packetArray)) return false;
        if (txBuffer.remaining() < packetArray.length) {
           txBuffer.limit(txBuffer.position());
            return false;
        }
        txBuffer.put(packetArray.data, packetArray.length);
        return true;
    }
    
    void sendPendingData() { 
        while (/*!txQueue.isEmpty() || */txBuffer.position() != 0) {
            /*if (txBuffer.hasRemaining()) {
                CanPacket packet;
                while ((packet = txQueue.peek()) != null) {
                    if (!appendTxBuffer(packet)) break;
                    txQueue.poll();
                }
            }*/
            if (/*ble.isPacketsPending() || */txBuffer.position() == 0) return;
            if (!onWriteData(txBuffer.array(), txBuffer.position())) return;
            // Clear TX buffer on success
            txBuffer.clear();
        }
        
        // if (txBuffer.limit() != getPacketLength())
        //     txBuffer.limit(getPacketLength());
    }
};

#endif /* LIB_TOTEM_SRC_INTERFACES_BLE_TOTEMCANBUS */
