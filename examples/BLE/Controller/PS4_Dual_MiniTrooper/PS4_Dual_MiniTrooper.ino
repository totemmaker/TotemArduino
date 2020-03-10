/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to control two Totem Mini Trooper https://totemmaker.net/product/mini-trooper/
 * using single DUALSHOCK®4 controller. This code should run on ESP32 MCU.
 * 
 * PREPARATIONS:
 * Follow instructions in https://docs.totemmaker.net/tutorials/04.SetupController/ to install required
 * libraries and setup controller.
 * 
 * INSTRUCTIONS:
 * 0. Make sure X3-FBI board is updated to the latest firmware (using Totem App)
 * 1. Select supported Board (ESP32 Dev Module)
 * 2. Compile and upload sketch
 * 3. Open Serial monitor at speed 115200 (For PlatformIO add "monitor_speed = 115200" to project platformio.ini file)
 * 4. Press PS (middle) button on remote controller
 * 5. Wait till controller connects to ESP32 and vibrates
 * 6. Power on Mini Trooper
 * 8. Wait till ESP connects to Mini Trooper and remote vibrates.
 * 9. Press PS button to connect to another Mini Trooper
 * 
 * REMOTE USAGE:
 * Triggers - move flipper
 * PS - connect/disconnect to robot
 * Joystick push - indicate robot that belongs to joystick
 * Left Joystick - drive first connected robot 
 * Right Joystick - drive second connected robot
 * LIGHTS:
 * Green - right robot connected and ready to run
 * Blue - left robot connected and ready to run
 * Cyan - both robots connected and ready to run
 * Blinking Blue - ESP32 searching for robot to connect
 * Blinking Cyan - PS controller is trying to connect to ESP32
 * Red - Robot is disconnected
 * Off - PS controller is disconnected
 */
// PS Controller library for ESP32
#include <PS4Controller.h>
// Alias for variable
PS4Controller &PSx = PS4;
class PSController {
public:
    enum State {
        Idle,
        Welcome,
        Disconnected1,
        Disconnected2,
        Searching,
        SearchStop,
        Connected1,
        Connected2,
    };

