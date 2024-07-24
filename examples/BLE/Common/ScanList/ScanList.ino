#include <Arduino.h>
#include <TotemBLE.h>
/*
  ESP32 board ====> (BLE) ====> (Totem BLE Boards)
  Discover available Totem boards over Bluetooth Low Energy.
  This example will list all discovered Totem boards.
*/
TotemBLE totemBLE;
// Board found event
void onScanResult(TotemScanResult result) {
  // Print discovered board information
  Serial.printf("Address: '%s', Type: %s, Name: %s, Color: %x\n",
    result.getAddress().c_str(), result.getType().c_str(), result.getName().c_str(), result.getColor());
  // Stop scan when result is found
  // totemBLE.stop();
}
// Initialize program
void setup() {
  Serial.begin(115200);
  // Register result function
  totemBLE.addOnScanResult(onScanResult);
  // Start scan for available Totem Boards
  if (totemBLE.scan())
    Serial.printf("Start scan...\n");
  else
    Serial.println("ERROR: Scan failed to start\n");
  // Wait until scan end
  // totemBLE.wait();
  // Serial.println("Stop scan.");
}
// Loop program
void loop() {

}
