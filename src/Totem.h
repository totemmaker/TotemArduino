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
#ifndef LIB_TOTEM_SRC_TOTEM
#define LIB_TOTEM_SRC_TOTEM

#include <Arduino.h>

#ifndef IGNORE_TOTEM_H_WARNING
#pragma GCC warning "Totem.h is deprecated. Use: #include <TotemLabBoard.h>"
#endif
#include "TotemLabBoard.h"
#define LB _getLabBoardInstance()
inline TotemLabBoard& _getLabBoardInstance() {
    static TotemLabBoard instance;
    return instance;
}

#ifdef ARDUINO_ARCH_ESP32
#include "api/TotemModule.h"
#include "api/MotorDriver.h"
#include "interfaces/InterfaceBLE.h"

#define Totem _getTotemInstance()

class _TotemClass {
public:
    TotemLib::InterfaceBLE    BLE;
};

_TotemClass& _getTotemInstance();
// Workaround for compiling on X4, until Totem.BLE is refactored
using TotemModule = TotemLib::TotemModule;
using ModuleData = TotemLib::ModuleData;

#endif // ARDUINO_ARCH_ESP32

#endif /* LIB_TOTEM_SRC_TOTEM */
