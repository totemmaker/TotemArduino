/*
 * Copyright (c) 2021 TotemMaker. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.  
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */
/* DESCRIPTION:
 * Minimal code required to upload program to X4.
 * This code is handy if you need to update X4 firmware
 * or upload empty program to not interfere with controlling
 * over Bluetooth.
 * 
 * INSTRUCTIONS:
 * 1. Connect X4 board to PC.
 * 2. Upload code.
 */
// Totem Library include
#include <Totem.h>
// Arduino setup function.
void setup() {
    // Initialize X4 module
    Totem.X4.begin();
}
// Arduino loop function
void loop() {
    // Do nothing
    delay(1000);
}