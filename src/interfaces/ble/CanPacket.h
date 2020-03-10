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
#ifndef LIB_TOTEM_SRC_INTERFACES_BLE_CANPACKET
#define LIB_TOTEM_SRC_INTERFACES_BLE_CANPACKET

#include <string.h>

#include "ByteBuffer.h"

class CanPacket {
public:
    template <int SIZE>
    struct Data {
        const uint8_t capacity = SIZE;
        uint8_t data[SIZE];
        uint8_t length = 0;
        void set(uint8_t* data, uint8_t len) {
            this->length = len % (SIZE+1);
            memcpy(this->data, data, this->length);
        }
    };
private:
    static const uint32_t CAN_ID_EXTENDED = 0x80000000;
    static const uint32_t CAN_ID_RTR      = 0x40000000;

    uint32_t _id;
    uint8_t _len;
    Data<8> _data;
public:
    CanPacket() {
        _id = 0;
        _len = 0;
    }
    CanPacket(uint32_t id, uint8_t *data, uint8_t len) {
        _id = id;
        _len = len;
        if (!isRTR())
            _data.set(data, len);
    }

    uint32_t id() {
        return _id;
    }
    uint8_t len() {
        return _len;
    }
    uint8_t* data() {
        return _data.data;
    }

    bool isExtended() {
        return (_id & CAN_ID_EXTENDED) != 0;
    }
    bool isRTR() {
        return (_id & CAN_ID_RTR) != 0;
    }

    bool arrayPacked(Data<13> &array) {
        return isExtended() ? writeExtendedPacket(array) : writeStandardPacket(array);
    }
    static bool fromPackedStream(ByteBuffer &stream, CanPacket &packet) {
        if (stream.remaining() == 0) return false;
        bool result;
        if (isExtended(stream.get(0)))
            result = packet.readExtendedPacket(stream);
        else
            result = packet.readStandardPacket(stream);
        return result;
    }
private:
    bool writeExtendedPacket(Data<13> &array) {
        ByteBuffer buffer(array.data, array.capacity);
        buffer.putInt(_id);
        buffer.put(_len);
        if (!isRTR()) {
            buffer.put(_data.data, _data.length);
        }
        array.length = buffer.position();
        return !buffer.isError();
    }
    bool writeStandardPacket(Data<13> &array) {
        ByteBuffer buffer(array.data, array.capacity);
        short size = _len;
        if (isRTR()) size = 0xF;
        buffer.putShort((short) ( (size << 11) | (_id & 0x7FF) ));
        if (isRTR())
            buffer.put(_len);
        else
            buffer.put(_data.data, _data.length);
        array.length = buffer.position();
        return !buffer.isError();
    }
    bool readExtendedPacket(ByteBuffer &buffer) {
        _id = buffer.getInt();
        _len = buffer.get();
        if (_len > 8) return false;
        return buffer.get(_data.data, isRTR() ? 0 : _len);
    }
    bool readStandardPacket(ByteBuffer &buffer) {
        uint16_t packedId = buffer.getShort();
        _len = static_cast<uint8_t>((packedId >> 11) & 0xF);
        _id = packedId & 0x7FF;
        if (_len > 8) {
            _id |= CAN_ID_RTR;
            _len = buffer.get();
            if (_len > 8) return false;
        }
        return buffer.get(_data.data, isRTR() ? 0 : _len);
    }
    static bool isExtended(uint8_t firstByte) {
        return (firstByte & 0x80) != 0;
    }

};

#endif /* LIB_TOTEM_SRC_INTERFACES_BLE_CANPACKET */
