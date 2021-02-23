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
#ifndef LIB_TOTEMX4_SRC_TOTEMX4
#define LIB_TOTEMX4_SRC_TOTEMX4

#include <Update.h>

#include "core/TotemBUS.h"

namespace TotemLib {

class TotemX4 {
public:
    using CallbackMessage = void (*)(void *context, TotemBUS::Message &message);
    
    static bool init(void *context, CallbackMessage messageClbk);
    static bool startBLE();
    static bool sendFrame(TotemBUS::Frame &frame, int number, int serial);
    
    static int getNumber();
    static int getSerial();
};

} // namespace TotemLib

#endif /* LIB_TOTEMX4_SRC_TOTEMX4 */
