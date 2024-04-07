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

static uint8_t enabled;

static uint8_t kbd_icon[] = {
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0xfc,0xff,0xff,0x3f,0xfc,0xff,0xff,0x3f,
  0xfc,0xff,0xff,0x3f,0x8c,0x31,0xc6,0x38,
  0x8c,0x31,0xc6,0x38,0x8c,0x31,0xc6,0x38,
  0xfc,0xff,0xff,0x3f,0xfc,0xff,0xff,0x3f,
  0x3c,0xc6,0x18,0x3f,0x3c,0xc6,0x18,0x3f,
  0x3c,0xc6,0x18,0x3f,0xfc,0xff,0xff,0x3f,
  0xfc,0xff,0xff,0x3f,0x8c,0x1,0x8c,0x31,
  0x8c,0x1,0x8c,0x31,0x8c,0x1,0x8c,0x31,
  0xfc,0xff,0xff,0x3f,0xfc,0xff,0xff,0x3f,
  0xfc,0xff,0xff,0x3f,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
};


void sda_extensions_detect() {
  static uint8_t lock_pre;
  static uint8_t init;
  uint8_t lock;
  if (sda_resource_get_lock(EXTERNAL_EXPANSION_PORT, 0) == SDA_LOCK_UNLOCKED
      && sda_resource_get_lock(SERIAL_PORT, 0) == SDA_LOCK_UNLOCKED
  ){
    lock = 0;
  } else {
    lock = 1;
  }

  if ((lock == 0 && lock_pre == 1) || init == 0) {
    init = 1;

    // init serial

    // set pins
    sda_external_pin_def(5, SDA_BASE_PIN_ALT, SDA_BASE_PIN_NOPULL);
    sda_external_pin_def(6, SDA_BASE_PIN_ALT, SDA_BASE_PIN_NOPULL);

    // enable receive
    sda_serial_recieve_init();
  }

  if (lock == 0) {
    // listen to spec char
    if (sda_serial_get_spec()) {
      uint8_t buff[256];
      
      sda_serial_get_str(buff);

      // compare 
      uint16_t i = 0;
      while(buff[i] != ':') {
        i++;
        if (i > 255) break;
        if (buff[i] == 0) break;
      }
      i++;
      buff[i + 6] = 0;

      //printf("testy test: %s\n", buff + i);

      if(svp_strcmp(buff + i, "KBD001")) {
        printf("Keyboard addon detected!");
        sda_keyboard_driver_set(1);
      }
    }
  }

  lock_pre = lock;
}

void decode(uint8_t *buff);
uint8_t sda_keyboard_driver_init();

void sda_keyboard_driver_set(uint8_t en) {
  static uint8_t ico_id;

  if (en == enabled) return;

  enabled = en;
  if(en) {
    svpSGlobal.inputMethod = OTHER;
    sda_keyboard_driver_init();
    
    sda_resource_claim(EXTERNAL_EXPANSION_PORT, 0);
    sda_resource_claim(SERIAL_PORT, 0);
    ico_id = sda_custom_icon_set(kbd_icon, 0, (uint8_t*)"");

  } else {
    svpSGlobal.inputMethod = ON_SCREEN_KEYBOARD;
    sda_tray_kbd_enable(0);
    sda_resource_free(EXTERNAL_EXPANSION_PORT, 0);
    sda_resource_free(SERIAL_PORT, 0);

    sda_custom_icon_release_spot(ico_id);
  }
  
}

uint8_t sda_keyboard_driver_enabled() {
  return enabled;
}


uint8_t sda_keyboard_driver_init() {
  // set pins
  sda_external_pin_def(5, SDA_BASE_PIN_ALT, SDA_BASE_PIN_NOPULL);
  sda_external_pin_def(6, SDA_BASE_PIN_ALT, SDA_BASE_PIN_NOPULL);
  // detect pin
  sda_external_pin_def(4, SDA_BASE_PIN_IN, SDA_BASE_PIN_PULLDOWN);

  // enable receive
  sda_serial_recieve_init();

  return 0;
}


