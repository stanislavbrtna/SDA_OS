/*
Copyright (c) 2022 Stanislav Brtna

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

#include "sda_scr_btn_handler.h"

uint8_t sda_screen_button_handler(uint16_t screen_id, uint16_t back_id, gr2context *con) {

  if (sda_wrap_get_button(BUTTON_RIGHT) != EV_NONE) {
    gr2_keypad_input(GR2_BUTTON_RIGHT, sda_wrap_get_button(BUTTON_RIGHT), screen_id, con);
    sda_wrap_clear_button(BUTTON_RIGHT);
  }
  
  if (sda_wrap_get_button(BUTTON_LEFT) != EV_NONE) {
    gr2_keypad_input(GR2_BUTTON_LEFT, sda_wrap_get_button(BUTTON_LEFT), screen_id, con);
    sda_wrap_clear_button(BUTTON_LEFT);
  }

  if (sda_wrap_get_button(BUTTON_UP) != EV_NONE) {
    gr2_keypad_input(GR2_BUTTON_UP, sda_wrap_get_button(BUTTON_UP), screen_id, con);
    sda_wrap_clear_button(BUTTON_UP);
  }
  if (sda_wrap_get_button(BUTTON_DOWN) != EV_NONE) {
    gr2_keypad_input(GR2_BUTTON_DOWN, sda_wrap_get_button(BUTTON_DOWN), screen_id, con);
    sda_wrap_clear_button(BUTTON_DOWN);
  }

  if (back_id != 0 && con->pscgElements[back_id].valid == 1 && gr2_get_visible(back_id, con) && sda_wrap_get_button(BUTTON_A) != EV_NONE) {
    gr2_set_event(back_id, sda_wrap_get_button(BUTTON_A), con);
    sda_wrap_clear_button(BUTTON_A);
  }

  if (sda_wrap_get_button(BUTTON_B) != EV_NONE) {
    gr2_keypad_input(GR2_BUTTON_OK, sda_wrap_get_button(BUTTON_B), screen_id, con);
    sda_wrap_clear_button(BUTTON_B);
  }
}