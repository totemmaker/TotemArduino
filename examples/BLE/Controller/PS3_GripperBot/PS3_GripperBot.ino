/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to control Totem Gripper Bot https://totemmaker.net/product/gripper-bot-smartphone-app-controlled-car/
 * using DUALSHOCK®3 controller. This code should run on ESP32 MCU.
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
 * 6. Power on robot
 * 8. Wait till ESP connects to robot and remote vibrates.
 * 
 * REMOTE USAGE:
 * Bumpers (L1,R1) - close/open gripper
 * Triggers (L2,R2) - move gripper precisely
 * D-Pad - drive robot
 * PS - connect/disconnect to robot
 * Left Joystick - control robot driving
 * Right Joystick - control robot arm
 * LIGHTS:
 * led 4-1 light dancing - PS controller connected to ESP32
 * led 4 on - PS controller connected to ESP32, robot disconnected
 * led 1 blink - ESP32 searching for robot to connect
 * led 1 on - Robot connected and ready to run
 * led off - PS controller is disconnected
 */
// PS Controller library for ESP32
#include <Ps3Controller.h>
// Alias for variable
Ps3Controller &PSx = Ps3;
class PSController {
public:
    enum State {
        Idle,
        Welcome,
        Disconnected,
        Searching,
        Connected,
    };

