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
#ifndef LIB_TOTEM_SRC_API_TOTEMMODULE
#define LIB_TOTEM_SRC_API_TOTEMMODULE

#include <stdint.h>

#include "lib/ModuleControl.h"
#include "api/ModuleData.h"

class TotemModule : public TotemLib::Module::Control {
public:
    using DataReceiver = void (*)(ModuleData data);

    TotemModule(uint16_t number, uint16_t serial, DataReceiver receiver) : Control(number, serial), receiver(receiver) { }
    TotemModule(uint16_t number, uint16_t serial = 0) : TotemModule(number, serial, nullptr) { }

    bool write(const char *command) {
        return write(hashCmd(command));
    }
    bool write(const char *command, int32_t value) {
        return write(hashCmd(command), value);
    }
    template <int VSIZE>
    bool write(const char *command, const char (&value)[VSIZE]) {
        return write(hashCmd(command), value);
    }
    bool write(const char *command, const std::string value) {
        return write(hashCmd(command), value);
    }
    bool write(const char *command, const uint8_t *bytes, uint32_t bytesCount) {
        return write(hashCmd(command), bytes, bytesCount);
    }
    bool write(const char *command, int8_t A, int8_t B, int8_t C, int8_t D) {
        return write(hashCmd(command), A, B, C, D);
    }
    bool write(const char *command, int8_t A, int8_t B, int8_t C) {
        return write(hashCmd(command), A, B, C);
    }
    bool writeWait(const char *command) {
        return writeWait(hashCmd(command));
    }
    bool writeWait(const char *command, int32_t value) {
        return writeWait(hashCmd(command), value);
    }
    template <int VSIZE>
    bool writeWait(const char *command, const char (&value)[VSIZE]) {
        return writeWait(hashCmd(command), value);
    }
    bool writeWait(const char *command, const std::string value) {
        return writeWait(hashCmd(command), value);
    }
    bool writeWait(const char *command, const uint8_t *bytes, uint32_t bytesCount) {
        return writeWait(hashCmd(command), bytes, bytesCount);
    }
    bool writeWait(const char *command, int8_t A, int8_t B, int8_t C, int8_t D) {
        return writeWait(hashCmd(command), A, B, C, D);
    }
    bool writeWait(const char *command, int8_t A, int8_t B, int8_t C) {
        return writeWait(hashCmd(command), A, B, C);
    }
    
    bool read(const char *command) {
        return read(hashCmd(command));
    }
    ModuleData readWait(const char *command) {
        return readWait(hashCmd(command));
    }
    bool readWait(const char *command, ModuleData &result) {
        return readWait(hashCmd(command), result);
    }
    bool subscribe(const char *command, int intervalMs = 0) {
        return subscribe(hashCmd(command), intervalMs);
    }
    bool subscribeWait(const char *command, int intervalMs = 0) {
        return subscribeWait(hashCmd(command), intervalMs);
        if (this->receiver == nullptr) return false;
        return moduleSubscribe(hashCmd(command), intervalMs, true);
    }
    bool unsubscribe(const char *command) {
        return unsubscribe(hashCmd(command));
    }
    bool unsubscribeWait(const char *command) {
        return unsubscribeWait(hashCmd(command));
    }
    void attachOnData(DataReceiver receiver) {
        this->receiver = receiver;
    }

