/*
Copyright (c) 2021 Stanislav Brtna

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

#include "sda_screen_slots.h"

uint16_t    slotScreen[APP_SLOT_MAX];
gr2context *slotScreenContext[APP_SLOT_MAX];

uint16_t slotOnTop[APP_SLOT_MAX];
uint16_t slotValid[APP_SLOT_MAX];

uint8_t prevTopSlot;

uint16_t mainScr; // top-slot screen


void sda_slot_init(uint8_t slot, uint16_t screen, gr2context *context, uint16_t valid, uint16_t top) {
  slotScreen[slot] = screen;
  slotScreenContext[slot] = context;
  slotValid[slot] = valid;
  slotOnTop[slot] = top;
}


void sda_slot_set_screen(uint8_t slot, uint16_t screen) {
  slotScreen[slot] = screen;
  if(sda_if_slot_on_top(slot)) {
    mainScr = screen;
    setRedrawFlag();
  }
}


uint8_t sda_get_prev_top_screen_slot() {
  return prevTopSlot;
}


void sda_slot_set_valid(uint16_t slot) {
  slotValid[slot] = 1;
}


void sda_slot_set_invalid(uint16_t slot) {
  slotValid[slot] = 0;
}


uint16_t sda_slot_get_valid(uint16_t slot) {
  return slotValid[slot];
}


uint16_t sda_slot_get_screen(uint8_t slot) {
  return slotScreen[slot];
}


uint8_t sda_if_slot_on_top(uint8_t slot) {
  return slotOnTop[slot];
}


void sda_slot_on_top(uint8_t slot) {
  uint8_t x;
  if (!(slotValid[slot]) && (slot <= APP_SLOT_MAX)) {
    printf("sda_slot_on_top: invalid slot! (%u)\n", slot);
    return;
  }

  for (x = 0; x < APP_SLOT_MAX; x++) {
    slotOnTop[x] = 0;
  }

  if (slotOnTop[slot] == 0) {
    prevTopSlot = slot;
  }

  slotOnTop[slot] = 1;
  
  if ((mainScr != slotScreen[slot]) || (sda_current_con != slotScreenContext[slot])) {
    mainScr = slotScreen[slot];
    sda_current_con = slotScreenContext[slot];
    setRedrawFlag();
  }
}


uint8_t sda_get_top_slot() {
  uint8_t x;
  for (x = 0; x < APP_SLOT_MAX; x++) {
    if (slotOnTop[x] == 1 && slotValid[x]) {
      return x;
    }
  }
  return 0;
}