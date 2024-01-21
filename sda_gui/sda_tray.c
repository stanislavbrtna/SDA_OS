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

#include "sda_tray.h"

volatile uint8_t irq_redraw;
volatile uint8_t irq_redraw_block;
extern volatile uint8_t systemBattClick;
extern volatile uint8_t systemDateClick;
static uint8_t alarm_string[3];
uint16_t trayBackgroundColor;


static void tray_time(int16_t x1, int16_t y1, uint16_t w);
static void tray_alarm(int16_t x1, int16_t y1, int16_t w);
static void tray_battery(int16_t x1, int16_t y1, int16_t w);
static uint8_t tray_close(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t reset);
static uint8_t tray_home(int16_t x1, int16_t y1, int16_t x2, int16_t y2);


void svp_set_irq_redraw() {
  irq_redraw = 1;
}


void sda_tray_alarm_enable() {
  alarm_string[0] = 'A';
  alarm_string[1] = 0;
  svp_set_irq_redraw();
}


void sda_tray_alarm_disable() {
  alarm_string[0] = ' ';
  alarm_string[1] = 0;
  svp_set_irq_redraw();
}


void irq_redraw_block_enable() {
  irq_redraw_block = 1;
}


void irq_redraw_block_disable() {
  irq_redraw_block = 0;
}


static gr2EventType tray_clicked(
  uint16_t x1,
  uint16_t y1,
  uint16_t x2,
  uint16_t y2
) {
  static uint16_t touch_origin_x;
  static uint16_t touch_origin_y;

  if (svpSGlobal.touchType == EV_PRESSED) {
    touch_origin_x = svpSGlobal.touchX;
    touch_origin_y = svpSGlobal.touchY;
  }

  if ((svpSGlobal.touchX > x1) &&
      (svpSGlobal.touchX < x2) &&
      (svpSGlobal.touchY > y1) &&
      (svpSGlobal.touchY < y2)
  ) {
    if (svpSGlobal.touchType != EV_NONE) {
      if ((touch_origin_x > x1) &&
          (touch_origin_x < x2) &&
          (touch_origin_y > y1) &&
          (touch_origin_y < y2)
        ) {
        return svpSGlobal.touchType;
        }
    }
  }

  return EV_NONE;
}


static void tray_time(int16_t x1, int16_t y1, uint16_t w) {
  static uint8_t redraw;
  static uint8_t time_string[6];
  static uint8_t oldmin;
  static uint8_t oldhour;
  uint8_t curr_font;

  if((redraw == 0) || (irq_redraw)) {
    time_string[0] = svpSGlobal.hour / 10 + 48;
    time_string[1] = svpSGlobal.hour % 10 + 48;
    time_string[2] = ':';
    time_string[3] = svpSGlobal.min / 10 + 48;
    time_string[4] = svpSGlobal.min % 10 + 48;
    time_string[5] = 0;

    oldmin = svpSGlobal.min;
    oldhour = svpSGlobal.hour;
    LCD_FillRect(x1 - 1, y1, x1 + w, 31, trayBackgroundColor);
    curr_font = LCD_Get_Font_Size();
    LCD_Set_Sys_Font(18);
    LCD_DrawText_ext(x1, y1 + 8, gr2_get_text_color(&sda_sys_con), time_string);
    LCD_Set_Sys_Font(curr_font);
    redraw = 1;
  }

  if (tray_clicked(x1 - 1, y1, x1 + w, y1 + 31) == EV_RELEASED) {
    systemDateClick = 1;
  }

  if((oldmin != svpSGlobal.min) || (oldhour != svpSGlobal.hour)) {
    redraw = 0;
  }
}


static void tray_alarm(int16_t x1, int16_t y1, int16_t w) {
  static uint8_t redraw;

  uint8_t curr_font;

  if((redraw == 0) || (irq_redraw)) {
    LCD_FillRect(x1 - 1, y1, x1 + w, 31, trayBackgroundColor);
    curr_font = LCD_Get_Font_Size();
    LCD_Set_Sys_Font(18);
    LCD_DrawText_ext(x1, y1 + 8, gr2_get_text_color(&sda_sys_con), alarm_string);
    LCD_Set_Sys_Font(curr_font);
    redraw = 1;
  }
}