    static void init(void (*eventFunction)());
    static void setState(State state);
    static void loop();
    static void onConnect();
    static void printBattery(int soc);
private:
    static State state;
    static uint8_t g, b;
};
// Totem Library include
#include <Totem.h>
// Mini Trooper control implementation
class MiniTrooper {
    // Helper object to control driving logic
    MotorDriver driver;
    // Remote connection handler
    TotemRobot robot;
    // Last update values
    int lastFlipperPos = 0;
    int lastBitSet = 0;
public:
    MiniTrooper() {
        // Configure MotorDriver to setup Mini Trooper wheels
        driver.addFrontLeft("motorA", 10, 100, true);
        driver.addFrontRight("motorB", 10, 100);
        driver.setTurnIntensity(50);
    }
    // Set Mini Trooper flipper possition. [0 - closed, 100 - fully open]
    void setFlipper(int position) {
        // Only write value if it's changed
        if (lastFlipperPos == position) return;
        driver.getModule().write("servoA", map(position, 0, 100, -100, 100));
        lastFlipperPos = position;
    }
    // Set Mini Trooper light bar [each bit represents a led]
    void setLightBar(int ledBits) {
        // Only write value if it's changed
        if (lastBitSet == ledBits) return;
        driver.getModule().write("leds", ledBits);
        lastBitSet = ledBits;
    }
    // Set robot move parameters [drive: -100:100, turn: -100:100]
    void move(int drive, int turn) {
        // Set motor driver to move
        driver.move(drive, turn);
        // Set light bar accordingly to move parameters
        setLightBar(moveToLightBar(drive, turn));
    }
    // Blink leds
    void indicate() {
        driver.getModule().write("indicate");
    }
    // Forward connection status
    bool isConnected() {
        return robot.isConnected();
    }
    // Forward disconnect function
    void disconnect() {
        return robot.disconnect();
    }
    void attachTo(TotemRobot &robot) {
        this->robot = robot;
        this->robot.attach(driver.getModule());
    }
    void detachFrom(TotemRobot &robot) {
        robot.detach(driver.getModule());
    }
    // Print battery voltage
    void printBattery() {
        int volt = driver.getModule().readWait("battery").getInt();
        // Display battery level
        Serial.print("Battery: ");
        Serial.print(volt/1000);
        Serial.print(".");
        Serial.print(volt%1000);
        Serial.print("V");
        if (volt > 3800) Serial.println(" (FULL)");
        else if (volt > 3100) Serial.println(" (OK)");
        else Serial.println(" (LOW) (charge Mini Trooper)");
    }
    // Run actions required when robot is connnected
    void initialize() {
        // Reset light bar
        setLightBar(0);
        // Display battery level
        printBattery();
    }
private:
    // Compute Mini Trooper led bar accordingly movement
    int moveToLightBar(int drive, int turn) {
        int ledValue = 0;
        if (turn == 0 && drive == 0) { // Stopped
            ledValue = 0b100001100001;
        }
        // Turn animation
        if (turn == 0) {} // No change
        else if (turn < 0) { // Turning left
            for (int i=0; i<=map(turn, -1, -100, 0, 10); i++) ledValue |= (1<<i);
        }
        else { // Turning right
            for (int i=11; i>=map(turn, 1, 100, 11, 1); i--) ledValue |= (1<<i);
        }
        // Drive animation
        if (drive == 0) {} // No change
        else if (drive > 0) { // Driving forward
            for (int i=5; i>=map(drive, 1, 100, 5, 1); i--) ledValue |= (1<<i);
            for (int i=6; i<=map(drive, 1, 100, 6, 10); i++) ledValue |= (1<<i);
        }
        else { // Driving backward
            for (int i=0; i<=map(drive, -1, -100, 0, 4); i++) ledValue |= (1<<i);
            for (int i=11; i>=map(drive, -1, -100, 11, 7); i--) ledValue |= (1<<i);
        }
        return ledValue;
    }
} miniTrooper[2];
// Event function called when something happend in PS controller
void onPSxEvent() {
    // Check if PS button pressed on controller
    if (PSx.event.button_down.ps) {
        // Start find if findRobot is not running
        if (!miniTrooper[0].isConnected() || !miniTrooper[1].isConnected()) {
            if (Totem.BLE.isFinding()) {
                PSController::setState(PSController::SearchStop);
                Totem.BLE.stopFind(); // Stop looking for Totem robot
                Serial.println("Find stopped");
            }
            else {
                PSController::setState(PSController::Searching);
                Totem.BLE.findRobotNoBlock(); // Start find Totem robot without waiting for result
                Serial.println("Looking for MiniTrooper robot...");
            }
        }
        else if (miniTrooper[0].isConnected() || miniTrooper[1].isConnected()) {
            miniTrooper[0].disconnect();
            miniTrooper[1].disconnect();
        }
    }
    // Indicate right trooper
    if (PSx.event.button_down.r3) miniTrooper[0].indicate();
    // Indicate left trooper
    if (PSx.event.button_down.l3) miniTrooper[1].indicate();
}
// Event function called when Totem robot is connected / disconnected to ESP32
void onRobotConnection(TotemRobot robot) {
    if (robot.isConnected()) {
        Serial.print("Connected to: ");
        Serial.println(robot.getName());
        if (!miniTrooper[0].isConnected()) { 
            miniTrooper[0].attachTo(robot);
            // On connection all TotemModule's gets attached automatically.
            // Detach unconnected module
            miniTrooper[1].detachFrom(robot);
            miniTrooper[0].initialize();
            PSController::setState(PSController::Connected1);
        }
        else if (!miniTrooper[1].isConnected()) { 
            miniTrooper[1].attachTo(robot);
            PSController::setState(PSController::Connected2);
        }
    }
    else {
        Serial.print("Disconnected from: ");
        Serial.println(robot.getName());
        if (!miniTrooper[0].isConnected())
            PSController::setState(PSController::Disconnected1);
        if (!miniTrooper[1].isConnected())
            PSController::setState(PSController::Disconnected2);
    }
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Setup PS controller library
    PSController::init(onPSxEvent);
    // Setup Totem Library
    Totem.BLE.begin(); // Start Bluetooth Low Energy interface
    Totem.BLE.attachOnConnection(onRobotConnection); // Register on connected / disconnected event
    // Proceed to loop
}