    static void init(void (*eventFunction)());
    static void setState(State state);
    static void loop();
    static void onConnect();
    static void printBattery(int soc);
private:
    static State state;
    static ps3_cmd_t cmd;
    static void update();
    static void setLed(uint8_t led);
    static void setRumble(uint8_t left, uint8_t right);
};
// Totem Library include
#include <Totem.h>
// Gripper Bot control implementation
class GripperBot {
    // Helper object to control driving logic
    MotorDriver driver;
public:
    int gripperPos = 0;
    GripperBot() {
        // Configure MotorDriver to setup Gripper Bot motors
        driver.addFrontLeft("motorC", 9, 100, true);
        driver.addFrontRight("motorD", 9, 100);
        driver.setTurnIntensity(70);
        driver.addServo(0, "servoA", 10, 55, 94); // Gripper
        driver.addServo(1, "servoB", -70, 50, 90, true); // Arm
    }
    // Set Gripper Bot Arm possition. [-100 - fully retracted, 100 - fully extended]
    void setArm(int position) {
        driver.moveServo(1, position);
        // Set light bar according to arm position
        position = constrain(position, -100, 100);
        position = map(position, -100, 100, 0, 11);
        setLightBar((1<<position));
    }
    // Set Gripper Bot Gripper possition. [-100 - fully open, 100 - fully closed]
    void setGripper(int position) {
        driver.moveServo(0, position);
        gripperPos = constrain(position, -100, 100);
    }
    // Set Gripper Bot light bar [each bit represents a led]
    void setLightBar(int ledBits) {
        static int lastBitSet = 0;
        // Only write value if it's changed
        if (lastBitSet == ledBits) return;
        driver.getModule().write("leds", ledBits);
        lastBitSet = ledBits;
    }
    // Set robot move parameters [drive: -100:100, turn: -100:100]
    void move(int drive, int turn) {
        // Set motor driver to move
        driver.move(drive, turn);
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
        else Serial.println(" (LOW) (charge Gripper Bot)");
    }
} gripperBot;
// Event function called when something happend in PS controller
void onPSxEvent() {
    // Check if PS button pressed on PS3 controller
    if (PSx.event.button_down.ps) { // Called once when PS button is pressed
        if (Totem.BLE.getConnectedLast().isConnected()) {
            Totem.BLE.getConnectedLast().disconnect();
        }
        else {
            // Start find if findRobot is not running
            if (!Totem.BLE.isFinding()) {
                PSController::setState(PSController::Searching);
                Totem.BLE.findRobotNoBlock(); // Start find Totem robot without waiting for result
                Serial.println("Looking for Totem robot...");
            }
            else {
                PSController::setState(PSController::Disconnected);
                Totem.BLE.stopFind(); // Stop looking for Totem robot
                Serial.println("Find stopped");
            }
        }
    }
    // Open gripper on button press
    if (PSx.event.button_down.r1) {
        gripperBot.setGripper(100);
    }
    // Close gripper on button press
    if (PSx.event.button_down.l1) {
        gripperBot.setGripper(-100);
    }
}
// Event function called when Totem robot is connected / disconnected
void onRobotConnection(TotemRobot robot) {
    if (robot.isConnected()) {
        // Set remote state to Connected
        PSController::setState(PSController::Connected);
        // Print robot name
        Serial.print("Connected to: ");
        Serial.println(robot.getName());
        // Reset led bar on X3-FBI board
        gripperBot.setLightBar(0);
        // Display battery level
        gripperBot.printBattery();
    }
    else {
        PSController::setState(PSController::Disconnected);
        Serial.print("Disconnected from: ");
        Serial.println(robot.getName());
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

int lastTriggerValue;
void loop() {
    // Control Totem Gripper Bot when input from controller received
    int drive = 0; // Variable that stores new value for drive power
    int steer = 0; // Variable that stores new value for steering direction
    // Joystick input. Left analog stick
    if (abs(PSx.data.analog.stick.lx) > 6) {
        steer = map(PSx.data.analog.stick.lx, -128, 127, -100, 100);
    }
    // Joystick input. Right analog stick
    if (abs(PSx.data.analog.stick.ly) > 6) {  
        drive = -map(PSx.data.analog.stick.ly, -128, 127, -100, 100);
    }
    // Drive with buttons
    if (PSx.data.button.left)  steer -= 60;
    if (PSx.data.button.right) steer += 60;
    if (PSx.data.button.up)    drive += 60;
    if (PSx.data.button.down)  drive -= 60;
    // Control arm
    if (abs(PSx.data.analog.stick.ry) > 6) {  
        gripperBot.setArm(-map(PSx.data.analog.stick.ry, -128, 127, -100, 100));
    }
    else gripperBot.setArm(0);
    // Control gripper precise
    if (PSx.data.analog.button.r2 > 0) {
        int trigger = map(PSx.data.analog.button.r2, 1, 255, 1, 150);
        if (trigger > lastTriggerValue) {
            gripperBot.setGripper(gripperBot.gripperPos+(trigger-lastTriggerValue));
            lastTriggerValue = trigger;
        }
    }
    else if (PSx.data.analog.button.l2 > 0) {
        int trigger = map(PSx.data.analog.button.l2, 1, 255, 1, 150);
        if (trigger > lastTriggerValue) {
            gripperBot.setGripper(gripperBot.gripperPos-(trigger-lastTriggerValue));
            lastTriggerValue = trigger;
        }
    }
    else lastTriggerValue = 0;
    // Update Gripper Bot motors
    gripperBot.move(drive, steer); // Update motors
    // Update controls each 50ms
	delay(50);
    // Process controller tasks
    PSController::loop();
}

// Initialize static variable
PSController::State PSController::state = PSController::Idle;
ps3_cmd_t PSController::cmd = {0};
// Define init function
void PSController::init(void (*eventFunction)()) {
    PSx.attach(eventFunction); // Register controller event function
    PSx.attachOnConnect(onConnect); // Register controller connected function
    PSx.begin((char*)"01:02:03:04:05:06"); // Start waiting for controller connection
    Serial.println("Looking for PS3 remote... Press PS (middle) button");
}
// Define set state function
void PSController::setState(State state) {
    PSController::state = state;
}
// Define loop function
void PSController::loop() {
    if (state == Idle || !PSx.isConnected()) 
        return;

    int localState = state;
    state = Idle;
    
    switch (localState) {
        case Welcome:
            for (int t=1; t<=2; t++) {
                setRumble(255, 0);
                for (int i=4; i>=1; i--) {
                    setLed(i);
                    update();
                    delay(60);
                    setRumble(0, 0);
                }
                setRumble(0, 255);
                for (int i=1; i<=4; i++) {
                    setLed(i);
                    update();
                    delay(60);
                    setRumble(0, 0);
                }
            }
            update();
            printBattery(PSx.data.status.battery*20);
            Totem.BLE.findRobotNoBlock(); // Start find Totem robot without waiting for result
            Serial.println("Looking for Totem robot...");
            // break;
        case Searching:
            setLed(1);
            update();
            delay(200);
            setLed(0);
            update();
            delay(200);
            if (state == PSController::Idle)
                state = PSController::Searching;
            break;
        case Disconnected:
            setLed(4);
            update();
            break;
        case Connected:
            setRumble(128, 128);
            setLed(1);
            update();
            delay(200);
            setRumble(0, 0);
            update();
            break;
        default: break;
    }
}
// Define on connect function
void PSController::onConnect() {
    Serial.println("PS3 remote found! Press PS to find Totem robot");
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
void PSController::update() {
    ps3Cmd(cmd);
}
void PSController::setLed(uint8_t led) {
    cmd.led1 = led == 1;
    cmd.led2 = led == 2;
    cmd.led3 = led == 3;
    cmd.led4 = led == 4;
}
void PSController::setRumble(uint8_t left, uint8_t right) {
    cmd.rumble_left_intensity = left;
    cmd.rumble_right_intensity = right;
}