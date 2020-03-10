/*
 * Copyright (c) 2020 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Example how to control Totem Truck https://totemmaker.net/product/t-r-u-c-k-diy-car-kit/
 * using DUALSHOCK®4 controller. This code should run on ESP32 MCU.
 * 
 * PREPARATIONS:
 * Follow instructions in https://docs.totemmaker.net/tutorials/04.SetupController/ to install required
 * libraries and setup controller.
 * 
 * INSTRUCTIONS:
 * 1. Select supported Board (ESP32 Dev Module)
 * 2. Power on robot
 * 2. Compile and upload sketch
 * 3. Open Serial monitor at speed 115200 (For PlatformIO add "monitor_speed = 115200" to project platformio.ini file)
 * 4. Press PS (middle) button on remote controller
 * 5. Wait till controller connects to X4 and vibrates
 * 
 * REMOTE USAGE:
 * D-Pad - drive/steer robot
 * Left Joystick - steer robot
 * Right Joystick - drive robot
 * Left Trigger - brake
 * Right Trigger - accelerate
 * Square - handbrake
 * Triangle - reverse
 * Cross - light leds randomly
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
        UpdateColor,
    };

    static void init(void (*eventFunction)());
    static void setState(State state);
    static void loop();
    static void onConnect();
    static void printBattery(int soc);
private:
    static State state;
};
// Totem Library include
#include <Totem.h>
// Totem Truck control implementation
class Truck {
    // Helper object to control driving logic
    MotorDriver driver;
public:
    Truck() {
        // Configure MotorDriver to setup Truck wheels
        driver.addRearLeft("motorB", 58, 100);
        driver.addRearRight("motorA", 58, 100, true);
        driver.addServo(0, "servoA", -30, 0, 50, true);
    }
    // Set drive and brake parameters
    void move(int drivePower, int brakePower) {
        driver.move(drivePower);
        driver.brakeAll(brakePower);
        // Light up leds according to drive and braking parameters
        if (drivePower < 0)
            setLed(brakePower != 0 ? ReverseBrake : Reverse);
        else
            setLed(brakePower != 0 ? Brake : Idle);
    }
    // Steer wheels
    void steer(int position) {
        driver.moveServo(0, position);
    }
    // Blink leds
    void indicateConnected() {
        setLed(Connected);
    }
    // Print battery voltage
    void printBattery() {
        int volt = driver.getModule().readWait("battery").getInt();
        // Display battery level
        Serial.print("Battery: ");
        if (volt == 0) {
            Serial.println("Not connected");
            return;
        }
        Serial.print(volt/1000);
        Serial.print(".");
        Serial.print(volt%1000);
        Serial.print("V");
        if (volt > 12000) Serial.println(" (FULL)");
        else if (volt > 10000) Serial.println(" (OK)");
        else Serial.println(" (LOW) (charge Truck)");
    }
private:
    enum LedState {
        Idle,
        Connected,
        Brake,
        Reverse,
        ReverseBrake
    };
    // Set led color state
    void setLed(LedState state) {
        static LedState lastLedState = Idle;
        if (state == lastLedState) return;
        switch (state) {
            case Idle:
                driver.getModule().write("rgbAll/reset");
                break;
            case Connected:
                for (int i=0; i<7; i++) {
                    driver.getModule().write("rgbAll", 255, 00, 255, 00); // Set to green
                    delay(100);
                    driver.getModule().write("rgbAll", 0); // Off leds
                }
                driver.getModule().write("rgbAll/reset");
                state = Idle;
                break;
            case Brake:
                driver.getModule().write("rgbAll", 255, 255, 0, 0); // Set to red
                break;
            case Reverse:
                driver.getModule().write("rgbAll", 255, 255, 255, 255); // Set to white
                break;
            case ReverseBrake:
                driver.getModule().write("rgbA", 255, 255, 255, 255); // Set led A to white
                driver.getModule().write("rgbB", 255, 255, 0, 0); // Set led B to red
                driver.getModule().write("rgbC", 255, 255, 0, 0); // Set led C to red
                driver.getModule().write("rgbD", 255, 255, 255, 255); // Set led D to white
                break;
        }
        lastLedState = state;
    }
} truck;
// Event function called when something happend in PS controller
void onPSxEvent() {
    // On specified button press - light up certain LED colors
    if (PSx.event.button_down.cross) Totem.X4.write("rgbAll", 0xff, random(0, 255), random(0, 255), random(0, 255));
}
// Variable containing "cfg/robot/color" value
struct Color {
    uint8_t R, G, B;
} robotColor;
// Data handler that is registered with attachOnData() function
void onModuleData(ModuleData data) {
    if (data.is("cfg/robot/color")) {
        robotColor.R = (data.getInt() >> 16) & 0xFF;
        robotColor.G = (data.getInt() >> 8) & 0xFF;
        robotColor.B = (data.getInt() >> 0) & 0xFF;
        // If color not configured - set to green
        if (robotColor.R == 0 && robotColor.G == 0 && robotColor.R == 0) {
            robotColor.G = 255;
        }
        PSController::setState(PSController::UpdateColor);
    }
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    // Setup PS controller library
    PSController::init(onPSxEvent);
    // Setup Totem Library
    Totem.X4.begin(); // Initialize X4 board
    Totem.X4.attachOnData(onModuleData); // Data handler for subscribed functions
    Totem.X4.subscribe("cfg/robot/color"); // Subscription for "cfg/robot/color" command
    Totem.X4.read("cfg/robot/color"); // Read robot color to show on remote
    // Print battery
    truck.printBattery();
    // Proceed to loop
}

void loop() {
    // Control Totem Truck when input from controller received
    int drive = 0; // Variable that stores new value for drive power
    int steer = 0; // Variable that stores new value for steering direction
    int brake = 0; // Variable that stores new value for braking power
    // Joystick input. Left analog stick
    if (abs(PSx.data.analog.stick.lx) > 6) {
        steer = map(PSx.data.analog.stick.lx, -128, 127, -100, 100);
    }
    // Joystick input. Right analog stick
    if (abs(PSx.data.analog.stick.ry) > 6) {  
        drive = map(PSx.data.analog.stick.ry, -128, 127, -100, 100);
        // Compensate stick when wigling to sides
        int side = map(PSx.data.analog.stick.rx, -128, 127, -100, 100);
        if (abs(drive) > abs(side)) drive = constrain(drive+(abs(side)*(drive/abs(drive))), -100, 100); 
    }
    // Drive with buttons
    if (PSx.data.button.left)  steer -= 80;
    if (PSx.data.button.right) steer += 80;
    if (PSx.data.button.up)    drive += 80;
    if (PSx.data.button.down)  drive -= 80;
    // Control braking
    brake = map(PSx.data.analog.button.l2, 0, 255, 0, 100);
    // If square button is pressed - act as handbrake
    if (PSx.data.button.square) brake = 100;
    // Control acceleration with trigger
    int triggerDrive = map(PSx.data.analog.button.r2, 0, 255, 0, 100);
    // If square button is pressed - reverse drive
    if (PSx.data.button.triangle) {
        if (triggerDrive < 20) triggerDrive = 20;
        triggerDrive *= -1;
    }
    // Update drive value with trigger input
    drive = constrain(drive+triggerDrive, -100, 100);
    // Update motors
    truck.move(drive, brake);
    truck.steer(steer);
    // Update controls each 50ms
	delay(50);
    // Process controller tasks
    PSController::loop();
}

// Initialize static variable
PSController::State PSController::state = PSController::Idle;
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
            PSx.setLed(robotColor.R, robotColor.G, robotColor.B);
            PSx.setRumble(50, 10);
            PSx.sendToController();
            printBattery(PSx.data.status.battery*10);
            delay(200);
            PSx.setRumble(0, 0);
            break;
        case UpdateColor:
            PSx.setLed(robotColor.R, robotColor.G, robotColor.B);
            break;
        default: break;
    }
    PSx.sendToController();
    state = Idle;
}
// Define on connect function
void PSController::onConnect() {
    Serial.println("PS4 remote found! Press PS to find Totem robot");
    truck.indicateConnected();
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