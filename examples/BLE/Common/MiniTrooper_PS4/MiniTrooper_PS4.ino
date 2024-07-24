#include <Arduino.h>
#include <TotemRoboBoardX3.h>
#include <TotemMiniControlBoard.h>
#include <PS4Controller.h> // https://github.com/aed3/PS4-esp32
/*
  PS4 Gamepad ====> (Bluetooth) ====> ESP32 board ====> (BLE) ====> Mini Control Board (MiniTrooper)
  Control MiniTrooper trough external ESP32 board (https://totemmaker.net/product/mini-trooper/)

  NOTE: If you have RoboBoard X3, connect PS4 Gamepad directly. Follow this example:
  https://github.com/totemmaker/arduino-examples/blob/master/kit_MiniTrooper/PS4_Controller/PS4_Controller.ino

  Controller setup guide: https://docs.totemmaker.net/remote-control/ps-controller

  1. Program ESP32 board with this example
  2. Press PS button on controller
  3. Turn on MiniTrooper
  4. Use trigger to flip, joysticks to drive around
*/

// Example is for Mini Control Board. "TotemRoboBoardX3" can be also used instead.
TotemMiniControlBoard board;
// TotemRoboBoardX3 board;

void setup() {
  Serial.begin(115200);
  /////////////////////////////////
  // Setup PS controller library
  /////////////////////////////////
  // Match MAC address stored in controller
  PS4.begin((char*)"00:02:03:04:05:06");
  // Wait for controller to connect
  Serial.println("Waiting for PS4 controller to connect...");
  while(!PS4.isConnected()) { delay(1); }
  // Wait for Mini Trooper to connect
  Serial.println("Waiting for Mini Trooper to connect...");
  board.connect();
  // Print connected robot name
  Serial.print("Connected to: ");
  Serial.println(board.getName());
  // Print battery
  Serial.print("Battery (mV): ");
  Serial.println(board.getBattery());
}

void loop() {
  // Restart board if controller has disconnected
  if (!PS4.isConnected() || !board.isConnected()) { ESP.restart(); }
  /////////////////////////////////
  // Read state of controller buttons and axis
  /////////////////////////////////
  int flipPos = map(max(PS4.data.analog.button.l2, PS4.data.analog.button.r2), 0, 255, -100, 100);
  int drive = map(-PS4.data.analog.stick.ly, -128, 127, -100, 100);
  int turn = map(PS4.data.analog.stick.rx, -128, 127, -100, 100);
  if (abs(drive) < 6) drive = 0;
  if (abs(turn) < 6) turn = 0;
  /////////////////////////////////
  // Calculate wheel power
  /////////////////////////////////
  // Calculate turn intensity
  const int turnIntensity = 50;
  int intensity = turnIntensity + ((100-turnIntensity)*abs(drive)/100);
  turn = (turn*intensity)/100;
  // Calculate motors power
  int powerA = constrain(drive+turn, -100, 100);
  int powerB = constrain(drive-turn, -100, 100);
  /////////////////////////////////
  // Update motors
  /////////////////////////////////
  board.dcSpinABCD(-powerA, powerB, 0, 0);
  board.servoSpinA(flipPos);

  delay(50);
}
