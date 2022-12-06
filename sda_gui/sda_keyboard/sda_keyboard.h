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
*/

#ifndef SDA_KEYBOARD_H
#define SDA_KEYBOARD_H

#include "../../SDA_OS.h"

typedef struct {
  uint8_t *key[47];
  uint8_t *keyStr[47];
  uint8_t spec[47];
} psvcKbdLayout;

void init_kblayout_standard(psvcKbdLayout *l);
void init_kblayout_uppcase(psvcKbdLayout *l);
void init_kblayout_special(psvcKbdLayout *l);
void init_kblayout_special_shift(psvcKbdLayout *l);

uint8_t sda_keyboard_set_layout(uint8_t layoutId, psvcKbdLayout *layout);

void svp_draw_keyboard(uint16_t x, uint16_t y, psvcKbdLayout *layout);
uint8_t svp_touch_keyboard(
  uint16_t x,
  uint16_t y,
  psvcKbdLayout *layout,
  uint16_t touch_x,
  uint16_t touch_y,
  uint8_t *retStr,
  gr2EventType event
);

// keyboard input handler for C
uint8_t svp_input_handler(uint8_t * str, uint16_t len, uint16_t input_id);

#endif