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

#include "api/TotemModule.h"
#include "api/MotorDriver.h"
#ifdef ARDUINO_ARCH_ESP32
#include "interfaces/InterfaceX4.h"
#include "interfaces/InterfaceBLE.h"
#endif

class _Totem {
public:
#ifdef ARDUINO_ARCH_ESP32
    TotemLib::InterfaceBLE    BLE;
    TotemLib::InterfaceX4     X4;
#endif
};

extern _Totem Totem;

#endif /* LIB_TOTEM_SRC_TOTEM */