void loop() {
    // Control Totem Mini Trooper when input from controller received
    int drive = 0; // Variable that stores new value for drive power
    int steer = 0; // Variable that stores new value for steering direction
    int flipper = 0; // Variable that stores new value for flipper position
    // Joystick input. Right analog stick
    if (abs(PSx.data.analog.stick.rx) > 6) {
        steer = map(PSx.data.analog.stick.rx, -128, 127, -100, 100);
    }
    if (abs(PSx.data.analog.stick.ry) > 6) {  
        drive = map(PSx.data.analog.stick.ry, -128, 127, -100, 100);
    }
    // Control flipper
    flipper = map(PSx.data.analog.button.r2, 0, 255, 0, 100);
    // Update motors of right connected Mini Trooper
    miniTrooper[0].move(drive, steer);
    miniTrooper[0].setFlipper(flipper);
    // Reset values for second Mini Trooper
    drive = steer = flipper =  0;
    // Joystick input. Left analog stick
    if (abs(PSx.data.analog.stick.lx) > 6) {
        steer = map(PSx.data.analog.stick.lx, -128, 127, -100, 100);
    }
    if (abs(PSx.data.analog.stick.ly) > 6) {  
        drive = map(PSx.data.analog.stick.ly, -128, 127, -100, 100);
    }
    // Control flipper
    flipper = map(PSx.data.analog.button.l2, 0, 255, 0, 100);
    // Update motors of left connected Mini Trooper
    miniTrooper[1].move(drive, steer);
    miniTrooper[1].setFlipper(flipper);
    // Update controls each 50ms
    delay(50);
    // Process controller tasks
    PSController::loop();
}

// Initialize static variable
PSController::State PSController::state = PSController::Idle;
uint8_t PSController::g = 0;
uint8_t PSController::b = 0;
// Define init function
void PSController::init(void (*eventFunction)()) {
    PSx.attach(eventFunction); // Register controller event function
    PSx.attachOnConnect(onConnect); // Register controller connected function
    PSx.begin((char*)"01:02:03:04:05:06"); // Start waiting for controller connection
    Serial.println("Looking for PS4 remote... Press PS (middle) button");
}
// Define set state function
void PSController::setState(State state) {
    PSController::state = state;
}
// Define loop function
void PSController::loop() {
    if (state == Idle || !PSx.isConnected()) 
        return;

    PSx.setLed(0, 0, 0);
    PSx.setRumble(0, 0);
    PSx.setFlashRate(10, 10);
    
    switch (state) {
        case Welcome:
            PSx.setLed(0, 255, 0);
            PSx.setFlashRate(20, 30);
            PSx.setRumble(50, 10);
            PSx.sendToController();
            delay(200);
            // Start search mode right away
            PSx.setFlashRate(10, 10);
            PSx.setRumble(0, 0);
            printBattery(PSx.data.status.battery*10);
            Totem.BLE.findRobotNoBlock(); // Start find Totem robot without waiting for result
            Serial.println("Looking for Totem robot...");
            // break;
        case Searching:
            PSx.setLed(0, 0, 255);
            PSx.setFlashRate(300, 300);
            break;
        case SearchStop:
            PSx.setLed(0, g, b);
            break;
        case Disconnected1:
            g = 0;
            PSx.setLed(((g|b)==0)?255:0, g, b);
            PSx.setFlashRate(0, 0);
            PSx.sendToController();
            break;
        case Disconnected2:
            b = 0;
            PSx.setLed(((g|b)==0)?255:0, g, b);
            PSx.setFlashRate(0, 0);
            PSx.sendToController();
            break;
        case Connected1:
            g = 255;
            PSx.setLed(0, g, b);
            PSx.setRumble(50, 10);
            PSx.sendToController();
            delay(200);
            PSx.setRumble(0, 0);
        case Connected2:
            b = 255;
            PSx.setLed(0, g, b);
            PSx.setRumble(50, 10);
            PSx.sendToController();
            delay(200);
            PSx.setRumble(0, 0);
            break;
        default: break;
    }
    PSx.sendToController();
    state = Idle;
}
// Define on connect function
void PSController::onConnect() {
    Serial.println("PS4 remote found! Press PS to find Totem robot");
    PSController::setState(PSController::Welcome);
}
// Define battery state of charge print function
void PSController::printBattery(int soc) {
    Serial.print("Battery level: ");
    Serial.print(soc);
    if (soc >= 80) Serial.println("% (FULL)");
    else if (soc >= 20) Serial.println("% (OK)");
    else Serial.println("% (LOW) (charge PS controller)");
}