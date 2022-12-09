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
#ifndef LIB_TOTEM_SRC_INTERFACES_INTERFACEBLE
#define LIB_TOTEM_SRC_INTERFACES_INTERFACEBLE

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "api/TotemRobot.h"

using RobotReceiver = void (*)(TotemRobot robot);

namespace TotemLib {

class InterfaceBLE : BLEAdvertisedDeviceCallbacks, BLEClientCallbacks {
    BLEUUID advertisingService = std::string("bae50001-a471-446a-bc43-4b0a60512636");
    BLEScan *scanner = nullptr;
    bool scanActive = false;
    bool mainTask = false;
    RobotReceiver foundReceiver = nullptr;
    RobotReceiver connectionReceiver = nullptr;
    TotemRobotInfo* robotInfoPool[15];
    // Values shared between tasks
    TotemRobotInfo** lastConnectedRobot = &robotInfoPool[14];
    QueueHandle_t connectQueue = nullptr;
public:
    InterfaceBLE() {

    }
    ~InterfaceBLE() {
        for (auto robot : robotInfoPool)
            delete robot;
    }
    /**
     * Initialize BLE interface.
     * Must be called inside setup() to use this interface
     */
    void begin() {
        BLEDevice::init("");
        BLEDevice::setMTU(517);
        scanner = BLEDevice::getScan();
        scanner->setActiveScan(true);
        scanner->setInterval(500);
        scanner->setWindow(5000);
        scanner->setAdvertisedDeviceCallbacks(this, true);
    }
    /**
     * Start searching for Totem Robot over Bluetooth Low Energy.
     * @note This function will block further code execution until 
     * connection to robot is established or stopFind() is called.
     * @brief There are two modes using this function:
     * 1. Find Any: It received is not provided or set to nullptr - this function
     * will connect to first discovered valid Totem Robot.
     * 2. List and connect: If receiver function is provided - all discovered results
     * will be sent to receiver function and user must select wanted robot and call
     * FoundDevice.connect() function. After this disconery will be stopped and
     * TotemRobot instance returned.
     * @param receiver - receiver function listing discovered robots
     * @return TotemRobot - instance of robot connection. Never returns nullptr
     */
    TotemRobot findRobot(RobotReceiver receiver = nullptr) {
        vTaskDelay(100 / portTICK_PERIOD_MS); //FIXME: some delay between findRobot calls is required to prevent crash
        this->foundReceiver = receiver;
        this->mainTask = true;
        scan_task(this);
        return TotemRobot(this->lastConnectedRobot);
    }
    /**
     * Start searching for Totem Robot over Bluetooth Low Energy
     * @note This function won't block further code execution. All processing 
     * is performed in background. Process continues until connection to robot is
     * established or stopFind() is called.
     * @brief There are two modes using this function:
     * 1. Find Any: It received is not passed or set to nullptr - this function
     * will connect to first discovered valid Totem Robot.
     * 2. List and connect: If receiver function is passed - all discovered results
     * will be sent to receiver function and user must select wanted robot and call
     * FoundDevice.connect() function. After this disconery will be stopped and
     * TotemRobot instance returned
     * @param receiver - receiver function listing discovered robots
     * @return TotemRobot - instance of robot connection. Never returns nullptr
     */
    void findRobotNoBlock(RobotReceiver receiver = nullptr) {
        if (scanner == nullptr) return;
        if (scanActive) return;
        this->foundReceiver = receiver;
        this->mainTask = false;
        FreeRTOS::startTask(InterfaceBLE::scan_task, "scan_task", this, 3072);
    }
    /**
     * Check if find operation is active.
     */
    bool isFinding() {
        return scanActive;
    }
    /**
     * Manually stop discovering for Totem Robots over Bluetooth Low Energy.
     * This will unblock findRobot() function and will stop execution of findRobotNoBlock().
     * @note findRobot discovery automatically will be stopped if 
     * FoundDevice.connect() is called and connected.
     */
    void stopFind() {
        stopScan();
    }
    /**
     * Register callback function to notify about connection or disconnection
     */
    void attachOnConnection(RobotReceiver receiver) {
        this->connectionReceiver = receiver;
    }
    /**
     * Get number of active connections
     */
    int getConnectedCount() {
        int count = 0;
        for (auto robot : robotInfoPool) {
            if (robot && robot->isConnected()) count++;
        }
        return count;
    }
    /**
     * Get object of last connection attempt.
     */
    TotemRobot getConnectedLast() {
        return TotemRobot(this->lastConnectedRobot);
    }
    /**
     * Get list of active connections
     */
    std::vector<TotemRobot> getConnectedList() {
        std::vector<TotemRobot> ret;
        for (auto &robot : robotInfoPool) {
            if (robot && robot->isConnected()) ret.push_back(&robot);
        }
        return ret;
    }
private:
    void stopScan() {
        if (scanActive) {
            BLEDevice::getScan()->stop();
        }
    }
    static void scan_task(void *context) {
        InterfaceBLE *inst = static_cast<InterfaceBLE*>(context);
        // Create queue
        inst->connectQueue = xQueueCreate(5, sizeof(TotemRobotInfo**));
        assert(inst->connectQueue);
        // Set scan status to active
        inst->scanActive = true;
        // Start BLE scan. This will block task
        BLEDevice::getScan()->start(0, nullptr, false);
        // Loop trough all results
        while (inst->scanActive) {
            TotemRobotInfo **robot;
            // Wait for any results from onResult()
            if (xQueueReceive(inst->connectQueue, &robot, (TickType_t)500) == pdPASS) {
                // Save last attempted to connect robot
                inst->lastConnectedRobot = robot;
                // Call found receiver or connect manually
                if (inst->foundReceiver) {
                    inst->foundReceiver(TotemRobot(robot));
                }
                else {
                    (*robot)->connect();
                }
                if ((*robot)->isConnected()) {
                    // Connected
                    if (inst->connectionReceiver)
                        inst->connectionReceiver(TotemRobot(robot));
                    break;
                }
            }
        }
        // Queue no more required
        vQueueDelete(inst->connectQueue);
        inst->connectQueue = nullptr;
        // Remove unconnected robots
        for (auto &r : inst->robotInfoPool) {
            if (r) {
                if (r->isConnected()) continue;
                delete r;
                r = nullptr;
            }
        }
        // Mark scan as inactive
        inst->scanActive = false;
        // Delete task
        if (!inst->mainTask) {
            vTaskDelete(nullptr);
        }
    }
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        TotemRobotInfo **robot = nullptr;
        // Look if device is already on the list
        for (auto &r : robotInfoPool) {
            if (r && r->address.equals(advertisedDevice.getAddress())) {
                robot = &r;
                break;
            }
        }
        // Create new Totem robot
        if (robot == nullptr) {
            if (advertisedDevice.isAdvertisingService(advertisingService)) {
                for (auto &r : robotInfoPool) { if (r == nullptr) { robot = &r; break; } }
                assert(robot);
                *robot = new TotemRobotInfo();
                assert(*robot);
                (*robot)->address = advertisedDevice.getAddress();
                (*robot)->addressType = advertisedDevice.getAddressType();
                (*robot)->remoteRobot.client->setClientCallbacks(this);
            }
            else {
                return;
            }
        }
        // Update manufacturer data
        if (advertisedDevice.haveManufacturerData()) {
            (*robot)->setManufacturerData(advertisedDevice.getManufacturerData());
        }
        // Update device name
        if (advertisedDevice.haveName()) {
            (*robot)->setName(advertisedDevice.getName());
        }
        // Show as discovered if all data is collected
        if ((*robot)->isReady()) {
            // Put discovered board to queue
            if (uxQueueSpacesAvailable(connectQueue) > 0) {
                // Send advertisement to scan_task
                xQueueSendFromISR(connectQueue, &robot, nullptr);
            }
        }
    }
    void onConnect(BLEClient *pClient) override { }
    void onDisconnect(BLEClient *pClient) override {
        for (auto &robot : robotInfoPool) {
            if (robot && robot->remoteRobot.client == pClient) {
                robot->remoteRobot.reset();
                if (this->connectionReceiver)
                    this->connectionReceiver(TotemRobot(&robot));
                // delete robot; //FIXME: properly clean resources
                robot = nullptr;
                break;
            }
        }
    }
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_INTERFACES_INTERFACEBLE */
