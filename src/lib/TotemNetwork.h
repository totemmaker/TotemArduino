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
#ifndef LIB_TOTEM_SRC_LIB_TOTEMNETWORK
#define LIB_TOTEM_SRC_LIB_TOTEMNETWORK

#include "core/TotemBUS.h"
#include "ModuleList.h"

namespace TotemLib {

class TotemNetwork : public ModuleList {

public:
    using OnConnectedReceiver = void (*)(uint16_t number, uint16_t serial);

    TotemNetwork() : ModuleList(this) { }  

    ~TotemNetwork() {
        moduleListMainReset();
    }

    void attachOnModuleConnected(OnConnectedReceiver receiver) {
        this->onConnectedReceiver = receiver;
        if (receiver) {
            TotemBUS::Frame frame = TotemBUS::ping();
            networkSend(frame, 0, 0);
        }
    }
    
    virtual bool networkSend(TotemBUS::Frame &frame, int number, int serial) = 0;
protected:
    virtual void onMessageReceive(TotemBUS::Message &message) {
        // If received ping
        if (message.type == TotemBUS::MessageType::ResponsePing) {
            if (onConnectedReceiver) {
                onConnectedReceiver(message.number, message.serial);
            }
            return;
        }
        moduleListCallMessageReceive(message);
    }
private:
    OnConnectedReceiver onConnectedReceiver = nullptr;
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_LIB_TOTEMNETWORK */
