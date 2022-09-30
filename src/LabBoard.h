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
#ifndef LIB_LABBOARD
#define LIB_LABBOARD

#include <Arduino.h>

class LabBoard {
public:
    // Invalid voltage reading
    const float invalid = -100.0;
    // Key names
    enum {
        KEY_MINUS,
        KEY_PLUS,
        KEY_RIGHT,
        KEY_MIDDLE,
        KEY_LEFT,
    };
    // LED names
    enum {
        LED_ALL,
        LED_DIG1,
        LED_DIG2,
        LED_50V,
        LED_5V,
        LED_05V,
        LED_DAC1,
        LED_DAC2,
        LED_DAC3,
        LED_VIN,
        LED_VREG,
        LED_mAmp,
    };

    struct Voltage {
        // Read VIN pin voltage.
        // Returns: (float) `6.0` - `30.0` V | `-100.0` - invalid.
        float getVIN();
        // Read ±50v pin voltage.
        // Returns: (float) `-50.0` - `50.0` V | `-100.0` - invalid.
        float get50V();
        // Read ±5v pin voltage.
        // Returns: (float) `-6.15` - `6.15` V | `-100.0` - invalid.
        float get5V();
        // Read ±0.5v pin voltage.
        // Returns: (float) `-0.7` - `0.7` V | `-100.0` - invalid.
        float get05V();
        // Read SHUNT pin current.
        // Returns: (float) `0.0` - `0.8` A | `-100.0` - invalid.
        float getAmp();

        // Write VREG pin voltage.
        // Maximum output voltage depends on VIN voltage.
        // `voltage`: (float) `3.0` - `VIN - 1.0` V.
        void setVREG(float voltage);
        // Write DAC1 pin voltage.
        // `voltage`: (float) `0.0` - `3.25` V.
        void setDAC1(float voltage);
        // Write DAC2 pin voltage.
        // `voltage`: (float) `0.0` - `3.25` V.
        void setDAC2(float voltage);
        // Write DAC3 pin voltage.
        // `voltage`: (float) `0.0` - `3.25` V.
        void setDAC3(float voltage);
        
        // Read VREG pin voltage.
        // Returns: (float) `3.0` - `VIN - 1.0` V.
        float getVREG();
        // Read DAC1 pin voltage.
        // Returns: (float) `0.0` - `3.25` V.
        float getDAC1();
        // Read DAC2 pin voltage.
        // Returns: (float) `0.0` - `3.25` V.
        float getDAC2();
        // Read DAC3 pin voltage.
        // Returns: (float) `0.0` - `3.25` V.
        float getDAC3();
    } volt;

    struct TXD {
        // Stop signal generator on `TXD` pin.
        void stop();
        // Start signal generator on `TXD` pin.
        void start();
        // Start signal generator on `TXD` pin and stop after number of pulses elapsed.
        // Configured with with `setBurstCount(count)`.
        void startBurst();
        // Write amount of pulses to output during burst mode.
        // Generator will stop when configured number is reached.
        // `count`: `0` - `65535`.
        void setBurstCount(uint16_t count);
        // Write output signal frequency in hertz.
        // `frequency`: `1` - `1000000` Hz
        void setFrequency(uint32_t frequency);
        // Write output signal duty cycle in percentage. 0.1 precision.
        // `percentage`: (float) `0.0` - `100.0` %
        void setDutyCycle(float percentage);
        // Write output signal frequency (period) in microseconds.
        // `period`: `1` - `1000000` μs
        void setPeriod(float period);
        // Write output signal duty cycle (pulse width) in microseconds.
        // Value can't be larger than period!
        // `pulse`: `0` - `1000000` μs
        void setPulseWidth(float pulse);

        // Read if generator is running in burst mode.
        bool isBurst();
        // Read if generator is running.
        bool isRunning();
        // Read output signal frequency in hertz.
        // Returns: `1` - `1000000` Hz
        uint32_t getFrequency();
        // Read output signal duty cycle in percentage.
        // Returns: (float) `0.0` - `100.0` %
        float getDutyCycle();
        // Read output signal frequency (period) in microseconds.
        // Returns: `1` - `1000000` μs
        float getPeriod();
        // Read output signal duty cycle (pulse width) in microseconds.
        // Returns: `0` - `1000000` μs
        float getPulseWidth();
    } txd;

    struct RXD {
        // Stop signal monitor on `DIG1` pin.
        void stop();
        // Start signal monitor on `DIG1` pin.
        void start();
        // Read input signal frequency in hertz.
        // Returns: `0` - `23000000` Hz
        uint32_t getFrequency();
        // Read input signal frequency (period) in microseconds.
        // Returns: `0.04` - `1000000.0` μs
        float getPeriod() { return 1.0f / getFrequency(); }
        // Read number of signal pulses elapsed.
        // Returns: number
        uint32_t getCount();
        // Reset pulse counter to 0 (value returned by `getCount()`).
        void resetCount();

