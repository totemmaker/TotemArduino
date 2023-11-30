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
#ifndef LIB_TOTEM_SRC_MODULES_MODULEDATA
#define LIB_TOTEM_SRC_MODULES_MODULEDATA

#include "core/TotemBUS.h"

namespace TotemLib {

struct ModuleData {
    bool is(const char *command) {
        return cmdHash == TotemBUS::hash(command);
    }
    bool isInt() {
        return ptr == nullptr;
    }
    bool isString() {
        return ptr != nullptr;
    }
    int getInt() {
        return value;
    }
    const char *getString() {
        return reinterpret_cast<char*>(ptr);
    }
    template <typename Type>
    bool getData(Type **data) {
        return getData<Type>(*data);
    }
    template <typename Type>
    bool getData(Type *&data) {
        data = reinterpret_cast<Type*>(ptr);
        return ptr != nullptr;
    }
    template <typename Type>
    bool getData(Type **data, int32_t &len) {
        return getData<Type>(*data, len);
    }
    template <typename Type>
    bool getData(Type *&data, int32_t &len) {
        data = reinterpret_cast<Type*>(ptr);
        len = (ptr == nullptr) ? 0 : value;
        return ptr != nullptr;
    }
    uint32_t getHashCmd() {
        return cmdHash;
    }
    ModuleData(uint32_t cmd, uint8_t *ptr, int32_t val) :
    cmdHash(cmd), ptr(ptr), value(val) { }
    ModuleData() {}
private:
    uint32_t cmdHash = 0;
    uint8_t *ptr = nullptr;
    int32_t value = 0;
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_MODULES_MODULEDATA */
