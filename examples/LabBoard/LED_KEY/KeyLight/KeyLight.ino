/**
 * Show pressed key on display.
 * Light LED near pressed key.
 */
#include <Totem.h>

void setup() {
  // Set serial baud rate to 57600
  Serial.begin(57600);
  // Control TotemDuino LED
  pinMode(13, OUTPUT);
  // Turn all LED off
  LB.led.off();
  // Prepare display
  LB.display.print("KEy      ");
}

int lastKeys = 0;
void loop() {
  // Read pressed LabBoard keys
  int keyBin = LB.key.getBinary();
  // Return if keys state wasn't changed
  if (keyBin == lastKeys) return;
  // Remember last keys state
  lastKeys = keyBin;
  // Prepare display to print pressed keys
  LB.display.print("KEy      ");
  int index = 8;
  // Print pressed keys
  if (keyBin & (1<<LabBoard::KEY_MINUS)) LB.display.print(index--, "-");
  if (keyBin & (1<<LabBoard::KEY_PLUS)) LB.display.print(index--, "+");
  if (keyBin & (1<<LabBoard::KEY_RIGHT)) LB.display.print(index--, "R");
  if (keyBin & (1<<LabBoard::KEY_MIDDLE)) LB.display.print(index--, "M");
  if (keyBin & (1<<LabBoard::KEY_LEFT)) LB.display.print(index--, "L");
  int ledBinary = 0;
  // Turn on LED near pressed keys
  if (keyBin & (1<<LabBoard::KEY_LEFT)) ledBinary |= 0x1C; // 3,4,5
  if (keyBin & (1<<LabBoard::KEY_MIDDLE)) ledBinary |= 0xE0; // 6,7,8
  if (keyBin & (1<<LabBoard::KEY_RIGHT)) ledBinary |= 0x700; // 9,10,11
  // Write LabBoard LED
  LB.led.setBinary(ledBinary);
  // Toggle TotemDuino LED when LabBoard button is pressed
  digitalWrite(13, digitalRead(13) ? LOW : HIGH);
}