        // Write sample (detect) edge. Default: HIGH
        // `edge`:
        // `0` - LOW edge (falling)
        // `1` - HIGH edge (rising)
        void setSampleEdge(uint8_t edge);
        // Read sample (detect) edge.
        // Returns:
        // `0` - LOW edge (falling)
        // `1` - HIGH edge (rising)
        uint8_t getSampleEdge();
    } rxd;

    // Read LabBoard pin `DIG1` digital state.
    // Returns: `0` - LOW | `1` - HIGH
    uint8_t getDIG1();
    // Read LabBoard pin `DIG2` digital state.
    // Returns: `0` - LOW | `1` - HIGH
    uint8_t getDIG2();
    
    struct Display {
        // Write value to display. Aligned to left.
        // `value`: any value or string.
        template<typename T1> void print(T1 value) {
            write("DISP:TXT", value);
        }
        // Write value to display. Aligned to left. Allows to set writing start point.
        // `offset`: number of segments to push from left.
        // `value`: any value or string.
        template<typename T1> void print(uint8_t offset, T1 value) {
            write("DISP:TXT", offset, value);
        }
        // Clear display (set to empty).
        void clear() { print(""); }
        // Write whole display blinking rate in milliseconds.
        // `rate`: `0` - `1000` ms | `0`- stop blink.
        void setBlink(uint16_t rate);
        // Write specific segment blinking rate in milliseconds.
        // `segment`: `1` - `9` number from left.
        // `rate`: `0` - `1000` ms | `0` - stop blink.
        void setBlink(uint8_t segment, uint16_t rate);
        // Write binary map of segments group to set blinking rate in milliseconds.
        // `map`: `B000000000` - `B111111111` | `0x0` - `0x1FF`.
        // `rate`: `0` - `1000` ms | `0` - stop blink.
        void setBlinkBinary(uint16_t map, uint16_t rate);
        // Write display brightness.
        // `brightness`: `0` - `15`
        void setBrightness(uint8_t brightness);
        // Write serial monitor feature state (on / off).
        // Will print all data from `Serial.println()` to display. Default: on.
        // `state`: `0` - off | `1` - on
        void setMonitor(uint8_t enabled);
    } display;

    struct LED {
        // Write specific LED to turn on.
        // `number`: `1` - `11` | `0` - all LED.
        void on(uint8_t num = 0) { set(num, HIGH); }
        // Write specific LED to turn off.
        // `number`: `1` - `11` | `0` - all LED.
        void off(uint8_t num = 0) { set(num, LOW); }
        // Write specific LED state (on / off).
        // `number`: `1` - `11` | `0` - all LED.
        // `state`: `0` - off | `1` - on.
        void set(uint8_t num, uint8_t state);
        // Read specific LED state.
        // `number`: `1` - `11` | `0` - all LED.
        // Returns: `0` - off | `1` - on.
        uint8_t get(uint8_t num);
        // Write binary map of turned on LED.
        // `map`: `B00000000000` - `B11111111111` | `0x0` - `0x7FF`
        void setBinary(uint16_t map);
        // Read binary map of turned on LED.
        // Returns: `B00000000000` - `B11111111111` | `0x0` - `0x7FF`
        uint16_t getBinary();
    } led;
    
    struct Key {
        // Read specific key state.
        // Returns:  `0` - not pressed | `1` - is pressed
        uint8_t get(uint8_t num);
        // Read binary map of pressed keys.
        // Returns: `B00000` - `B11111` | `0x0` - `0x1F`
        uint16_t getBinary();
    } key;

    struct Config {
        // Write configuration (setting) value.
        // `name`: setting name ("DISP")
        // `value`: setting value
        void set(const char name[], int32_t value);
        // Read configuration (setting) value.
        // `name`: setting name ("DISP")
        // Returns: setting value
        int32_t get(const char name[]);
    } config;

    // Restart LabBoard into boot mode (for firmware update).
    void runBoot();
    // Restart LabBoard
    void restart();
    
private:
    template<typename T1>
    static void write(const char *cmd, T1 param1){
        Serial.print("LB:");
        Serial.print(cmd);
        Serial.print(':');
        Serial.println(param1);
    }

    template<typename T1, typename T2>
    static void write(const char *cmd, T1 param1, T2 param2){
        Serial.print("LB:");
        Serial.print(cmd);
        Serial.print(':');
        Serial.print(param1);
        Serial.print(':');
        Serial.println(param2);
    }
};

#define LB _getLabBoardInstance()

LabBoard& _getLabBoardInstance();

#endif /* LIB_LABBOARD */
