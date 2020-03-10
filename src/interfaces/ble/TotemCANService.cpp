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
#ifdef ARDUINO_ARCH_ESP32

#include "TotemCANService.h"

BLEUUID TotemCANService::TOTEM_CAN_SERVICE = std::string("bae50001-a471-446a-bc43-4b0a60512636");
BLEUUID TotemCANService::TOTEM_CAN_TX      = std::string("bae50002-a471-446a-bc43-4b0a60512636");
BLEUUID TotemCANService::TOTEM_CAN_RX      = std::string("bae50003-a471-446a-bc43-4b0a60512636");
TotemCANService *TotemCANService::instance = nullptr;
#endif