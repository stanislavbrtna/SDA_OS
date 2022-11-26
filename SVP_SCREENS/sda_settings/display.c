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

#include "settings.h"

static uint8_t graphic_is_stored;
static uint16_t optLcdScr;

uint16_t sda_settings_display_screen(uint8_t init) {
  static uint16_t optLcdBack;
  static uint16_t optLcdCol;
  static uint16_t optLcdNum;
  static uint16_t optLcdIncr;
  static uint16_t optLcdDecr;
  static uint8_t optLcdMins[2];
  static uint16_t optLcdStore;
  static uint16_t optLcdCalib;
  static uint16_t optLcdBacklight;
  static uint16_t optColScr;

  if (init == 1) {

    optColScr = sda_settings_color_screen(1);
    optLcdScr = gr2_add_screen(&sda_sys_con);

    optLcdMins[0] = svpSGlobal.lcdShutdownTime + 48;
    gr2_add_text(1, 1, 10, 2, SCR_DISPLAY_SETTINGS_SCREEN, optLcdScr, &sda_sys_con);

    optLcdCol = gr2_add_button(1, 2, 9, 3, SCR_COLOR_SETTINGS, optLcdScr, &sda_sys_con);
    optLcdCalib = gr2_add_button(1, 3, 9, 4, SCR_LCD_CALIB, optLcdScr, &sda_sys_con);
    svp_settings_set_spacing(optLcdCol);
    svp_settings_set_spacing(optLcdCalib);

    gr2_add_text(1, 4, 8, 5, SCR_DISPLAY_SHDN_TIME, optLcdScr, &sda_sys_con);
    optLcdNum = gr2_add_text(4, 5, 5, 6, optLcdMins, optLcdScr, &sda_sys_con);
    gr2_add_text(5, 5, 7, 6, (uint8_t *)"min", optLcdScr, &sda_sys_con);

    optLcdDecr = gr2_add_button(1, 5, 3, 6, (uint8_t *)"-", optLcdScr, &sda_sys_con);

    optLcdIncr = gr2_add_button(7, 5, 9, 6, (uint8_t *)"+", optLcdScr, &sda_sys_con);
    gr2_text_set_align(optLcdDecr, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_text_set_align(optLcdIncr, GR2_ALIGN_CENTER, &sda_sys_con);

    gr2_add_text(1, 6, 10, 7, SCR_BACKLIGHT, optLcdScr, &sda_sys_con);

    optLcdBacklight = gr2_add_slider_h(1, 7, 9, 8, 255 - MIN_BACKLIGHT_VALUE, svpSGlobal.lcdBacklight, optLcdScr, &sda_sys_con);

    optLcdBack = gr2_add_button(1, 10, 4, 11, SCR_BACK, optLcdScr, &sda_sys_con);
    optLcdStore = gr2_add_button(6, 10, 9, 11, SCR_STORE, optLcdScr, &sda_sys_con);

    gr2_text_set_align(optLcdBack, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_text_set_align(optLcdStore, GR2_ALIGN_CENTER, &sda_sys_con);

    return optLcdScr;
  }

  if (init == 2) {
    gr2_set_value(optLcdBacklight, svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE, &sda_sys_con);
    return optLcdBack;
  }

  if (gr2_clicked(optLcdBack, &sda_sys_con)) {
    mainScr = slotScreen[2];
    setRedrawFlag();
  }

  if (gr2_clicked(optLcdCol, &sda_sys_con)) {
    mainScr = optColScr;
    graphic_is_stored = 0;
    sda_settings_color_screen(2);
    setRedrawFlag();
  }

  if (gr2_clicked(optLcdCalib, &sda_sys_con)) {
    sdaLockState tickLockOld;
    tickLockOld = tick_lock;
    tick_lock = SDA_LOCK_LOCKED;

    sda_calibrate();

    tick_lock = tickLockOld;

    sda_store_calibration();
    setRedrawFlag();
  }

  if (gr2_clicked(optLcdDecr, &sda_sys_con)) {
    if (svpSGlobal.lcdShutdownTime >= 2) {
      svpSGlobal.lcdShutdownTime--;
      optLcdMins[0] = svpSGlobal.lcdShutdownTime + 48;
      gr2_set_modified(optLcdNum, &sda_sys_con);
    }
  }

  if (gr2_clicked(optLcdIncr, &sda_sys_con)) {
    if (svpSGlobal.lcdShutdownTime < 9) {
      svpSGlobal.lcdShutdownTime++;
      optLcdMins[0] = svpSGlobal.lcdShutdownTime + 48;
      gr2_set_modified(optLcdNum, &sda_sys_con);
    }
  }

  if (gr2_get_event(optLcdBacklight, &sda_sys_con)) {
    if ((gr2_get_value(optLcdBacklight, &sda_sys_con) + MIN_BACKLIGHT_VALUE) > 255) {
      svpSGlobal.lcdBacklight = 255;
    } else {
      svpSGlobal.lcdBacklight = (uint8_t) (gr2_get_value(optLcdBacklight, &sda_sys_con) + MIN_BACKLIGHT_VALUE);
    }

    svp_set_backlight(svpSGlobal.lcdBacklight);
  }
  gr2_set_event(optLcdBacklight, EV_NONE, &sda_sys_con);

  if (gr2_get_value(optLcdBacklight, &sda_sys_con) != svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE) {
    gr2_set_value(optLcdBacklight, svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE, &sda_sys_con);
  }

  if (gr2_clicked(optLcdStore, &sda_sys_con)) {
    sda_store_config();
  }
}

uint16_t sda_settings_color_screen(uint8_t init) {
  // tlačítka volby barev
  static uint16_t b_border;
  static uint16_t b_text;
  static uint16_t b_back;
  static uint16_t b_fill;
  static uint16_t b_active;
  static uint16_t b_store;
  static uint16_t b_def;
  static uint16_t bcolOvrId;
  static uint16_t txcolOvrId;

  static uint16_t bgcolOvrId;
  static uint16_t ficolOvrId;
  static uint16_t actcolOvrId;

  static uint16_t optColBack;

  static uint16_t border_color_bkp;
  static uint16_t text_color_bkp;
  static uint16_t background_color_bkp;
  static uint16_t fill_color_bkp;
  static uint16_t active_color_bkp;

  uint16_t optColScr;

  if (init == 1) {
    //color screen
    optColScr = gr2_add_screen(&sda_sys_con);

    gr2_add_text(1, 1, 10, 2, SCR_COLOR_SETTINGS_SCREEN, optColScr, &sda_sys_con);
    b_border = gr2_add_cbutton(2, 2, 8, 3, SCR_BORDER_COLOR, optColScr, &sda_sys_con);
    gr2_text_set_align(b_border, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_set_value(b_border,gr2_get_border_color(&sda_sys_con), &sda_sys_con);

    b_text = gr2_add_cbutton(2, 3, 8, 4, SCR_TEXT_COLOR, optColScr, &sda_sys_con);
    gr2_set_value(b_text,gr2_get_text_color(&sda_sys_con), &sda_sys_con);
    gr2_text_set_align(b_text, GR2_ALIGN_CENTER, &sda_sys_con);

    b_back = gr2_add_cbutton(2, 4, 8, 5, SCR_BG_COLOR, optColScr, &sda_sys_con);
    gr2_set_value(b_back,gr2_get_background_color(&sda_sys_con), &sda_sys_con);
    gr2_text_set_align(b_back, GR2_ALIGN_CENTER, &sda_sys_con);

    b_fill = gr2_add_cbutton(2, 5, 8, 6, SCR_FILL_COLOR, optColScr, &sda_sys_con);
    gr2_set_value(b_fill, gr2_get_fill_color(&sda_sys_con), &sda_sys_con);
    gr2_text_set_align(b_fill, GR2_ALIGN_CENTER, &sda_sys_con);

    b_active = gr2_add_cbutton(2, 6, 8, 7, SCR_ACTIVE_COLOR, optColScr, &sda_sys_con);
    gr2_set_value(b_active,gr2_get_active_color(&sda_sys_con), &sda_sys_con);
    gr2_text_set_align(b_active, GR2_ALIGN_CENTER, &sda_sys_con);

    b_def = gr2_add_button(2, 8, 8, 9, SCR_SET_DEFAULTS, optColScr, &sda_sys_con);
    gr2_text_set_align(b_def, GR2_ALIGN_CENTER, &sda_sys_con);

    b_store = gr2_add_button(6, 10, 9, 11, SCR_STORE, optColScr, &sda_sys_con);
    optColBack = gr2_add_button(1, 10, 4, 11, SCR_BACK, optColScr, &sda_sys_con);

    gr2_text_set_align(b_store, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_text_set_align(optColBack, GR2_ALIGN_CENTER, &sda_sys_con);

    return optColScr;
  }

  if (init == 2) {
    // store for possible back out of settings
    border_color_bkp = gr2_get_border_color(&sda_sys_con);
    text_color_bkp = gr2_get_text_color(&sda_sys_con);
    background_color_bkp = gr2_get_background_color(&sda_sys_con);
    fill_color_bkp = gr2_get_fill_color(&sda_sys_con);
    active_color_bkp = gr2_get_active_color(&sda_sys_con);
    // reload the color input values
    gr2_set_value(b_border, gr2_get_border_color(&sda_sys_con), &sda_sys_con);
    gr2_set_value(b_text, gr2_get_text_color(&sda_sys_con), &sda_sys_con);
    gr2_set_value(b_back, gr2_get_background_color(&sda_sys_con), &sda_sys_con);
    gr2_set_value(b_fill, gr2_get_fill_color(&sda_sys_con), &sda_sys_con);
    gr2_set_value(b_active, gr2_get_active_color(&sda_sys_con), &sda_sys_con);
  }

  // border color
  if (gr2_clicked(b_border, &sda_sys_con)) {
    bcolOvrId = color_overlay_init();
    color_overlay_set_color(bcolOvrId, gr2_get_border_color(&sda_sys_con));
  }

  color_overlay_update(bcolOvrId);

  if (color_overlay_get_ok(bcolOvrId)){
      gr2_set_border_color(color_overlay_get_color(bcolOvrId), &sda_sys_con);
      color_overlay_clear_ok(bcolOvrId);
      gr2_set_value(b_border, gr2_get_border_color(&sda_sys_con), &sda_sys_con);
  }

  //text color
  if (gr2_clicked(b_text, &sda_sys_con)) {
    txcolOvrId = color_overlay_init();
    color_overlay_set_color(txcolOvrId, gr2_get_text_color(&sda_sys_con));
  }

  color_overlay_update(txcolOvrId);

  if (color_overlay_get_ok(txcolOvrId)) {
      gr2_set_text_color(color_overlay_get_color(txcolOvrId), &sda_sys_con);
      color_overlay_clear_ok(txcolOvrId);
      gr2_set_value(b_text, gr2_get_text_color(&sda_sys_con), &sda_sys_con);
  }

  //background
  if (gr2_clicked(b_back, &sda_sys_con)) {
    bgcolOvrId = color_overlay_init();
    color_overlay_set_color(bgcolOvrId, gr2_get_background_color(&sda_sys_con));
  }

  color_overlay_update(bgcolOvrId);

  if (color_overlay_get_ok(bgcolOvrId)) {
      gr2_set_background_color(color_overlay_get_color(bgcolOvrId), &sda_sys_con);
      color_overlay_clear_ok(bgcolOvrId);
      gr2_set_value(b_back, gr2_get_background_color(&sda_sys_con), &sda_sys_con);
  }

  //fill
  if (gr2_clicked(b_fill, &sda_sys_con)) {
    ficolOvrId = color_overlay_init();
    color_overlay_set_color(ficolOvrId, gr2_get_fill_color(&sda_sys_con));
  }

  color_overlay_update(ficolOvrId);

  if (color_overlay_get_ok(ficolOvrId)) {
      gr2_set_fill_color(color_overlay_get_color(ficolOvrId), &sda_sys_con);
      color_overlay_clear_ok(ficolOvrId);
      gr2_set_value(b_fill, gr2_get_fill_color(&sda_sys_con), &sda_sys_con);
  }

  //active
  if (gr2_clicked(b_active, &sda_sys_con)) {
    actcolOvrId = color_overlay_init();
    color_overlay_set_color(actcolOvrId, gr2_get_active_color(&sda_sys_con));
  }

  color_overlay_update(actcolOvrId);

  if (color_overlay_get_ok(actcolOvrId)) {
      gr2_set_active_color(color_overlay_get_color(actcolOvrId), &sda_sys_con);
      color_overlay_clear_ok(actcolOvrId);
      gr2_set_value(b_active,gr2_get_active_color(&sda_sys_con), &sda_sys_con);
  }

  //store button
  if (gr2_clicked(b_store, &sda_sys_con)) {
    sda_store_config_gui(0);
    graphic_is_stored = 1;
  }

  //def button
  if (gr2_clicked(b_def, &sda_sys_con)) {
    sda_store_config_gui(1);
    sda_load_config();
    gr2_set_value(b_border,gr2_get_border_color(&sda_sys_con), &sda_sys_con);
    gr2_set_value(b_text,gr2_get_text_color(&sda_sys_con), &sda_sys_con);
    gr2_set_value(b_back,gr2_get_background_color(&sda_sys_con), &sda_sys_con);
    gr2_set_value(b_fill, gr2_get_fill_color(&sda_sys_con), &sda_sys_con);
    gr2_set_value(b_active,gr2_get_active_color(&sda_sys_con), &sda_sys_con);
    setRedrawFlag();
  }

  if (gr2_clicked(optColBack, &sda_sys_con)) {
    if (!graphic_is_stored) {
      gr2_set_border_color(border_color_bkp, &sda_sys_con);
      gr2_set_text_color(text_color_bkp, &sda_sys_con);
      gr2_set_background_color(background_color_bkp, &sda_sys_con);
      gr2_set_fill_color(fill_color_bkp, &sda_sys_con);
      gr2_set_active_color(active_color_bkp, &sda_sys_con);
    }
    mainScr = optLcdScr;
    setRedrawFlag();
  }

}