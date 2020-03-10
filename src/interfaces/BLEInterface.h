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
#ifndef LIB_TOTEM_SRC_INTERFACES_BLEINTERFACE
#define LIB_TOTEM_SRC_INTERFACES_BLEINTERFACE

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <vector>

#include "api/TotemRobot.h"

using RobotReceiver = void (*)(TotemRobot robot);

namespace TotemLib {

class BLEInterface : BLEAdvertisedDeviceCallbacks, BLEClientCallbacks {
    BLEUUID advertisingService = std::string("bae50001-a471-446a-bc43-4b0a60512636");
    BLEScan *scanner = nullptr;
    bool scanActive = false;
    bool mainTask = false;
    RobotReceiver foundReceiver = nullptr;
    RobotReceiver connectionReceiver = nullptr;
    std::vector<TotemRobotInfo*> robotsList;
    // Values shared between tasks
    TotemRobotInfo *connectingRobot = nullptr;
    TotemRobotInfo *lastConnectedRobot = nullptr;
    QueueHandle_t connectQueue = nullptr;
public:
    BLEInterface() {
        this->lastConnectedRobot = getFreeRobot();
    }
    ~BLEInterface() {
        for (auto robot : robotsList)
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
        scanner->setAdvertisedDeviceCallbacks(this, false);
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
        this->foundReceiver = receiver;
        this->mainTask = true;
        if (receiver) {
            FreeRTOS::startTask(BLEInterface::connect_task, "connect_task", this, 3072);
        }
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
        this->foundReceiver = receiver;
        this->mainTask = false;
        if (receiver) {
            FreeRTOS::startTask(BLEInterface::connect_task, "connect_task", this, 3072);
        }
        startScan();
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
        for (auto robot : robotsList)
            if (robot->isConnected()) count++;
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
        for (auto robot : robotsList) {
            if (robot->isConnected()) ret.push_back(robot);
        }
        return ret;
    }
private:
    void startScan(bool continueScan = false) {
        if (!scanActive) {
            scanActive = true;
            FreeRTOS::startTask(BLEInterface::scan_task, "scan_task", this, 3072);
        }
    }
    void stopScan() {
        if (scanActive) {
            BLEDevice::getScan()->stop();
        }
    }
    TotemRobotInfo* getFreeRobot() {
        for (auto robot : robotsList) {
            if (!robot->isConnected()) return robot;
        }
        auto robot = new TotemRobotInfo();
        robot->remoteRobot.client->setClientCallbacks(this);
        assert(robot);
        robotsList.push_back(robot);
        return robot;
    }
    static void scan_task(void *context) {
        BLEInterface *inst = static_cast<BLEInterface*>(context);
        // Set scan status to active
        inst->scanActive = true;
        // Start BLE scan. This will block task
        BLEDevice::getScan()->start(0, false);
        // Scan completed.
        // Check if found receiver was not registered
        if (inst->foundReceiver == nullptr) {
            // Check if any robot was found
            if (inst->connectingRobot) {
                // Try connect discovered robot
                inst->connectingRobot->connect();
                // Check if robot is connected
                if (inst->connectingRobot->isConnected()) {
                    // Set robot as connected
                    inst->lastConnectedRobot = inst->connectingRobot;
                    // Call connected event if registered
                    if (inst->connectionReceiver)
                        inst->connectionReceiver(TotemRobot(inst->lastConnectedRobot));
                }
                // Set connecting robot to none
                inst->connectingRobot = nullptr;
            }
        }
        // Mark scan as inactive
        inst->scanActive = false;
        // Delete task
        if (!inst->mainTask) {
            vTaskDelete(nullptr);
        }
    }
    static void connect_task(void *context) {
        BLEInterface *inst = static_cast<BLEInterface*>(context);
        // Create queue
        inst->connectQueue = xQueueCreate(5, sizeof(BLEAdvertisedDevice*));
        assert(inst->connectQueue);
        // Loop trough all results
        while (inst->scanActive) {
            TotemRobotInfo *robot;
            // Wait for any results from onResult()
            if (xQueueReceive(inst->connectQueue, &robot, (TickType_t)500) == pdPASS) {
                // Call found receiver to connect manually
                inst->foundReceiver(TotemRobot(robot));
                if (robot->isConnected()) {
                    // Connected
                    inst->lastConnectedRobot = robot;
                    if (inst->connectionReceiver)
                        inst->connectionReceiver(TotemRobot(robot));
                    break;
                }
            }
        }
        // Queue no more required
        vQueueDelete(inst->connectQueue);
        inst->connectQueue = nullptr;
        // Delete task
        vTaskDelete(nullptr);
    }
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        // Check if advertised device is Totem Robot
        if (advertisedDevice.isAdvertisingService(advertisingService)) {
            // If no receiver provided - connect to any first result
            if (foundReceiver == nullptr) {
                this->connectingRobot = getFreeRobot();
                this->connectingRobot->setAdvertisingData(advertisedDevice);
                stopScan();
            }
            // Check if there is free space in queue
            else if (uxQueueSpacesAvailable(connectQueue) > 0) {
                // Send advertisement to connect_task
                TotemRobotInfo *robot = getFreeRobot();
                robot->setAdvertisingData(advertisedDevice);
                xQueueSendFromISR(connectQueue, &robot, nullptr);
            }    
        }
    }
    void onConnect(BLEClient *pClient) override { }
    void onDisconnect(BLEClient *pClient) override {
        for (auto robot : robotsList) {
            if (robot->remoteRobot.client == pClient) {
                robot->remoteRobot.reset();
                if (this->connectionReceiver)
                    this->connectionReceiver(TotemRobot(robot));
                break;
            }
        }
    }
};

} // namespace TotemLib

#endif /* LIB_TOTEM_SRC_INTERFACES_BLEINTERFACE */