    bool write(uint32_t command) {
        return moduleWrite(command, false);
    }
    bool write(uint32_t command, int32_t value) {
        return moduleWrite(command, value, false);
    }
    template <int VSIZE>
    bool write(uint32_t command, const char (&value)[VSIZE]) {
        return moduleWrite(command, {value, VSIZE-1}, false);
    }
    bool write(uint32_t command, const std::string value) {
        return moduleWrite(command, {value.c_str(), value.length()}, false);
    }
    bool write(uint32_t command, const uint8_t *bytes, uint32_t bytesCount) {
        return moduleWrite(command, {reinterpret_cast<const char*>(bytes), bytesCount}, false);
    }
    bool write(uint32_t command, int8_t A, int8_t B, int8_t C, int8_t D) {
        return moduleWrite(command, toValue(A, B, C, D), false);
    }
    bool write(uint32_t command, int8_t A, int8_t B, int8_t C) {
        return moduleWrite(command, toValue(0, A, B, C), false);
    }
    bool writeWait(uint32_t command) {
        return moduleWrite(command, true);
    }
    bool writeWait(uint32_t command, int32_t value) {
        return moduleWrite(command, value, true);
    }
    template <int VSIZE>
    bool writeWait(uint32_t command, const char (&value)[VSIZE]) {
        return moduleWrite(command, {value, VSIZE-1}, true);
    }
    bool writeWait(uint32_t command, const std::string &value) {
        return moduleWrite(command, {value.c_str(), value.length()}, true);
    }
    bool writeWait(uint32_t command, const uint8_t *bytes, uint32_t bytesCount) {
        return moduleWrite(command, {reinterpret_cast<const char*>(bytes), bytesCount}, true);
    }
    bool writeWait(uint32_t command, int8_t A, int8_t B, int8_t C, int8_t D) {
        return moduleWrite(command, toValue(A, B, C, D), true);
    }
    bool writeWait(uint32_t command, int8_t A, int8_t B, int8_t C) {
        return moduleWrite(command, toValue(0, A, B, C), true);
    }

    bool read(uint32_t command) {
        if (this->receiver == nullptr) return false;
        return moduleRead(command, false);
    }
    ModuleData readWait(uint32_t command) {
        response.command = command;
        response.waiting = true;
        bool succ = moduleRead(response.command, true);
        if (!succ || response.waiting) {
            ModuleData data(0, (uint8_t*)"", 0);
            return data;
        }
        return response.data;
    }
    bool readWait(uint32_t command, ModuleData &result) {
        response.command = command;
        response.waiting = true;
        bool succ = moduleRead(response.command, true);
        if (!succ || response.waiting) return false;
        result = response.data;
        return true;
    }
    bool subscribe(uint32_t command, int intervalMs = 0) {
        if (this->receiver == nullptr) return false;
        return moduleSubscribe(command, intervalMs, false);
    }
    bool subscribeWait(uint32_t command, int intervalMs = 0) {
        if (this->receiver == nullptr) return false;
        return moduleSubscribe(command, intervalMs, true);
    }
    bool unsubscribe(uint32_t command) {
        return subscribe(command, -1);
    }
    bool unsubscribeWait(uint32_t command) {
        return subscribeWait(command, -1);
    }

private:
    uint32_t toValue(int8_t A, int8_t B, int8_t C, int8_t D) {
        uint32_t value = 0;
        value |= (static_cast<uint32_t>(A) & 0xFF) << 24;
        value |= (static_cast<uint32_t>(B) & 0xFF) << 16;
        value |= (static_cast<uint32_t>(C) & 0xFF) << 8;
        value |= (static_cast<uint32_t>(D) & 0xFF);
        return value;
    }
    ModuleData getModuleData(int command, int value) {
        ModuleData moduleData(command, nullptr, value);
        return moduleData;
    }
    ModuleData getModuleData(int command, TotemBUSProtocol::String string) {
        ModuleData moduleData(command, reinterpret_cast<uint8_t*>(const_cast<char*>(string.data)), string.length);
        return moduleData;
    }
    struct {
        int command;
        ModuleData data;
        bool waiting;
    } response;
    DataReceiver receiver = nullptr;
    void onModuleMessage(int command, int value, TotemBUSProtocol::String string) override {

        if (response.waiting && command == response.command) {
            if (string.data == nullptr)
                response.data = getModuleData(command, value);
            else
                response.data = getModuleData(command, string);
            response.waiting = false; 
        }
        else if (this->receiver) {
            if (string.data == nullptr)
                this->receiver(getModuleData(command, value));
            else
                this->receiver(getModuleData(command, string));
        }
    }
};

#endif /* LIB_TOTEM_SRC_API_TOTEMMODULE */
