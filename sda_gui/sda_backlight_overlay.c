/*
Copyright (c) 2021 Stanislav Brtna

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

// power options overlay
uint16_t batt_overlay;
uint8_t batt_overlay_flag;

void sda_batt_overlay_init() {
  if (batt_overlay_flag == 0) {
    destroyOverlay();
    batt_overlay_handle(1);
    setOverlayScreen(batt_overlay, &sda_sys_con);
    setOverlayDestructor(sda_batt_overlay_destructor);
    batt_overlay_flag = 1;
    setOverlayY2(172);
  }
}


void sda_batt_overlay_destructor() {
  gr2_destroy(batt_overlay, &sda_sys_con);
  setRedrawFlag();
  batt_overlay_flag = 0;
  batt_overlay = 0xFFFF;
}


uint8_t sda_batt_overlay_shown() {
  return batt_overlay_flag;
}


void batt_overlay_handle(uint8_t init) {
  static uint16_t backlightSlider;
  static uint16_t backlightOk;
  static uint16_t soundEnable;
  static uint8_t backlightOld;

  if (init == 1) {
    batt_overlay = gr2_add_screen(&sda_sys_con);
    gr2_set_x_cell(batt_overlay, 16, &sda_sys_con);
    gr2_set_y_cell(batt_overlay, 16, &sda_sys_con);
    backlightSlider
      = gr2_add_slider_h(
          1, 1, 15,  3,
          255 - MIN_BACKLIGHT_VALUE,
          svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
          batt_overlay,
          &sda_sys_con
      );
    backlightOk
      = gr2_add_button(
          11, 4, 15, 6,
          OVRL_OK,
          batt_overlay,
          &sda_sys_con
      );
    gr2_text_set_align(backlightOk, GR2_ALIGN_CENTER, &sda_sys_con);
    soundEnable
      = gr2_add_checkbox(
          1, 4, 10, 6,
          OVRL_SILENT,
          batt_overlay,
          &sda_sys_con
      );

    gr2_set_value(soundEnable, svpSGlobal.mute, &sda_sys_con);
    return;
  }

  if (gr2_get_event(backlightSlider, &sda_sys_con)) {
    if ((gr2_get_value(backlightSlider, &sda_sys_con) + MIN_BACKLIGHT_VALUE) > 255){
      svpSGlobal.lcdBacklight = 255;
    } else {
      svpSGlobal.lcdBacklight
        = (uint8_t) (gr2_get_value(backlightSlider, &sda_sys_con) + MIN_BACKLIGHT_VALUE);
    }
    svp_set_backlight(svpSGlobal.lcdBacklight);
  }
  gr2_set_event(backlightSlider, EV_NONE, &sda_sys_con);

  if (svpSGlobal.systemPwrLongPress == 1) {
    svpSGlobal.systemPwrLongPress = 0;
    svpSGlobal.lcdBacklight = 255;
    svp_set_backlight(svpSGlobal.lcdBacklight);
    gr2_set_value(
      backlightSlider,
      svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
      &sda_sys_con
    );
  }

  if (gr2_get_event(backlightOk, &sda_sys_con) == EV_RELEASED || svpSGlobal.lcdState == LCD_OFF) {
    batt_overlay_flag = 0;
    setRedrawFlag();
    destroyOverlay();
    return;
  }
  gr2_set_event(backlightOk, EV_NONE, &sda_sys_con);

  if (gr2_get_event(soundEnable, &sda_sys_con) == EV_RELEASED) {
    svpSGlobal.mute = gr2_get_value(soundEnable, &sda_sys_con);
    sda_store_mute_config();
  }
  gr2_set_event(soundEnable, EV_NONE, &sda_sys_con);

  if (svpSGlobal.mute != gr2_get_value(soundEnable, &sda_sys_con))
  gr2_set_value(soundEnable, svpSGlobal.mute, &sda_sys_con);

  if (svpSGlobal.lcdBacklight != backlightOld) {
    gr2_set_value(
        backlightSlider,
        svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
        &sda_sys_con
    );
  }

  sda_screen_button_handler(batt_overlay, backlightOk, &sda_sys_con);

  backlightOld = svpSGlobal.lcdBacklight;
}