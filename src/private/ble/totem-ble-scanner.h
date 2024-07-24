/* 
 * Copyright 2024 Totem Technology, UAB
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef LIB_PRIVATE_BLE_TOTEM_BLE_SCANNER
#define LIB_PRIVATE_BLE_TOTEM_BLE_SCANNER

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

namespace _Totem::BLE {

struct AdvertisedData {
    BLEAddress address;
    esp_ble_addr_type_t addressType;
    String name;
    struct __attribute__((__packed__)) TotemAdvData {
        uint32_t color : 24; // 3 bytes
        uint16_t model;      // 2 bytes
        uint8_t number;      // 1 byte
    } data;
};

class TotemBLEDevice {
    uint8_t ready = 0;
public:
    TotemBLEDevice *next = nullptr;
    AdvertisedData adv;

    TotemBLEDevice(BLEAddress address, esp_ble_addr_type_t type) :
    adv { address, type, "", {}}
    { }

    void setManufacturerData(std::string manufData) {
        // Read manufacturer data from advertisement
        if (!manufData.empty()) {
            if (manufData.size() == 2+sizeof(adv.data)-1) {
                memcpy(&(adv.data), &manufData.data()[2], sizeof(adv.data)-1);
                adv.data.number = 3;
            }
            else if (manufData.size() == 2+sizeof(adv.data))
                memcpy((&adv.data), &manufData.data()[2], sizeof(adv.data));
        }
        ready |= 0x1;
    }
    void setManufacturerData(String manufData) {
        // Read manufacturer data from advertisement
        if (!manufData.isEmpty()) {
            if (manufData.length() == 2+sizeof(adv.data)-1) {
                memcpy(&(adv.data), &(manufData.c_str()[2]), sizeof(adv.data)-1);
                adv.data.number = 3;
            }
            else if (manufData.length() == 2+sizeof(adv.data))
                memcpy((&adv.data), &(manufData.c_str()[2]), sizeof(adv.data));
        }
        ready |= 0x1;
    }

    void setName(std::string name) {
        this->adv.name = String(name.c_str());
        ready |= 0x2;
    }
    void setName(String name) {
        this->adv.name = name;
        ready |= 0x2;
    }

    bool isReady() {
        if (ready == 3) {
            ready |= 0x4;
            return true;
        }
        return false;
    }
};

class TotemBLEScanner : protected BLEAdvertisedDeviceCallbacks {
    TaskHandle_t xTaskUser = nullptr;
    void (*onDeviceFoundClbk)(TotemBLEDevice *device, void *arg) = nullptr;
    void *onDeviceFoundArg = nullptr;
    bool scanRunning = false;
    TotemBLEScanner() { }
public:
    static TotemBLEScanner& getInstance() {
        static TotemBLEScanner instance;
        return instance;
    }
    void addOnDeviceFound(void (*onDeviceFound)(TotemBLEDevice *device, void *arg), void *arg) {
        onDeviceFoundClbk = onDeviceFound;
        onDeviceFoundArg = arg;
    }
    bool scan(uint32_t duration = 0) {
        xTaskUser = xTaskGetCurrentTaskHandle();
        BLEDevice::init("");
        BLEDevice::setMTU(517);
        BLEScan *scanner = BLEDevice::getScan();
        scanner->setActiveScan(true);
        scanner->setInterval(500);
        scanner->setWindow(5000);
        scanner->setAdvertisedDeviceCallbacks(this, true);
        return scanRunning = scanner->start(duration, onScanComplete, false);
    }
    void stop() {
        BLEScan *scanner = BLEDevice::getScan();
        scanner->stop();
        scanRunning = false;
        scanner->clearResults();
        xTaskUser = nullptr;
        // Clean scan results
        while (firstDevice) {
            TotemBLEDevice *next = firstDevice->next;
            delete firstDevice;
            firstDevice = next;
        }
    }
    bool isScanning() {
        return scanRunning;
    }

    TotemBLEDevice* findBoard(int boardID, const char *name, uint32_t timeout) {
        TotemBLEDevice *scanResult = nullptr;
        scan(timeout);
        while (1) {
            xTaskNotifyWait(ULONG_MAX, 0, (uint32_t*)&scanResult, portMAX_DELAY);
            if (scanResult == nullptr) return nullptr;
            if (boardID != -1 && boardID != scanResult->adv.data.number) continue;
            if (name != nullptr && !(scanResult->adv.name.equals(name))) continue;
            stop(); return scanResult;
        }
    }
    TotemBLEDevice* findAddress(const char *address, uint32_t timeout) {
        BLEAddress targetAddress(address);
        TotemBLEDevice *scanResult = nullptr;
        xTaskUser = xTaskGetCurrentTaskHandle();
        scan(timeout);
        while (1) {
            xTaskNotifyWait(ULONG_MAX, 0, (uint32_t*)&scanResult, portMAX_DELAY);
            if (scanResult == nullptr) return nullptr;
            if (scanResult->adv.address.equals(targetAddress)) {
                stop(); return scanResult;
            }
        }
    }

private:
    TotemBLEDevice *firstDevice;

    static void onScanComplete(BLEScanResults results) {
        getInstance().scanRunning = false;
        xTaskNotify(getInstance().xTaskUser, 0, eSetValueWithOverwrite);
    }

    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        static const BLEUUID advertisingService("bae50001-a471-446a-bc43-4b0a60512636");
        // Find existing result
        TotemBLEDevice *device = firstDevice;
        while (device) {
            if (device->adv.address.equals(advertisedDevice.getAddress())) break;
            device = device->next;
        }
        // Insert new device
        if (device == nullptr) {
            if (advertisedDevice.isAdvertisingService(advertisingService)) {
                device = new TotemBLEDevice(
                    advertisedDevice.getAddress(),
                    advertisedDevice.getAddressType()
                );
                // Place device to end of linked list
                if (firstDevice == nullptr) firstDevice = device;
                else {
                    TotemBLEDevice *last = firstDevice;
                    while (last && last->next) { last = last->next; }
                    last->next = device;
                }
            }
            else {
                return;
            }
        }
        // Update manufacturer data
        if (advertisedDevice.haveManufacturerData()) {
            device->setManufacturerData(advertisedDevice.getManufacturerData());
        }
        // Update device name
        if (advertisedDevice.haveName()) {
            device->setName(advertisedDevice.getName());
        }
        // Show as discovered if all data is collected
        if (device->isReady()) {
            // onTotemDeviceFound(device);
            if (xTaskUser) xTaskNotify(xTaskUser, (uint32_t)device, eSetValueWithOverwrite);
            if (onDeviceFoundClbk) onDeviceFoundClbk(device, onDeviceFoundArg);
        }
    }
};

} // namespace _Totem::BLE

#endif /* LIB_PRIVATE_BLE_TOTEM_BLE_SCANNER */
