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
  pscg_destroy(batt_overlay, &sda_sys_con);
  setRedrawFlag();
  batt_overlay_flag = 0;
  batt_overlay = 0xFFFF;
}


uint8_t sda_batt_overlay_shown() {
  return batt_overlay_flag;
}


void batt_overlay_handle(uint8_t init) {
  static uint16_t backlightSlider;
  static uint16_t backlightButton;
  static uint16_t soundEnable;
  static uint8_t backlightOld;

  if (init == 1) {
    batt_overlay = pscg_add_screen(&sda_sys_con);
    pscg_set_x_cell(batt_overlay, 16, &sda_sys_con);
    pscg_set_y_cell(batt_overlay, 16, &sda_sys_con);
    backlightSlider
      = pscg_add_slider_h(
          1, 1, 15,  3,
          255 - MIN_BACKLIGHT_VALUE,
          svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
          batt_overlay,
          &sda_sys_con
      );
    backlightButton
      = pscg_add_button(
          11, 4, 15, 6,
          OVRL_OK,
          batt_overlay,
          &sda_sys_con
      );
    soundEnable
      = pscg_add_checkbox(
          1, 4, 10, 6,
          OVRL_SILENT,
          batt_overlay,
          &sda_sys_con
      );

    pscg_set_value(soundEnable, svpSGlobal.mute, &sda_sys_con);
    return;
  }

  if (pscg_get_event(backlightSlider, &sda_sys_con)) {
    if ((pscg_get_value(backlightSlider, &sda_sys_con) + MIN_BACKLIGHT_VALUE) > 255){
      svpSGlobal.lcdBacklight = 255;
    } else {
      svpSGlobal.lcdBacklight
        = (uint8_t) (pscg_get_value(backlightSlider, &sda_sys_con) + MIN_BACKLIGHT_VALUE);
    }
    svp_set_backlight(svpSGlobal.lcdBacklight);
  }
  pscg_set_event(backlightSlider, EV_NONE, &sda_sys_con);

  if (sda_wrap_get_button(BUTTON_RIGHT) || sda_wrap_get_button(BUTTON_UP)) {
    if (svpSGlobal.lcdBacklight < 255) {
      svpSGlobal.lcdBacklight++;
      svp_set_backlight(svpSGlobal.lcdBacklight);
      pscg_set_value(
        backlightSlider,
        svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
        &sda_sys_con
      );
    }
    sda_wrap_clear_button(BUTTON_RIGHT);
    sda_wrap_clear_button(BUTTON_UP);
  }

  if (sda_wrap_get_button(BUTTON_LEFT) || sda_wrap_get_button(BUTTON_DOWN)) {
    if (svpSGlobal.lcdBacklight > MIN_BACKLIGHT_VALUE) {
      svpSGlobal.lcdBacklight--;
      svp_set_backlight(svpSGlobal.lcdBacklight);
      pscg_set_value(
        backlightSlider,
        svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
        &sda_sys_con
      );
    }
    sda_wrap_clear_button(BUTTON_LEFT);
    sda_wrap_clear_button(BUTTON_DOWN);
  }

  if (svpSGlobal.systemPwrLongPress == 1) {
    svpSGlobal.systemPwrLongPress = 0;
    svpSGlobal.lcdBacklight = 255;
    svp_set_backlight(svpSGlobal.lcdBacklight);
    pscg_set_value(
      backlightSlider,
      svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
      &sda_sys_con
    );
  }

  if (pscg_get_event(backlightButton, &sda_sys_con) == EV_RELEASED || svpSGlobal.lcdState == LCD_OFF) {
    batt_overlay_flag = 0;
    setRedrawFlag();
    destroyOverlay();
    return;
  }
  pscg_set_event(backlightButton, EV_NONE, &sda_sys_con);

  if (pscg_get_event(soundEnable, &sda_sys_con) == EV_RELEASED) {
    svpSGlobal.mute = pscg_get_value(soundEnable, &sda_sys_con);
    sda_store_mute_config();
  }
  pscg_set_event(soundEnable, EV_NONE, &sda_sys_con);

  if (svpSGlobal.mute != pscg_get_value(soundEnable, &sda_sys_con))
  pscg_set_value(soundEnable, svpSGlobal.mute, &sda_sys_con);

  if (svpSGlobal.lcdBacklight != backlightOld) {
    pscg_set_value(
        backlightSlider,
        svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
        &sda_sys_con
    );
  }

  backlightOld = svpSGlobal.lcdBacklight;
}