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

#include "sda_tray.h"

extern volatile uint8_t irq_redraw;


static void draw_home_button(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t active) {
  tray_draw_tab(x1, y1, x2, y2, active);
  LCD_DrawText_ext(x1 + 20, y1 + 2, gr2_get_text_color(&sda_sys_con), (uint8_t *)"S!");
}

int16_t sda_tray_home(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
  static uint8_t init;
  static uint8_t clickOld;
  static uint8_t click;
  uint8_t curr_font;
  static uint16_t holdCounter;

  curr_font = LCD_Get_Font_Size();
  LCD_Set_Sys_Font(32);

  if((irq_redraw) || (init == 0) || ((svpSGlobal.systemOptClick == CLICKED_NONE) && clickOld == 1 && click == 0)) {
    draw_home_button(x1, y1, x2, y2, 0);
    init = 1;
  }

  if (sda_tray_clicked(x1, y1, x2, y2)) {
    if (clickOld == 0) {
      draw_home_button(x1, y1, x2, y2, 1);
    }

    if (holdCounter > OPT_HOLD_CNT_BEGIN && holdCounter <= OPT_HOLD_CNT_MAX) {
      
      LCD_FillRect(
        x1 + 2,
        y1 + 2,
        x1 - 2 + ((x2 - x1) / (OPT_HOLD_CNT_MAX - OPT_HOLD_CNT_BEGIN)) * (holdCounter - OPT_HOLD_CNT_BEGIN),
        y2 - 2,
        gr2_get_active_color(&sda_sys_con)
      );

      LCD_FillRect(
        x1 + 2 + ((x2 - x1) / (OPT_HOLD_CNT_MAX - OPT_HOLD_CNT_BEGIN)) * (holdCounter - OPT_HOLD_CNT_BEGIN),
        y1 + 2,
        x2 - 2,
        y2 - 2,
        gr2_get_fill_color(&sda_sys_con)
      );

      if (holdCounter == OPT_HOLD_CNT_MAX) {
        LCD_FillRect(x1 + 2, y1 + 2, x2 - 2, y2 - 2, gr2_get_active_color(&sda_sys_con));
      }

      LCD_DrawText_ext(x1 + 20, y1 + 2, gr2_get_text_color(&sda_sys_con), (uint8_t *)"S!");
    }

    click = 1;
    holdCounter++;
  } else {
    click = 0;
  }
  LCD_Set_Sys_Font(curr_font);

  if (holdCounter < OPT_HOLD_CNT_MAX && click == 0 && clickOld == 1 && svpSGlobal.systemOptClick == CLICKED_NONE) {
    svpSGlobal.systemOptClick = CLICKED_SHORT;
    init = 0;
    holdCounter = 0;
  }

  if (holdCounter > OPT_HOLD_CNT_MAX && svpSGlobal.systemOptClick == CLICKED_NONE && click == 1) {
    svpSGlobal.systemOptClick = CLICKED_LONG;
  }

  if (holdCounter > 0 && click == 0 && clickOld == 1 ) {
    init = 0;
    holdCounter = 0;
  }

  clickOld = click;

  return 0;
}