static void tray_battery(int16_t x1, int16_t y1, int16_t w) {
  static uint8_t redraw;
  static uint8_t batt_string[5];
  static uint8_t oldbatt;
  static systemPwrType oldBattState;

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

    LCD_FillRect(x1 - 1, y1, x1 + w, 31, trayBackgroundColor);
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

  if (tray_clicked(x1 - 1, y1, x1 + w, y1 + 31) == EV_RELEASED) {
    systemBattClick = 1;
  }

  if ((oldbatt != svpSGlobal.battPercentage) || (oldBattState != svpSGlobal.pwrType)) {
    redraw = 0;
  }
}


static uint8_t tray_close(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t reset) {
  static uint8_t init;
  static uint8_t selected;
  uint8_t touch_event;

  if (reset == 2) {
    if (tray_clicked(x1, y1, x2, y2) == EV_PRESSED) {
      if (svpSGlobal.systemXBtnTime == 0) {
        svpSGlobal.systemXBtnTime = svpSGlobal.timestamp;
      }
    }
    return 0;
  }

  if((init == 0) || (reset == 1) || (irq_redraw)) {
    LCD_FillRect(x1, y1, x2, y2, gr2_get_fill_color(&sda_sys_con));
    LCD_DrawRectangle(x1, y1, x2, y2, gr2_get_border_color(&sda_sys_con));
    LCD_FillRect(x1 + 6, y1 + 14, x2 - 6, y2 - 14, gr2_get_text_color(&sda_sys_con));
    init = 1;
  }

  touch_event = tray_clicked(x1, y1, x2, y2);

  if (touch_event == EV_PRESSED) {
    LCD_FillRect(x1, y1, x2, y2, gr2_get_active_color(&sda_sys_con));
    LCD_DrawRectangle(x1, y1, x2, y2, gr2_get_border_color(&sda_sys_con));
    LCD_FillRect(x1 + 6, y1 + 14, x2 - 6, y2 - 14, gr2_get_text_color(&sda_sys_con));
    selected = 1;
    if (svpSGlobal.systemXBtnTime == 0) {
      svpSGlobal.systemXBtnTime = svpSGlobal.timestamp;
    }
  }

  if (touch_event == EV_NONE && selected) {
    selected = 0;
    init = 0;
  }

  if ((touch_event == EV_RELEASED) && (svpSGlobal.systemXBtnClick == 0)) {
    svpSGlobal.systemXBtnClick = 1;
    svpSGlobal.systemXBtnTime = 0;
    init = 0;
  }

  return 0;
}

#define OPT_HOLD_CNT_MAX 12
#define OPT_HOLD_CNT_BEGIN 5