uint8_t sda_keyboard_driver_update() {
  uint8_t buff[256];
  if (!enabled) return 0;

  if(sda_serial_get_rdy() == 2) {
    sda_serial_get_str(buff);
    decode(buff);
  }

  if(sda_external_pin_get(4) == 0) {
    sda_keyboard_driver_set(0);
  } 

  return 0;
}


#define TYPE_PRESSED  1
#define TYPE_HOLD     2
#define TYPE_RELEASED 3

void ev_handler(uint8_t button, uint8_t kbd_ev) {
  if (kbd_ev == TYPE_PRESSED)  svpSGlobal.keyEv[button] = EV_PRESSED;
  if (kbd_ev == TYPE_HOLD)     svpSGlobal.keyEv[button] = EV_HOLD;
  if (kbd_ev == TYPE_RELEASED) svpSGlobal.keyEv[button] = EV_RELEASED;
}

void decode(uint8_t *buff) {
  uint16_t id   = 0;
  uint8_t  type = 0;
  uint8_t  c[3];
  uint8_t  spec = 0;

  c[0] = 0;
  c[1] = 0;
  c[2] = 0;

  if(buff[0] != '$') return;

  if(buff[1] == 'P') type = TYPE_PRESSED;
  if(buff[1] == 'H') type = TYPE_HOLD;
  if(buff[1] == 'R') type = TYPE_RELEASED;

  id = (buff[2] - '0') * 10 + (buff[3] - '0');

  if(buff[4] != '$') return;

  if(buff[6] == '!') {
    c[0] = buff[5];
  } else if (buff[7] == '!') {
    c[0] = buff[5];
    c[1] = buff[6];
  } else {
    spec = 1;
  }

  //printf("Kbd dbg: id: %s -end\n", buff);

  if(id > 80) {
    printf("Kbd Error: failed to decode: %s -end\n", buff);
    return;
  }

  // normal characters
  if (spec == 0 && type == TYPE_PRESSED) {
    sda_strcp(c, svpSGlobal.kbdKeyStr, sizeof(svpSGlobal.kbdKeyStr));
    svpSGlobal.kbdFlag = 1;
    return;
  }

  // special chars
  if (id == 27 && type == TYPE_PRESSED) { // bksp
    sda_strcp("\b", svpSGlobal.kbdKeyStr, sizeof(svpSGlobal.kbdKeyStr));
    svpSGlobal.kbdFlag = 1;
    return;
  }

  if (id == 14 && type == TYPE_PRESSED) { // del
    sda_strcp("\bd", svpSGlobal.kbdKeyStr, sizeof(svpSGlobal.kbdKeyStr));
    svpSGlobal.kbdFlag = 1;
    return;
  }

  if (id == 52) { //ent
    if (type == TYPE_PRESSED) {
      svpSGlobal.keyEv[BUTTON_A] = EV_PRESSED;
      sda_strcp("\n", svpSGlobal.kbdKeyStr, sizeof(svpSGlobal.kbdKeyStr));
      svpSGlobal.kbdFlag = 1;
    }

    if (type == TYPE_HOLD)     svpSGlobal.keyEv[BUTTON_B] = EV_HOLD;
    if (type == TYPE_RELEASED) svpSGlobal.keyEv[BUTTON_B] = EV_RELEASED;

    return;
  }
  
  if (id == 28 && type == TYPE_PRESSED) { // tab
    sda_strcp("  ", svpSGlobal.kbdKeyStr, sizeof(svpSGlobal.kbdKeyStr));
    svpSGlobal.kbdFlag = 1;
    return;
  }
  
  if (id == 0) { // esc
    ev_handler(BUTTON_A, type);
    return;
  }

  if (id == 79) { // right
    ev_handler(BUTTON_RIGHT, type);
    return;
  }

  if (id == 77) { // left
    ev_handler(BUTTON_LEFT, type);
    return;
  }

  if (id == 65) { // up
    ev_handler(BUTTON_UP, type);
    return;
  }

  if (id == 78) { // Down
    ev_handler(BUTTON_DOWN, type);
    return;
  }

  // manage shifts


}
