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
extern volatile uint8_t systemBattClick;

int16_t sda_tray_battery(int16_t x2, int16_t y1, int16_t w) {
  static uint8_t redraw;
  static uint8_t batt_string[5];
  static uint8_t oldbatt;
  static systemPwrType oldBattState;

  int16_t x1 = x2 - w;

  uint8_t curr_font;

  if((redraw == 0) || (irq_redraw)) {
    // constructing battery string
    if (0 != svpSGlobal.battPercentage / 100) {
      batt_string[0] = svpSGlobal.battPercentage / 100 + 48;
    } else {
      batt_string[0] = ' ';
    }

    if (0 != svpSGlobal.battPercentage / 10) {
      batt_string[1] = (svpSGlobal.battPercentage / 10) % 10 + 48;
    } else {
      batt_string[1] = ' ';
    }

    batt_string[2] = svpSGlobal.battPercentage % 10 + 48;
    batt_string[3] = '%';
    batt_string[4] = 0;

    oldbatt = svpSGlobal.battPercentage;
    oldBattState = svpSGlobal.pwrType;

    // unknown value
    if(svpSGlobal.battPercentage == 101) {
      batt_string[0] = ' ';
      batt_string[1] = ' ';
      batt_string[2] = '?';
      batt_string[3] = '%';
      batt_string[4] = 0;
    }

    LCD_FillRect(x1, y1, x2, 31, trayBackgroundColor);
    curr_font = LCD_Get_Font_Size();
    LCD_Set_Sys_Font(18);
    if (svpSGlobal.pwrType == POWER_USB) {
      LCD_DrawText_ext(x1, y1 + 8, gr2_get_text_color(&sda_sys_con), IRQ_BATT_CHRG);
    } else {
      LCD_DrawText_ext(x1, y1 + 8, gr2_get_text_color(&sda_sys_con), batt_string);
    }

    if (svpSGlobal.battPercentage != 101) {
      LCD_FillRect(x1 + 8,
        y1 + 31 - 6,
        x1 + (int16_t)(((float)w - 4)*((float)svpSGlobal.battPercentage/(float)100)),
        y1 + 31 - 2,
        gr2_get_active_color(&sda_sys_con)
      );
    }

    LCD_DrawRectangle(x1 + 8, y1 + 31 - 6, x1 + w - 4, y1 + 31 - 2, gr2_get_border_color(&sda_sys_con));
    LCD_Set_Sys_Font(curr_font);
    redraw = 1;
  }

  if (sda_tray_clicked(x1 - 1, y1, x1 + w, y1 + 31) == EV_RELEASED) {
    systemBattClick = 1;
  }

  if ((oldbatt != svpSGlobal.battPercentage) || (oldBattState != svpSGlobal.pwrType)) {
    redraw = 0;
  }

  return w;
}