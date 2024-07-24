#include <Arduino.h>
#include <TotemBLE.h>
#include <TotemRoboBoardX3.h>
/*
  ESP32 board ====> (BLE) ====> (Totem BLE Boards)
  Find RoboBoardX3 with name "My Robot" and connect to it.
  This example how to discover and connect a board.
        Same result can be done directly with "connectName" function.
  Check "ScanList" example to list all available boards.
*/
TotemBLE totemBLE;
TotemRoboBoardX3 roboboard;
// Initialize program
void setup() {
  /**
   * NOTE: this is example of TotemBLE "find" functionality.
   * All of it can be replaced with single line:
   * roboboard.connectName("My Robot");
   * (see TotemRoboBoardX3 example)
   */
  Serial.begin(115200);
  /**
   * Find RoboBoard X3 with name "My Robot".
   * Ignore everything else.
   */
  Serial.println("Looking for RoboBoard X3...");
  // Look for RoboBoard X3 with specific name "My Robot"
  TotemScanResult result = totemBLE.findRoboBoardX3("My Robot");
  /**
   * Board is found and stored in "result"
   */
  // Print discovered board information
  Serial.print("Address: "); Serial.println(result.getAddress());
  Serial.print("Type: "); Serial.println(result.getType());
  Serial.print("Name: "); Serial.println(result.getName());
  Serial.print("Color: "); Serial.println(result.getColor(), HEX);
  /**
   * (optional) Connect to "result"
   * NOTE: board type should match the one it's connecting.
   */
  // Connect to discovered board
  if (roboboard.connectAddress(result.getAddress()))
    Serial.print("Connected to "); Serial.println(roboboard.getName());
  else
    Serial.println("Connection failed");
}
// Loop program
void loop() {

}
