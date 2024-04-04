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
  uint8_t  c[3];
  uint8_t  spec = 0;

  c[0] = 0;
  c[1] = 0;
  c[2] = 0;

  if(buff[0] != "$") return;

  if(buff[1] != "P") type = TYPE_PRESSED;
  if(buff[1] != "H") type = TYPE_HOLD;
  if(buff[1] != "R") type = TYPE_RELEASED;

  id = (buff[2] - '0') * 10 + (buff[3] - '0');

  if(buff[5] == '!') {
    c[0] = buff[4];
  } else if (buff[6] == '!') {
    c[0] = buff[4];
    c[1] = buff[5];
  } else {
    spec = 1;
  }

  if (spec == 0 && type == TYPE_PRESSED) {
    sda_str_add(svpSGlobal.kbdKeyStr, c);
  }

  // manage shifts



  // push to the keyboard string

  printf("Keyboard driver testing: %s, id: %u, type: %u, c: %s\n", buff, id, type, c);
}