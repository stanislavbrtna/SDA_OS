/*
Copyright (c) 2018 Stanislav Brtna

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

File description:
	This file stores platform-dependent data types.
*/

#ifndef SDA_BASE_H
#define SDA_BASE_H

// system random
uint32_t svp_random();

//time&alarm functions
void set_svp_counter(uint16_t val);
uint16_t get_svp_counter();

uint8_t getTimeUpdateFlag();
void setTimeUpdateFlag();
uint16_t svp_get_uptime();

// [HW] beeper
void svp_beep();
void svp_beep_set_t(uint16_t time);
void svp_beep_set_pf(uint16_t val);
void svp_beep_set_def();

// [HW] led
void led_set_pattern(ledPatternType pat);
void sda_set_led(uint8_t set);

// [HW] clock management
void system_clock_set_low(void);
void system_clock_set_normal(void);

// [HW] LCD
void svp_set_lcd_state(lcdStateType state);
void svp_set_backlight(uint8_t val);
void sda_calibrate();

// [HW] Expansion
uint8_t sda_serial_recieve(uint8_t *str, uint32_t len, uint32_t timeout);
void sda_serial_transmit(uint8_t *str, uint32_t len);
uint8_t sda_serial_is_enabled();
void sda_serial_enable();
void sda_serial_disable();
void sda_internal_pin_def(uint8_t pinNum, uint8_t pinType, uint8_t pull);
void sda_internal_pin_set(uint8_t pinNum, uint8_t val);
uint8_t sda_internal_pin_get(uint8_t pinNum);
void sda_external_pin_def(uint8_t pinNum, uint8_t pinType, uint8_t pull);
void sda_external_pin_set(uint8_t pinNum, uint8_t val);
uint8_t sda_external_pin_get(uint8_t pinNum);
float sda_external_ADC_get();

// [HW] Debug serial port
uint8_t sda_dbg_serial_is_enabled();
void sda_dbg_serial_disable();
void sda_dbg_serial_enable();

// [HW] System
void svp_set_calibration_data(touchCalibDataStruct input);
uint8_t svp_getLcdCalibrationFlag();

#endif
