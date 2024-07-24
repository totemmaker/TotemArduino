#include <Arduino.h>
#include <TotemMiniControlBoard.h>
#include <TotemRoboBoardX3.h>
#include <TotemRoboBoardX4.h>
/*
  ESP32 board ====> (BLE) ====> (Totem BLE Boards)
  Example to connect multiple Totem boards.
*/
TotemMiniControlBoard miniboard_car;
TotemMiniControlBoard miniboard_robot;
TotemRoboBoardX3 roboboardX3;
TotemRoboBoardX4 roboboardX4;
// Initialize program
void setup() {
  Serial.begin(115200);
  // Connect to all boards
  Serial.println("Looking for Mini Control Board with name 'My Car'...");
  // Connect Mini Control Board
  miniboard_car.connectName("My Car");
  Serial.println("Looking for Mini Control Board with name 'My Robot'...");
  // Connect Mini Control Board
  miniboard_robot.connectName("My Robot");
  Serial.println("Looking for RoboBoard X3...");
  // Connect RoboBoard X3
  roboboardX3.connect();
  Serial.println("Looking for RoboBoard X4...");
  // Connect RoboBoard X4
  roboboardX4.connect();
  // Connected
  Serial.println("All boards connected");
}
// Loop program
void loop() {
  // Control boards individually
  miniboard_car.rgbColor(125,0,0); // Red
  delay(1000);
  miniboard_robot.rgbColor(125,125,0); // Yellow
  delay(1000);
  roboboardX3.rgbColor(0,125,0); // Green
  delay(1000);
  roboboardX4.rgbColor(0,0,125); // Blue
  delay(2000);
  // Off
  miniboard_car.rgbColor(0);
  miniboard_robot.rgbColor(0);
  roboboardX3.rgbColor(0);
  roboboardX4.rgbColor(0);
  delay(2000);
}
