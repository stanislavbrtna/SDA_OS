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
extern uint16_t trayBackgroundColor;
extern volatile uint8_t systemDateClick;


int16_t sda_tray_time(int16_t x2, int16_t y1, uint16_t w) {
  static uint8_t redraw;
  static uint8_t time_string[6];
  static uint8_t oldmin;
  static uint8_t oldhour;
  uint8_t curr_font;

  int16_t x1 = x2 - w;

  if((redraw == 0) || (irq_redraw)) {
    time_string[0] = svpSGlobal.hour / 10 + 48;
    time_string[1] = svpSGlobal.hour % 10 + 48;
    time_string[2] = ':';
    time_string[3] = svpSGlobal.min / 10 + 48;
    time_string[4] = svpSGlobal.min % 10 + 48;
    time_string[5] = 0;

    oldmin = svpSGlobal.min;
    oldhour = svpSGlobal.hour;
    LCD_FillRect(x1 - 1 - 32, y1, x1 + w, 31, trayBackgroundColor);
    curr_font = LCD_Get_Font_Size();
    LCD_Set_Sys_Font(18);
    LCD_DrawText_ext(x1 + 2, y1 + 8, gr2_get_text_color(&sda_sys_con), time_string);
    LCD_Set_Sys_Font(curr_font);
    redraw = 1;
    irq_redraw = 1;
  }

  if (sda_tray_clicked(x1 - 1, y1, x1 + w, y1 + 31) == EV_RELEASED) {
    systemDateClick = 1;
  }

  if((oldmin != svpSGlobal.min) || (oldhour != svpSGlobal.hour)) {
    redraw = 0;
  }

  return w;
}