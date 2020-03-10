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
#ifndef LIB_TOTEM_SRC_INTERFACES_BLE_BYTEBUFFER
#define LIB_TOTEM_SRC_INTERFACES_BLE_BYTEBUFFER

#include <stdint.h>

class ByteBuffer {
    int _capacity;
    int _position;
    int _limit;
    uint8_t *memory;
    bool error = false;
public:
    ByteBuffer(uint8_t* buffer, uint32_t size) {
        memory = buffer;
        _capacity = size;
        _limit = _capacity;
        _position = 0;
    }
    bool isError() { return error; }
    int position() { return _position; }
    int limit() { return _limit; }
    void limit(int newLimit) { 
        if ((newLimit > _capacity) || (newLimit < 0))
            newLimit = _capacity;
        _limit = newLimit;
        if (_position > _limit) _position = _limit;
    }
    int capacity() { return _capacity; }
    int remaining() { return _limit - _position; }
    bool hasRemaining() {
        return _position < _limit;
    }
    uint8_t* array() { return memory; }
    void clear() {
        _position = 0;
        _limit = _capacity;
    }
    uint8_t get(int index) {
        if (index >= _capacity) return 0;
        return memory[index];
    }
    void putInt(uint32_t value) {
        if (remaining() < 4) { error = true; return; }
        memory[_position++] = (value >> 24) & 0xFF;
        memory[_position++] = (value >> 16) & 0xFF;
        memory[_position++] = (value >>  8) & 0xFF;
        memory[_position++] =         value & 0xFF;
    }
    void putShort(uint16_t value) {
        if (remaining() < 2) { error = true; return; }
        memory[_position++] = (value >>  8) & 0xFF;
        memory[_position++] =         value & 0xFF;
    }
    void put(uint8_t value) {
        if (remaining() < 1) { error = true; return; }
        memory[_position++] =         value & 0xFF;
    }
    void put(uint8_t *data, uint32_t len) {
        if (remaining() < len) { error = true; return; }
        memcpy(&memory[_position], data, len);
        _position += len;
    }
    uint32_t getInt() {
        if (_limit-_position < 4) { error = true; return 0; }
        uint32_t value = 0;
        value |= (static_cast<uint32_t>(memory[_position++]) << 24);
        value |= (static_cast<uint32_t>(memory[_position++]) << 16);
        value |= (static_cast<uint32_t>(memory[_position++]) <<  8);
        value |= (static_cast<uint32_t>(memory[_position++]));
        return value;
    }
    uint16_t getShort() {
        if (_limit-_position < 2) { error = true; return 0; }
        uint16_t value = 0;
        value |= (static_cast<uint16_t>(memory[_position++]) <<  8);
        value |= (static_cast<uint16_t>(memory[_position++]));
        return value;
    }
    uint8_t get() {
        if (_limit-_position < 1) { error = true; return 0; }
        return memory[_position++];
    }
    bool get(uint8_t *data, uint32_t len) {
        if (len == 0) return !isError();
        if (_limit-_position < len) { error = true; return !isError(); }
        memcpy(data, &memory[_position], len);
        _position += len;
        return true;
    }
};

#endif /* LIB_TOTEM_SRC_INTERFACES_BLE_BYTEBUFFER */
