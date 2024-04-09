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

#include "../../SDA_OS.h"

// max 255 chars limit
uint8_t svp_input_handler(uint8_t * str, uint16_t len, uint16_t input_id) {
  uint16_t x = 0;
  uint8_t buff[256];

  // string lenght
  while(str[x] != 0) {
    x++;
    if (x == len) {
      return 0;
    }
  }

  if (gr2_get_value(input_id, &sda_sys_con) && !gr2_get_grayout(input_id, &sda_sys_con)) {
    // set the cursor position
    if (((gr2_get_event(input_id, &sda_sys_con) == EV_PRESSED)
        || (gr2_get_event(input_id, &sda_sys_con) == EV_HOLD))
        && gr2_text_get_pwd(input_id, &sda_sys_con) == 0) {
      uint16_t temp;
      uint8_t curr_font;
      curr_font = LCD_Get_Font_Size();
      LCD_Set_Sys_Font(gr2_get_param2(input_id, &sda_sys_con));

      temp = LCD_Text_Get_Cursor_Pos(str, gr2_get_tmx(&sda_sys_con), gr2_get_tmy(&sda_sys_con));

      LCD_Set_Sys_Font(curr_font);

      if (temp == 0) {
        gr2_set_param(input_id, 0, &sda_sys_con);
      } else {
        gr2_set_param(input_id, temp, &sda_sys_con);
      }
    }
    gr2_set_event(input_id, EV_NONE, &sda_sys_con);

    if (sda_get_keyboard_key_flag()) {
        if (*((uint8_t *)svpSGlobal.kbdKeyStr) != 8) {
          // TODO: Fix this
          sda_str_insert(str, svpSGlobal.kbdKeyStr, buff,  gr2_get_param(input_id, &sda_sys_con), len);

          if (*((uint8_t *)svpSGlobal.kbdKeyStr + 1) != 0) {
            gr2_set_param(input_id, gr2_get_param(input_id, &sda_sys_con) + 2, &sda_sys_con); // move cursor
          } else {
            gr2_set_param(input_id, gr2_get_param(input_id, &sda_sys_con) + 1, &sda_sys_con);
          }
          return 1;
        } else {
          uint16_t len = 0;

          while(str[len] != 0) {
            len++;
          }

          if (len > 0) {
            uint16_t prac;
            uint8_t czFlag = 0;

            if (len >= 2
                && (str[gr2_get_param(input_id, &sda_sys_con) - 2] >= 0xC3)
                && (str[gr2_get_param(input_id, &sda_sys_con) - 2] <= 0xC5)
                ) {
              gr2_set_param(input_id, gr2_get_param(input_id, &sda_sys_con) - 2, &sda_sys_con);
              czFlag = 1;
            } else {
              gr2_set_param(input_id, gr2_get_param(input_id, &sda_sys_con) - 1, &sda_sys_con);
              czFlag = 0;
            }
            prac = gr2_get_param(input_id, &sda_sys_con);

            x = 0;

            while(str[x] != 0) {
              str[prac + x] = str[prac + x + 1 + czFlag];
              x++;
            }

            str[x] = 0; // add null terminator, just to be sure
            gr2_set_str(input_id, str, &sda_sys_con); // set/update
            return 1;
          }
        }
      }
    }
    if (gr2_get_str(input_id, &sda_sys_con) != str && !gr2_get_grayout(input_id, &sda_sys_con)) {
      gr2_set_str(input_id, str, &sda_sys_con); // still set/update
    }
    return 0;
}


uint8_t sda_kbd_input_ret_detect(uint8_t * str, uint16_t len) {
  uint16_t x = 0;

  // string lenght
  while(str[x] != 0) {
    x++;
    if (x == len) {
      return 0;
    }
  }

  if (x == 0) return 0;

  if (str[x - 1] == '\n') {
    str[x - 1] = 0;
    return 1;
  }
}