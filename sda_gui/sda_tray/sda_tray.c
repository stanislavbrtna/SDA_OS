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
uint16_t trayBackgroundColor;

static uint8_t kbd_tray_en;
static uint8_t alarm_en;

static void tray_battery(int16_t x1, int16_t y1, int16_t w);
static uint8_t tray_close(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t reset);
static uint8_t tray_home(int16_t x1, int16_t y1, int16_t x2, int16_t y2);


void svp_set_irq_redraw() {
  irq_redraw = 1;
}


void sda_tray_alarm_enable() {
  alarm_en = 1;
  svp_set_irq_redraw();
}


void sda_tray_alarm_disable() {
  alarm_en = 0;
  svp_set_irq_redraw();
}


void irq_redraw_block_enable() {
  irq_redraw_block = 1;
}


void irq_redraw_block_disable() {
  irq_redraw_block = 0;
}


gr2EventType sda_tray_clicked(
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


int16_t tray_alarm(int16_t x2, int16_t y1, int16_t w) {
  static uint8_t redraw;
  uint8_t curr_font;

  int16_t x1 = x2 - w;

  if((redraw == 0) || (irq_redraw)) {
    LCD_FillRect(x1 - 1, y1, x1 + w, 31, trayBackgroundColor);
    if (alarm_en) {
      curr_font = LCD_Get_Font_Size();
      LCD_Set_Sys_Font(18);
      LCD_DrawText_ext(x1 + 3, y1 + 8, gr2_get_text_color(&sda_sys_con), (uint8_t *)"A");
      LCD_Set_Sys_Font(curr_font);
    } else {
      w = 0;
    }
    redraw = 1;
  }

  return w;
}


static uint8_t tray_close(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t reset) {
  static uint8_t init;
  static uint8_t selected;
  uint8_t touch_event;

  if (reset == 2) {
    if (sda_tray_clicked(x1, y1, x2, y2) == EV_PRESSED) {
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

  touch_event = sda_tray_clicked(x1, y1, x2, y2);

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

  // store previous draw area
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

  int16_t icons_left_x = 33;

  icons_left_x += sda_tray_battery(PM_TOPBAR_X2 - icons_left_x, 0, 49);

  if(sda_get_top_slot() != SDA_SLOT_HOMESCREEN) {
    icons_left_x += sda_tray_time(PM_TOPBAR_X2 - icons_left_x, 0, 50);
  }

  icons_left_x += tray_alarm(PM_TOPBAR_X2 - icons_left_x, 0, 20);

  icons_left_x += sda_custom_icon_handle(PM_TOPBAR_X2 - icons_left_x, 0, 128);

  // redraw spills with home&close
  sda_tray_home(PM_TOPBAR_X1, PM_TOPBAR_Y1, 48, 31);


  // From left: Close button
  if(svpSGlobal.systemXBtnVisible == 1) {
    tray_close(PM_TOPBAR_X2 - 32, 0, PM_TOPBAR_X2, 31, 1 - XbtnVisibleOld);
  }

  if((XbtnVisibleOld == 1) && (svpSGlobal.systemXBtnVisible == 0)) {
    LCD_FillRect(PM_TOPBAR_X2 - 32, 0, PM_TOPBAR_X2, 31, trayBackgroundColor);
  }
  XbtnVisibleOld = svpSGlobal.systemXBtnVisible;

  // restore draw area
  LCD_setDrawAreaS(&area);
  return 0;
}


uint8_t svp_irq() {
  svp_tray();
  // clear redraw
  irq_redraw = 0;
  return 0;
}