static uint8_t tray_home(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
  static uint8_t init;
  static uint8_t clickOld;
  static uint8_t click;
  uint8_t curr_font;
  static uint16_t holdCounter;

  curr_font = LCD_Get_Font_Size();
  LCD_Set_Sys_Font(32);

  if((irq_redraw) || (init == 0) || ((svpSGlobal.systemOptClick == CLICKED_NONE) && clickOld == 1 && click == 0)) {
    LCD_FillRect(x1, y1, x2, y2, gr2_get_fill_color(&sda_sys_con));
    LCD_DrawRectangle(x1, y1, x2, y2, gr2_get_border_color(&sda_sys_con));
    LCD_DrawText_ext(x1 + 10, y1 + 2, gr2_get_text_color(&sda_sys_con), (uint8_t *)"S!");
    init = 1;
  }

  if (tray_clicked(x1, y1, x2, y2)) {
    if (clickOld == 0) {
      LCD_FillRect(x1, y1, x2, y2, gr2_get_active_color(&sda_sys_con));
      LCD_DrawRectangle(x1, y1, x2, y2, gr2_get_border_color(&sda_sys_con));
      LCD_DrawText_ext(x1 + 10, y1 + 2, gr2_get_text_color(&sda_sys_con), (uint8_t *)"S!");
    }

    if (holdCounter > OPT_HOLD_CNT_BEGIN && holdCounter <= OPT_HOLD_CNT_MAX) {
      LCD_FillRect(
        x1 + 1,
        y1 + 1,
        x1 - 1 + ((x2 - x1) / (OPT_HOLD_CNT_MAX - OPT_HOLD_CNT_BEGIN)) * (holdCounter - OPT_HOLD_CNT_BEGIN),
        y2 - 1,
        gr2_get_active_color(&sda_sys_con)
      );
      LCD_FillRect(
        x1 + 1 + ((x2 - x1) / (OPT_HOLD_CNT_MAX - OPT_HOLD_CNT_BEGIN)) * (holdCounter - OPT_HOLD_CNT_BEGIN),
        y1 + 1,
        x2 - 1,
        y2 - 1,
        gr2_get_fill_color(&sda_sys_con)
      );

      if (holdCounter == OPT_HOLD_CNT_MAX) {
        LCD_FillRect(x1 + 1, y1 + 1, x2 - 1, y2 - 1, gr2_get_active_color(&sda_sys_con));
      }

      LCD_DrawText_ext(x1 + 10, y1 + 2, gr2_get_text_color(&sda_sys_con), (uint8_t *)"S!");
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


uint8_t svp_tray() {
  static uint8_t init;
  static uint8_t XbtnVisibleOld;
  LCD_drawArea area;

  // Unresponsive app killer (works only on real hw)
  if ((svpSGlobal.systemXBtnTime != 0) && ((svpSGlobal.systemXBtnTime + 5) < svpSGlobal.timestamp)) {
    svpSGlobal.systemXBtnTime = 0;
    svmSetError((uint8_t *) "App not responding.");
    svpSGlobal.breakP16Draw = 1;
    svmKillRunning();
    irq_redraw_block_disable(); 
  }

  if (irq_redraw_block == 1) {
    tray_close(PM_TOPBAR_X2 - 32, 0, PM_TOPBAR_X2, 31, 2);
    return 0;
  }

  LCD_getDrawArea(&area);
  LCD_setDrawArea(PM_TOPBAR_X1, PM_TOPBAR_Y1, PM_TOPBAR_X2, PM_TOPBAR_Y2);

  if ((init == 0) || (irq_redraw)) {
    trayBackgroundColor = LCD_color_darken(gr2_get_background_color(&sda_sys_con), 5);
    if (trayBackgroundColor == 0){
      trayBackgroundColor = LCD_color_lighten(gr2_get_background_color(&sda_sys_con), 5);
    }
    LCD_FillRect(PM_TOPBAR_X1, PM_TOPBAR_Y1, PM_TOPBAR_X2, PM_TOPBAR_Y2, trayBackgroundColor);
    init = 1;
  }

  tray_alarm(PM_TOPBAR_X2 - 175, 0, 32);

  tray_time(PM_TOPBAR_X2 - 140, 0, 46);

  tray_battery(PM_TOPBAR_X2 - 85, 0, 49);

  tray_home(PM_TOPBAR_X1, PM_TOPBAR_Y1, 48, 31);

  if(svpSGlobal.systemXBtnVisible == 1) {
    if((XbtnVisibleOld == 0) && (svpSGlobal.systemXBtnVisible == 1)) {
      tray_close(PM_TOPBAR_X2 - 32, 0, PM_TOPBAR_X2, 31, 1);
    }else{
      tray_close(PM_TOPBAR_X2 - 32, 0, PM_TOPBAR_X2, 31, 0);
    }
  }

  if((XbtnVisibleOld == 1) && (svpSGlobal.systemXBtnVisible == 0)) {
    LCD_FillRect(PM_TOPBAR_X2 - 32, 0, PM_TOPBAR_X2, 31, trayBackgroundColor);
  }
  XbtnVisibleOld = svpSGlobal.systemXBtnVisible;

  LCD_setDrawAreaS(&area);
  return 0;
}


uint8_t svp_irq() {
  svp_tray();
  if (irq_redraw) {
    irq_redraw = 0;
  }
  return 0;
}
