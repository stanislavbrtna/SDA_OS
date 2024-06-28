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

uint16_t slotValid[APP_SLOT_MAX];
uint8_t prevSlot[APP_SLOT_MAX];
uint8_t topSlot;

uint16_t mainScr; // top-slot screen


void sda_slot_init(uint8_t slot, uint16_t screen, gr2context *context, uint16_t valid, uint16_t top) {
  slotScreen[slot] = screen;
  slotScreenContext[slot] = context;
  slotValid[slot] = valid;

  if (top == 1) {
    topSlot = slot;
  }
}


void sda_slot_set_screen(uint8_t slot, uint16_t screen) {
  slotScreen[slot] = screen;
  if(sda_if_slot_on_top(slot)) {
    mainScr = screen;
    setRedrawFlag();
  }
}


uint8_t sda_get_prev_top_screen_slot() {
  return prevSlot[topSlot];
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
  if(slot == topSlot && slotValid[slot]) {
    return 1;
  }
  
  return 0;  
}


void sda_slot_on_top(uint8_t slot) {
  if(slot >= APP_SLOT_MAX) {
    printf("sda_slot_on_top: slot out of range! (%u)\n", slot);
    return;
  }

  if (slotValid[slot] == 0) {
    printf("sda_slot_on_top: invalid slot! (%u)\n", slot);
    return;
  }

  if (topSlot != slot) {
    prevSlot[slot] = topSlot;
    topSlot = slot;
  }

  if ((mainScr != slotScreen[slot]) || (sda_current_con != slotScreenContext[slot])) {
    mainScr = slotScreen[slot];
    sda_current_con = slotScreenContext[slot];
    setRedrawFlag();
  }
}


void sda_prev_slot_on_top(uint8_t def) {

  if (slotValid[prevSlot[topSlot]] == 0) {
    topSlot = def;
  } else {
    topSlot = prevSlot[topSlot];
  }

  if ((mainScr != slotScreen[topSlot]) || (sda_current_con != slotScreenContext[topSlot])) {
    mainScr = slotScreen[topSlot];
    sda_current_con = slotScreenContext[topSlot];
    setRedrawFlag();
  }
}


uint8_t sda_get_top_slot() {
  if (slotValid[topSlot] == 1) {
    return topSlot;
  }
  
  printf("Warn: no valid screen slot!\n");

  return 0;
}