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

  if (getOverlayScreen() != 0 && getOverlayScreen() != screen_id) {
    return 0;
  }

  if (svpSGlobal.lcdState == LCD_OFF) {
    return 0;
  }

  // Text field handler
  if (con->textActive) {
    if (sda_wrap_get_button(BUTTON_RIGHT) == EV_PRESSED  || sda_wrap_get_button(BUTTON_LEFT) == EV_PRESSED ) {
      if(sda_keyboard_driver_get_shift()) {
        if ((con->textBlockStart == 0 || con->textBlockEnd == 0)) {
          con->textBlockStart = gr2_get_param(con->textActiveId, con) + 1;
        }
      } else {
        con->textBlockStart = 0;
        con->textBlockEnd = 0;
      }
    }
    

    if (sda_wrap_get_button(BUTTON_RIGHT) == EV_PRESSED) {
      uint8_t *str = gr2_get_str(con->textActiveId, con);

      if(sda_strlen(str) > gr2_get_param(con->textActiveId, con)) {
        if ((str[gr2_get_param(con->textActiveId, con)] >= 0xC3) && (str[gr2_get_param(con->textActiveId, con)] <= 0xC5)) {
          gr2_set_param(con->textActiveId, gr2_get_param(con->textActiveId, con) + 2, con);
        } else {
          gr2_set_param(con->textActiveId, gr2_get_param(con->textActiveId, con) + 1, con);
        }
      }
    }

    if (sda_wrap_get_button(BUTTON_LEFT) == EV_PRESSED) {
      if(gr2_get_param(con->textActiveId, con) != 0) {
        uint8_t *str = gr2_get_str(con->textActiveId, con);

        if (gr2_get_param(con->textActiveId, con) >= 2 && (str[gr2_get_param(con->textActiveId, con) - 2] >= 0xC3)
            && (str[gr2_get_param(con->textActiveId, con) - 2] <= 0xC5)
        ){
          gr2_set_param(con->textActiveId, gr2_get_param(con->textActiveId, con) - 2, con);
        } else {
          gr2_set_param(con->textActiveId, gr2_get_param(con->textActiveId, con) - 1, con);
        }
      }
    }

    if(sda_keyboard_driver_get_shift()) {
      if(con->textBlockStart != 0) {
        if(gr2_get_param(con->textActiveId, con) > con->textBlockStart) {
          con->textBlockEnd = gr2_get_param(con->textActiveId, con);
        } else {
          if(con->textBlockEnd == 0) {
            con->textBlockEnd = con->textBlockStart - 1;
          }
          con->textBlockStart = gr2_get_param(con->textActiveId, con);
        }
      }
    }
    
    if (sda_wrap_get_button(BUTTON_A) == EV_RELEASED) {
      gr2_text_deactivate(con);
      sda_keyboard_hide();
    }

    sda_wrap_clear_button(BUTTON_LEFT);
    sda_wrap_clear_button(BUTTON_RIGHT);
    sda_wrap_clear_button(BUTTON_A);

    return 0;
  }

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

  if (sda_wrap_get_button(BUTTON_A) != EV_NONE) {
    static uint64_t holdCnt;
    if (back_id != 0
        && con->pscgElements[back_id].valid == 1
        && gr2_get_visible(back_id, con)
        && gr2_get_grayout(back_id, con) == 0
        && holdCnt == 0
    ) {
      gr2_set_event(back_id, sda_wrap_get_button(BUTTON_A), con);
    } else {
      
      if (sda_wrap_get_button(BUTTON_A) == EV_PRESSED) {
        holdCnt = svpSGlobal.uptimeMs;
      }
      if (sda_wrap_get_button(BUTTON_A) == EV_HOLD
          && holdCnt != 0
          && (holdCnt + 800) <= svpSGlobal.uptimeMs
      ) {
        svpSGlobal.systemOptClick = CLICKED_LONG;
      }
      if (sda_wrap_get_button(BUTTON_A) == EV_RELEASED) {
        if ((holdCnt + 800) >= svpSGlobal.uptimeMs) {
          svpSGlobal.systemOptClick = CLICKED_SHORT;
        }
        holdCnt = 0;
      }
    }
    sda_wrap_clear_button(BUTTON_A);
  }

  if (sda_wrap_get_button(BUTTON_B) != EV_NONE) {
    uint8_t r;
    r = gr2_keypad_input(GR2_BUTTON_OK, sda_wrap_get_button(BUTTON_B), screen_id, con);

    svpSGlobal.kbdKeyStr[0] = 0; // reset keyboard string (if keyboard was activated with ent key)

    if (r == 2 && svpSGlobal.inputMethod == ON_SCREEN_KEYBOARD) {
      sda_keyboard_show();
    }
    sda_wrap_clear_button(BUTTON_B);
  }

  return 0;
}