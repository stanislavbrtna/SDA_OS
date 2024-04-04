/*
Copyright (c) 2024 Stanislav Brtna

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
*/

#include "sda_keyboard_driver.h"

static enabled;

void decode(uint8_t *buff);
uint8_t sda_keyboard_driver_init();

void sda_keyboard_driver_set(uint8_t en) {
  enabled = en;
  if(en) {
    svpSGlobal.inputMethod = OTHER;
    sda_keyboard_driver_init();
  } else {
    svpSGlobal.inputMethod = ON_SCREEN_KEYBOARD;
  }
  
}

uint8_t sda_keyboard_driver_enabled() {
  return enabled;
}


uint8_t sda_keyboard_driver_init() {
  // set pins
  sda_external_pin_def(5, SDA_BASE_PIN_ALT, SDA_BASE_PIN_NOPULL);
  sda_external_pin_def(6, SDA_BASE_PIN_ALT, SDA_BASE_PIN_NOPULL);

  // enable receive
  sda_serial_recieve_init();
}


uint8_t sda_keyboard_driver_update() {
  uint8_t buff[256];
  if (!enabled) return 0;

  if(sda_serial_get_rdy() == 2) {
    sda_serial_get_str(buff);
    decode(buff);
  }

  return 0;
}


#define TYPE_PRESSED  1
#define TYPE_HOLD     2
#define TYPE_RELEASED 3

void decode(uint8_t *buff) {
  uint16_t id   = 0;
  uint8_t  type = 0;

  printf("Keyboard driver testing: %s\n", buff);
}