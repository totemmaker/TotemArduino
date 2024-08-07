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
#include <Arduino.h>

#define IGNORE_TOTEM_H_WARNING
#include "Totem.h"

#ifdef ARDUINO_ARCH_ESP32

_TotemClass& _getTotemInstance() {
    static _TotemClass instance;
    return instance;
}

namespace TotemLib {

static ModuleList detachedModuleList(nullptr);
static ModuleList *defaultModuleList = &detachedModuleList;
ModuleList& getDefaultModuleList() {
    return *defaultModuleList;
}
ModuleList& getDetachedModuleList() {
    return detachedModuleList;
}
void setDefaultModuleList(ModuleList &list) {
    defaultModuleList = &list;
}

} // namespace TotemLib

#endif // ARDUINO_ARCH_ESP32
