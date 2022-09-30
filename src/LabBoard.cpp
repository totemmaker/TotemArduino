/* 
 * Copyright (c) 2022 TotemMaker.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */
#include "LabBoard.h"

LabBoard& _getLabBoardInstance() {
    static LabBoard instance;
    return instance;
}

static int32_t read_serial(const char *cmd, bool isHex = false) {
    // Send read request
    Serial.flush();
    Serial.print("LB:");
    Serial.print(cmd);
    Serial.println(":?");
    // Wait for response
    int32_t result = 0;
    if (Serial.find(const_cast<char*>(cmd))) {
        char hexStr[15];
        int read = Serial.readBytesUntil('\n', hexStr, sizeof(hexStr));
        if (read < 2) return 0;
        hexStr[read] = '\0';
        result = strtol(hexStr+1, NULL, isHex ? 16 : 10);
    }
    return result;
}

// LabBoard::Voltage
float LabBoard::Voltage::getVIN() {
    return ((float)read_serial("IN:VIN")) / 1000;
}
float LabBoard::Voltage::get50V() {
    return ((float)read_serial("IN:50V")) / 1000;
}
float LabBoard::Voltage::get5V() {
    return ((float)read_serial("IN:5V")) / 1000;
}
float LabBoard::Voltage::get05V() {
    return ((float)read_serial("IN:05V")) / 1000;
}
float LabBoard::Voltage::getAmp() {
    return read_serial("IN:AMP");
}
void LabBoard::Voltage::setVREG(float voltage) {
    write("OUT:VREG", (int32_t)(voltage * 1000));
}
void LabBoard::Voltage::setDAC1(float voltage) {
    write("OUT:DAC1", (int32_t)(voltage * 1000));
}
void LabBoard::Voltage::setDAC2(float voltage) {
    write("OUT:DAC2", (int32_t)(voltage * 1000));
}
void LabBoard::Voltage::setDAC3(float voltage) {
    write("OUT:DAC3", (int32_t)(voltage * 1000));
}
float LabBoard::Voltage::getVREG() {
    return ((float)read_serial("OUT:VREG")) / 1000;
}
float LabBoard::Voltage::getDAC1() {
    return ((float)read_serial("OUT:DAC1")) / 1000;
}
float LabBoard::Voltage::getDAC2() {
    return ((float)read_serial("OUT:DAC2")) / 1000;
}
float LabBoard::Voltage::getDAC3() {
    return ((float)read_serial("OUT:DAC3")) / 1000;
}
// LabBoard::TXD
void LabBoard::TXD::stop() {
    write("TXD:RUN", 0);
}
void LabBoard::TXD::start() {
    write("TXD:RUN", 1);
}
void LabBoard::TXD::startBurst() {
    write("TXD:RUN", 2);
}
void LabBoard::TXD::setBurstCount(uint16_t count) {
    write("TXD:CNT", count);
}
void LabBoard::TXD::setFrequency(uint32_t frequency) {
    write("TXD:FHZ", frequency);
}
void LabBoard::TXD::setDutyCycle(float percentage) {
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;
    write("TXD:DPCT", (int32_t)(percentage * 10));
}
void LabBoard::TXD::setPeriod(float period) {
    if (period < 0) period = 0;
    write("TXD:FUS", (int32_t)(period * 1000000));
}
void LabBoard::TXD::setPulseWidth(float pulse) {
    if (pulse < 0) pulse = 0;
    write("TXD:DUS", (int32_t)(pulse * 1000000));
}
bool LabBoard::TXD::isBurst() {
    return read_serial("TXD:RUN") == 2;
}
bool LabBoard::TXD::isRunning() {
    return read_serial("TXD:RUN") != 0;
}
uint32_t LabBoard::TXD::getFrequency()  {
    return read_serial("TXD:FHZ");
}
float LabBoard::TXD::getDutyCycle() {
    return ((float)read_serial("TXD:DPCT")) / 10;
}
float LabBoard::TXD::getPeriod()  {
    return ((float)read_serial("TXD:FUS")) / 1000000;
}
float LabBoard::TXD::getPulseWidth() {
    return ((float)read_serial("TXD:DUS")) / 1000000;
}
// LabBoard::RXD
void LabBoard::RXD::stop() {
    write("RXD:RUN", (int)0);
}
void LabBoard::RXD::start() {
    write("RXD:RUN", (int)1);
}
uint32_t LabBoard::RXD::getFrequency() {
    return read_serial("RXD:FHZ");
}
uint32_t LabBoard::RXD::getCount() {
    return read_serial("RXD:CNT");
}
void LabBoard::RXD::resetCount() {
    write("RXD:CNT", "0");
}
void LabBoard::RXD::setSampleEdge(uint8_t edge) {
    write("RXD:EDGE", edge ? "1" : "0");
}
uint8_t LabBoard::RXD::getSampleEdge() {
    return read_serial("RXD:EDGE") ? HIGH : LOW;
}
// LabBoard::Display
void LabBoard::Display::setBlink(uint16_t rate) {
    write("DISP:BLI", rate);
}
void LabBoard::Display::setBlink(uint8_t segment, uint16_t rate) {
    if (segment == 0)
        setBlink(rate);
    else
        setBlinkBinary((1<<(segment-1)), rate);
}
void LabBoard::Display::setBlinkBinary(uint16_t map, uint16_t rate) {
    write("DISP:BLI", map, rate);
}
void LabBoard::Display::setBrightness(uint8_t brightness) {
    if (brightness > 15) brightness = 15;
    write("DISP:DIM", brightness);
}
void LabBoard::Display::setMonitor(uint8_t enabled) {
    write("DISP:MON", enabled);
}
// LabBoard::LED
void LabBoard::LED::set(uint8_t num, uint8_t state) {
    write("LED", num, state ? "1" : "0");
}
uint8_t LabBoard::LED::get(uint8_t num) {
    return !!(getBinary() & (1 << num));
}
void LabBoard::LED::setBinary(uint16_t map) {
    Serial.print("LB:LED:");
    Serial.println(map, HEX);
}
uint16_t LabBoard::LED::getBinary() {
    return read_serial("LED", true);
}
// LabBoard::Key
uint8_t LabBoard::Key::get(uint8_t num) {
    return !!(getBinary() & (1 << num));
}
uint16_t LabBoard::Key::getBinary() {
    return read_serial("KEY", true);
}
// LabBoard::Config
void LabBoard::Config::set(const char name[], int32_t value) {
    write(name, value);
}
int32_t LabBoard::Config::get(const char name[]) {
    char cmd[10] = "CFG:";
    strncpy(cmd+4, name, sizeof(cmd)-4);
    return read_serial(cmd);
}
// LabBoard
uint8_t LabBoard::getDIG1() {
    return read_serial("DIG1") ? HIGH : LOW;
}
uint8_t LabBoard::getDIG2() {
    return read_serial("DIG2") ? HIGH : LOW;
}
void LabBoard::runBoot() {
    write("BOOT", 1);
}
void LabBoard::restart() {
    write("RST", 1);
}
