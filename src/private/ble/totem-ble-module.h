/* 
 * Copyright 2024 Totem Technology, UAB
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_PRIVATE_BLE_TOTEM_BLE_MODULE
#define LIB_PRIVATE_BLE_TOTEM_BLE_MODULE

#include <BLEDevice.h>

namespace _Totem::BLE {

#include "totem-ble-scanner.h"

#include "api/TotemRobot.h"
#include "interfaces/ble/TotemBLENetwork.h"
#include "interfaces/ble/TotemCANService.h"

class TotemBLEModule : protected TotemCANServiceReceiver, protected BLEClientCallbacks {
    TotemCANService canService;
    TotemBUS::Memory<1, 128> memory;
    TotemBUS totemBUS;
    BLEClient *client;
    BLEAddress bleAddress = {BLEAddress("")};
    TaskHandle_t xTaskUser = nullptr;
    uint32_t xTaskCommand = 0;
    void (*onConnectionChangeClbk)() = nullptr;
    void (*onConnectionChangeClbkArg)(void *arg) = nullptr;
    void *onConnectionChangeArg = nullptr;
    void (*onValueClbk)(int id, int value) = nullptr;
    void (*onValueClbkArg)(int id, int value, void *arg) = nullptr;
    void *onValueArg = nullptr;
    void (*onStringClbk)(int id, String string) = nullptr;
    void (*onStringClbkArg)(int id, String string, void *arg) = nullptr;
    void *onStringArg = nullptr;
public:
    TotemBLEModule() :
    canService(client, *this),
    totemBUS(memory, this, onTotemBUSCANSend, onTotemBUSMessageReceive) {
        client = BLEDevice::createClient();
        client->setClientCallbacks(this);
    }

    void addOnConnectionChange(void (*onConnectionChange)()) {
        onConnectionChangeClbk = onConnectionChange;
    }
    void addOnConnectionChange(void (*onConnectionChange)(void*), void *arg) {
        onConnectionChangeClbkArg = onConnectionChange;
        onConnectionChangeArg = arg;
    }
    void addOnValue(void (*onValue)(int id, int value)) {
        onValueClbk = onValue;
    }
    void addOnValueArg(void (*onValue)(int id, int value, void *arg), void *arg) {
        onValueClbkArg = onValue;
        onValueArg = arg;
    }
    void addOnString(void (*onString)(int id, String string)) {
        onStringClbk = onString;
    }
    void addOnStringArg(void (*onString)(int id, String string, void *arg), void *arg) {
        onStringClbkArg = onString;
        onStringArg = arg;
    }

    bool connectName(int boardID, const char *name) {
        if (isConnected()) return true;
        _Totem::BLE::TotemBLEDevice *device = _Totem::BLE::TotemBLEScanner::getInstance().findBoard(boardID, name, 0);
        if (device == nullptr) return false;
        return establishConnection(device->adv.address);
    }
    bool connectAddress(const char *address) {
        if (isConnected()) return true;
        BLEDevice::init("");
        BLEDevice::setMTU(517);
        return establishConnection(BLEAddress(address));
    }

    bool isConnected() {
        return client->isConnected();
    }
    void disconnect() {
        if (!isConnected()) return;
        bleAddress = BLEAddress("");
        client->disconnect();
    }

    String getAddress() { return String(bleAddress.toString().c_str()); }

    int cmdReadValue(uint32_t cmd) {
        return waitReadValue(cmd, TotemBUS::read(cmd));
    }
    String cmdReadString(uint32_t cmd) {
        return waitReadString(cmd, TotemBUS::read(cmd));
    }
    int cmdRequestValue(uint32_t cmd) {
        return waitReadValue(cmd, TotemBUS::requestValue(cmd));
    }
    String cmdRequestString(uint32_t cmd) {
        return waitReadString(cmd, TotemBUS::requestString(cmd));
    }
    bool cmdSendValue(uint32_t cmd, int value) {
        if (!isConnected()) return false;
        return networkSend(TotemBUS::sendValue(cmd, (int32_t)value));
    }
    bool cmdSendString(uint32_t cmd, const char *str, int len = -1) {
        if (!isConnected()) return false;
        return networkSend(TotemBUS::sendString(cmd, {str, len<0?strlen(str):len}));
    }
    bool cmdWrite(uint32_t cmd) {
        if (!isConnected()) return false;
        return networkSend(TotemBUS::write(cmd));
    }
    bool cmdWrite(uint32_t cmd, int value) {
        if (!isConnected()) return false;
        return networkSend(TotemBUS::write(cmd, (int32_t)value));
    }
    bool cmdWrite(uint32_t cmd, const char *str, int len = -1) {
        if (!isConnected()) return false;
        return networkSend(TotemBUS::write(cmd, {str, len<0?strlen(str):len}));
    }

    int cmdReadValue(const char *cmd) {
        return cmdReadValue(TotemBUS::hash(cmd));
    }
    String cmdReadString(const char *cmd) {
        return cmdReadString(TotemBUS::hash(cmd));
    }
    bool cmdWrite(const char *cmd) {
        return cmdWrite(TotemBUS::hash(cmd));
    }
    bool cmdWrite(const char *cmd, int value) {
        return cmdWrite(TotemBUS::hash(cmd), value);
    }
    bool cmdWrite(const char *cmd, const char *str, int len = -1) {
        return cmdWrite(TotemBUS::hash(cmd), str, len);
    }
private:
    int waitReadValue(uint32_t cmd, TotemBUS::Frame frame) {
        if (!isConnected()) return 0;
        xTaskCommand = cmd;
        xTaskUser = xTaskGetCurrentTaskHandle();
        if (!networkSend(frame)) {
            xTaskUser = nullptr;
            return 0;
        }
        uint32_t result = 0;
        if (xTaskNotifyWait(ULONG_MAX, 0, &result, pdMS_TO_TICKS(200)) == pdFALSE) return 0;
        return result;
    }
    String waitReadString(uint32_t cmd, TotemBUS::Frame frame) {
        if (!isConnected()) return String("");
        xTaskCommand = cmd;
        xTaskUser = xTaskGetCurrentTaskHandle();
        if (!networkSend(frame)) {
            xTaskUser = nullptr;
            return String("");
        }
        const char *str = nullptr;
        if (xTaskNotifyWait(ULONG_MAX, 0, (uint32_t*)&str, pdMS_TO_TICKS(200)) == pdFALSE) return String("");
        if (str == nullptr) return String("");
        return String(str);
    }
    bool networkSend(TotemBUS::Frame frame) {
        return frame.send(totemBUS, 0, 0);
    }
    bool establishConnection(BLEAddress address) {
        if (!client->connect(address, BLE_ADDR_TYPE_PUBLIC)) return false;
        if (!canService.initService()) {
            client->disconnect();
            return false;
        }
        bleAddress = address;
        return true;
    }
    void onBUSMessageReceive(TotemBUS::Message &message) {
        switch (message.type) {
            case TotemBUS::MessageType::ResponseValue:
                if (xTaskUser && xTaskCommand == message.command) {
                    xTaskNotify(xTaskUser, message.value, eSetValueWithOverwrite);
                    xTaskUser = nullptr;
                    break;
                }
                if (onValueClbk) onValueClbk(message.command, message.value);
                if (onValueClbkArg) onValueClbkArg(message.command, message.value, onValueArg);
                break;
            case TotemBUS::MessageType::ResponseString:
                if (xTaskUser && xTaskCommand == message.command) {
                    xTaskNotify(xTaskUser, (uint32_t)(message.string.data), eSetValueWithOverwrite);
                    xTaskUser = nullptr;
                    break;
                }
                if (onStringClbk) onStringClbk(message.command, String(message.string.data, message.string.length));
                if (onStringClbkArg) onStringClbkArg(message.command, String(message.string.data, message.string.length), onStringArg);
                break;
            case TotemBUS::MessageType::ResponseOk:
                break;
            default:
                return;
        }
    }
    // TotemBUS request to send CAN packet to physical interface
    static bool onTotemBUSCANSend(void *context, TotemBUSProtocol::CanPacket &packet) {
        // Send requested packet to CAN service
        static_cast<TotemBLEModule*>(context)->canService.send(packet.id, packet.data, packet.len);
        return true;
    }
    static bool onTotemBUSMessageReceive(void *context, TotemBUS::Message message) {
        static_cast<TotemBLEModule*>(context)->onBUSMessageReceive(message);
        return true;
    }
    // Received CAN packet from BLE CAN service
    void onServiceReceive(uint32_t id, uint8_t *data, uint8_t len) override {
        // Pass received CAN packet to TotemBUS for processing
        totemBUS.processCAN(id, data, len);
    }
    // BLEClient connection event
    void onConnect(BLEClient *pClient) override {
        if (onConnectionChangeClbk) onConnectionChangeClbk();
        if (onConnectionChangeClbkArg) onConnectionChangeClbkArg(onConnectionChangeArg);
    }
    void onDisconnect(BLEClient *pClient) override {
        if (onConnectionChangeClbk) onConnectionChangeClbk();
        if (onConnectionChangeClbkArg) onConnectionChangeClbkArg(onConnectionChangeArg);
    }
};

} // namespace _Totem::BLE

#endif /* LIB_PRIVATE_BLE_TOTEM_BLE_MODULE */
