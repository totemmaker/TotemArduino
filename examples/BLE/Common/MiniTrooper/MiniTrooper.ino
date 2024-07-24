#include <Arduino.h>
#include <TotemRoboBoardX3.h>
#include <TotemMiniControlBoard.h>
/*
  ESP32 board ====> (BLE) ====> Mini Control Board (MiniTrooper)
  Control MiniTrooper trough external ESP32 board (https://totemmaker.net/product/mini-trooper/)

  NOTE: If you have RoboBoard X3, you can program it using Arduino: https://docs.totemmaker.net/setup/
*/

// Example is for Mini Control Board. "TotemRoboBoardX3" can be also used instead.
TotemMiniControlBoard board;
// TotemRoboBoardX3 board;

void setup() {
  Serial.begin(115200);
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
  // Open flipper
  board.servoSpinA(100);
  delay(1000);
  // Close flipper
  board.servoSpinA(-100);
  delay(1000);
  // Spin left
  board.dcSpinABCD(50, 50, 0, 0);
  delay(1000);
  // Spin right
  board.dcSpinABCD(-50, -50, 0, 0);
  delay(1000);
  // Stop
  board.dcBrakeABCD();
  delay(1000);
}
