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

#include "sda_system_overlays.h"

uint16_t dc_overlay;
uint8_t  dc_overlay_flag;

void sda_dc_overlay_destructor();


void sda_dc_overlay_init() {
  if (dc_overlay_flag == 0) {
    destroyOverlay();
    sda_dc_overlay_handle(1);
    setOverlayScreen(dc_overlay, &sda_sys_con);
    setOverlayDestructor(sda_dc_overlay_destructor);
    dc_overlay_flag = 1;
    setOverlayY2(172);
  }
}


void sda_dc_overlay_destructor() {
  gr2_destroy(dc_overlay, &sda_sys_con);
  setRedrawFlag();
  dc_overlay_flag = 0;
  dc_overlay = 0xFFFF;
}


uint8_t sda_dc_overlay_shown() {
  return dc_overlay_flag;
}


// 0: 28. 1. 2024
// 1: 28. ledna 2024
// 2: 28-01-2024
// 3: 2024-01-28

void fmt_date(uint8_t * buff, uint8_t format) {
  if(format == 0) {
    sda_date_fmt(buff, 1, ".", " ", 0, 1);
  }
  if(format == 1) {
    sda_date_fmt(buff, 0, ".", " ", 0, 1);
  }
  if(format == 2) {
    sda_date_fmt(buff, 1, "", "-", 0, 1);
  }
  if(format == 3) {
    sda_date_fmt(buff, 1, "", "-", 1, 1);
  }
}


void sda_dc_overlay_handle(uint8_t init) {
  static uint16_t dateTxt;
  static uint16_t copyBtn;
  static uint16_t fmtBtn;
  static uint8_t  dateBuff[128];

  static uint8_t  format;

  if (init == 1) {
    dc_overlay = gr2_add_screen(&sda_sys_con);
    gr2_set_x_cell(dc_overlay, 16, &sda_sys_con);
    gr2_set_y_cell(dc_overlay, 16, &sda_sys_con);
    
    fmt_date(dateBuff, format);

    dateTxt = gr2_add_text(1, 1, 15, 3, dateBuff, dc_overlay, &sda_sys_con);
    gr2_text_set_align(dateTxt, GR2_ALIGN_CENTER, &sda_sys_con);
    
    copyBtn = gr2_add_button(
          6, 4, 15, 6,
          OVRL_COPY_COPY,
          dc_overlay,
          &sda_sys_con
      );
    gr2_text_set_align(copyBtn, GR2_ALIGN_CENTER, &sda_sys_con);
    
    fmtBtn = gr2_add_button(
          1, 4, 5, 6,
          OVRL_DC_FMT,
          dc_overlay,
          &sda_sys_con
      );

    return;
  }

  if (gr2_get_event(copyBtn, &sda_sys_con) == EV_RELEASED) {
    sda_strcp(dateBuff, svpSGlobal.clipboard, 128);
    dc_overlay_flag = 0;
    setRedrawFlag();
    destroyOverlay();
    return;
  }
  gr2_set_event(copyBtn, EV_NONE, &sda_sys_con);


  if (gr2_get_event(fmtBtn, &sda_sys_con) == EV_RELEASED) {
    format++;

    if(format == 4) {
      format = 0;
    }
    
    fmt_date(dateBuff, format);
    gr2_set_modified(dateTxt, &sda_sys_con);
  }
  gr2_set_event(fmtBtn, EV_NONE, &sda_sys_con);

  if(svpSGlobal.lcdState == LCD_OFF) {
    dc_overlay_flag = 0;
    setRedrawFlag();
    destroyOverlay();
    return;
  }

  sda_screen_button_handler(dc_overlay, 0, &sda_sys_con);
}