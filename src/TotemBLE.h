/* 
 * Copyright 2024 Totem Technology, UAB
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_LINK_TOTEM_BLE
#define LIB_LINK_TOTEM_BLE

#include "private/ble/totem-ble-scanner.h"

class TotemScanResult {
    _Totem::BLE::AdvertisedData adv;
    String strAddress;
public:
    TotemScanResult(_Totem::BLE::AdvertisedData adv) : adv(adv), strAddress(adv.address.toString().c_str()) { }

    /// @brief Get Bluetooth address of discovered board
    /// @return String object
    String getAddress() { return strAddress; }
    /// @brief Get discovered board name
    /// @return String object
    String getName() { return adv.name; }
    /// @brief Get discovered board appearance color
    /// @return 24-bit HEX color
    int getColor() { return adv.data.color; }
    /// @brief Get discovered board appearance identifier
    /// @return 16-bit identifier
    int getModel() { return adv.data.model; }
    /// @brief Get board identification number
    /// @return 8-bit id
    int getNumber() { return adv.data.number; }
    /// @brief Get discovered board type
    /// @return String object
    String getType() {
        switch (adv.data.number) {
        case 0x03: return "Mini Control Board";
        case 0x83: return "RoboBoard X3";
        case 0x04: return "RoboBoard X4";
        default: return "Undefined";
        }
    }
    /// @brief Is discovered board a Mini Control Board
    /// @return [true:false]
    bool isMiniControlBoard() { return getNumber() == 0x03; }
    /// @brief Is discovered board a RoboBoard X3
    /// @return [true:false]
    bool isRoboBoardX3() { return getNumber() == 0x83; }
    /// @brief Is discovered board a RoboBoard X4
    /// @return [true:false]
    bool isRoboBoardX4() { return getNumber() == 0x04; }
};

class TotemBLE {
    void (*onResultClbk)(TotemScanResult);
public:
    /// @brief Register event for discovered scan result
    /// @param onResult void onResult(TotemScanResult result)
    void addOnScanResult(void (*onResult)(TotemScanResult)) {
        onResultClbk = onResult;
        _Totem::BLE::TotemBLEScanner::getInstance().addOnDeviceFound(onDeviceFound, this);
    }
    /// @brief Start Bluetooth scan for Totem boards
    /// @param durationSeconds amount of seconds to scan. [0] infinite
    /// @return [true] scan is started, [false] failed to start
    bool scan(uint32_t durationSeconds = 0) {
        return _Totem::BLE::TotemBLEScanner::getInstance().scan(durationSeconds);
    }
    /// @brief Stop ongoing Bluetooth scan
    void stop() {
        return _Totem::BLE::TotemBLEScanner::getInstance().stop();
    }
    /// @brief Is ongoing scan
    /// @return [true] scanning, [false] not scanning
    bool isScanning() {
        return _Totem::BLE::TotemBLEScanner::getInstance().isScanning();
    }
    /// @brief Wait until scan is completed (or stopped)
    void wait() {
        while (isScanning()) vTaskDelay(10);
    }

    /// @brief Discover any Totem board (block until found)
    /// @param name (optional) find board with matching name
    /// @return TotemScanResult object
    TotemScanResult findAny(const char *name = nullptr) {
        _Totem::BLE::TotemBLEDevice *device;
        device = _Totem::BLE::TotemBLEScanner::getInstance().findBoard(-1, name, 0);
        if (device) return TotemScanResult(device->adv);
        else return TotemScanResult({BLEAddress("")});
    }
    /// @brief Discover Mini Control Board (block until found)
    /// @param name (optional) find board with matching name
    /// @return TotemScanResult object
    TotemScanResult findMiniControlBoard(const char *name = nullptr) {
        _Totem::BLE::TotemBLEDevice *device;
        device = _Totem::BLE::TotemBLEScanner::getInstance().findBoard(0x03, name, 0);
        if (device) return TotemScanResult(device->adv);
        else return TotemScanResult({BLEAddress("")});
    }
    /// @brief Discover RoboBoard X3 (block until found)
    /// @param name (optional) find board with matching name
    /// @return TotemScanResult object
    TotemScanResult findRoboBoardX3(const char *name = nullptr) {
        _Totem::BLE::TotemBLEDevice *device;
        device = _Totem::BLE::TotemBLEScanner::getInstance().findBoard(0x83, name, 0);
        if (device) return TotemScanResult(device->adv);
        else return TotemScanResult({BLEAddress("")});
    }
    /// @brief Discover RoboBoard X4 (block until found)
    /// @param name (optional) find board with matching name
    /// @return TotemScanResult object
    TotemScanResult findRoboBoardX4(const char *name = nullptr) {
        _Totem::BLE::TotemBLEDevice *device;
        device = _Totem::BLE::TotemBLEScanner::getInstance().findBoard(0x04, name, 0);
        if (device) return TotemScanResult(device->adv);
        else return TotemScanResult({BLEAddress("")});
    }
    /// @brief Discover Totem board matching Bluetooth address (block until found)
    /// @param address Bluetooth address to find
    /// @return TotemScanResult object
    TotemScanResult findAddress(const char *address) {
        _Totem::BLE::TotemBLEDevice *device;
        device = _Totem::BLE::TotemBLEScanner::getInstance().findAddress(address, 0);
        if (device) return TotemScanResult(device->adv);
        else return TotemScanResult({BLEAddress("")});
    }
private:
    static void onDeviceFound(_Totem::BLE::TotemBLEDevice *device, void *arg) {
        TotemBLE *inst = (TotemBLE*)arg;
        if (inst->onResultClbk && device) inst->onResultClbk(TotemScanResult(device->adv));
    }
};

#endif /* LIB_LINK_TOTEM_BLE */
