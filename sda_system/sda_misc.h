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

#ifndef SDA_MISC_H
#define SDA_MISC_H
#include "../SDA_OS.h"

uint8_t sda_get_keyboard_key_flag(); //returns 1 when key is ready, key is stored in global struct
uint16_t sda_get_uptime();
uint8_t sda_get_time_update_flag();
void sda_set_time_update_flag();

void sda_keyboard_show();
void sda_keyboard_hide();

void set_sda_counter(uint16_t val);
uint16_t get_sda_counter();
void sda_set_sleep_lock(uint8_t val);

void pscg_error_callback(uint8_t *str, gr2context * c);

void sda_clr_button_ev();

gr2EventType sda_wrap_get_button(uint8_t num);
void sda_wrap_clear_button(uint8_t num);
void sda_store_buttons();

void sda_draw_overlay_shadow(
  int16_t overlayX1,
  int16_t overlayY1,
  int16_t overlayX2,
  int16_t overlayY2,
  gr2context *c
);

void sda_check_fs();

#endif
