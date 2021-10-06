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
#ifndef LIB_TOTEM_SRC_INTERFACES_INTERFACEX4
#define LIB_TOTEM_SRC_INTERFACES_INTERFACEX4

#include "x4/TotemX4.h"
#include "api/TotemModule.h"

namespace TotemLib {

class InterfaceX4 : public TotemModule, public TotemNetwork {
public:
    InterfaceX4() : 
    TotemModule(04)
    { }
    void begin() {
        TotemX4::init(this, onNetworkMessageReceive);
        TotemX4::startBLE();
        setSerial(TotemX4::getSerial());
        moduleListMainSet();
        attach(*this); // TODO: for some reason MainSet does not relocate
    }
    void beginNoBluetooth() {
        TotemX4::init(this, onNetworkMessageReceive);
        setSerial(TotemX4::getSerial());
        moduleListMainSet();
        attach(*this); // TODO: for some reason MainSet does not relocate
    }
private:
    // TotemNetwork sending request to TotemBUS
    bool networkSend(TotemBUS::Frame &frame, int number, int serial) override {
        return TotemX4::sendFrame(frame, number, serial);
    }
    // Message received from X4 network
    static void onNetworkMessageReceive(void *context, TotemBUS::Message &message) {
        InterfaceX4 *interface = static_cast<InterfaceX4*>(context);
        // Pass message to TotemNetwork
        interface->onMessageReceive(message);
    }
};

#define GPIOA 14
#define GPIOB 23
#define GPIOC 25
#define GPIOD 26

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_INTERFACES_INTERFACEX4 */
