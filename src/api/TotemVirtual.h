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
#ifndef LIB_TOTEM_SRC_MODULES_TOTEMVIRTUAL
#define LIB_TOTEM_SRC_MODULES_TOTEMVIRTUAL

#include "TotemModule.h"

class TotemVirtual : public TotemModule {
public:
    TotemVirtual(uint16_t unique_id, TotemModule::DataReceiver receiver = nullptr) : 
    TotemModule(01, unique_id, receiver) {
        // static_assert(0 < unique_id && unique_id <= 0x0FFF, "Unique ID only allowed between 1 and 4095.");
    }
    TotemVirtual(uint16_t robot_type, bool virtualRobot) : 
    TotemModule(01, robot_type & 0x4000, nullptr) {
        // static_assert(0 < robot_type && robot_type <= 0x0FFF, "Robot type only allowed between 1 and 4095.");
    }
};

#endif /* LIB_TOTEM_SRC_MODULES_TOTEMVIRTUAL */
