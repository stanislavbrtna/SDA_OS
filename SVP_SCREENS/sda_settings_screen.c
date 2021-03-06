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

#include "svp_screens.h"
#include "sda_settings/settings.h"

uint16_t svp_optScreen(uint8_t init, uint8_t top) {
  static uint16_t optScreen;

  static uint16_t optTimeScr;
  static uint16_t optLcdScr;
  static uint16_t optColScr;
  static uint16_t optInfoScr;
  static uint16_t optNotifyScr;
  static uint16_t optSecuScr;
  static uint16_t optDbgScr;

  static uint16_t optTimSel;
  static uint16_t optLcdSel;
  static uint16_t optDbgSel;
  static uint16_t optInfoSel;

  static uint16_t optSecuSel;

  static uint16_t optMntSel;
  static uint16_t optSound;

  //nastavení data a času
  static uint16_t optTimeBtn;
  static uint16_t optDateBtn;
  static uint16_t optTimBack;

  static uint16_t timeOvrId;
  static uint16_t dateOvrId;

  // lcd nastavení
  static uint16_t optLcdBack;
  static uint16_t optLcdCol;
  static uint16_t optLcdNum;
  static uint16_t optLcdIncr;
  static uint16_t optLcdDecr;
  static uint8_t optLcdMins[2];
  static uint16_t optLcdStore;
  static uint16_t optLcdCalib;
  static uint16_t optLcdBacklight;

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

  static uint8_t graphic_is_stored;
  static uint16_t border_color_bkp = 0;
  static uint16_t text_color_bkp = 0;
  static uint16_t background_color_bkp = 0;
  static uint16_t fill_color_bkp = 0;
  static uint16_t active_color_bkp = 0;

  //security
  static uint16_t optSecuBack;
  static uint16_t optSecuOld;
  static uint16_t optSecuNew;

  static uint16_t optSecuOldBtn;
  static uint16_t optSecuNewBtn;

  static uint8_t optSecuNewStr[33];
  static uint8_t optSecuOldStr[33];
  static uint16_t optSecuOk;
  static uint16_t optSecuMsg;
  static uint16_t optSecuMsg2;

  //mount
  static uint8_t sd_mounted;
  static uint16_t prac_screen;
  //uint16_t x;

  //uint8_t retval;

  if (init == 1) {

    //probably
    sd_mounted = 1;

    optScreen = pscg_add_screen(&sda_sys_con);

    optTimeScr = pscg_add_screen(&sda_sys_con);

    optLcdScr = pscg_add_screen(&sda_sys_con);
    optColScr = pscg_add_screen(&sda_sys_con);
    optInfoScr = svp_opt_info(1);

    optNotifyScr = svp_opt_notifications(1);

    optSecuScr = pscg_add_screen(&sda_sys_con);

    optDbgScr = svp_opt_debug(1);


    // options screen init
    pscg_add_text(1, 1, 10, 2, SCR_SETTINGS, optScreen, &sda_sys_con);

    optTimSel = pscg_add_button(1, 2, 8, 3, SCR_SET_DATE_TIME, optScreen, &sda_sys_con);
    optLcdSel = pscg_add_button(1, 3, 8, 4, SCR_SET_DISPLAY, optScreen, &sda_sys_con);
    optSound = pscg_add_button(1, 4, 8, 5, SCR_SETTINGS_SND, optScreen, &sda_sys_con);
    optSecuSel = pscg_add_button(1, 5, 8, 6, SCR_SET_SECU, optScreen, &sda_sys_con);
    optMntSel = pscg_add_button(1, 8, 8, 9, SD_UMOUNT, optScreen, &sda_sys_con);
    optInfoSel = pscg_add_button(1, 10, 8, 11, SCR_ABOUT_SYSTEM, optScreen, &sda_sys_con);

    optDbgSel = pscg_add_button(1, 6, 8, 7, (uint8_t *)"Debug", optScreen, &sda_sys_con);

    //time screen
    pscg_add_text(1, 1, 10, 2, SCR_SET_TIME_SCR, optTimeScr, &sda_sys_con);
    optTimeBtn = pscg_add_button(1, 2, 6, 3, SCR_SET_TIME, optTimeScr, &sda_sys_con);
    optDateBtn = pscg_add_button(1, 3, 6, 4, SCR_SET_DATE, optTimeScr, &sda_sys_con);

    optTimBack = pscg_add_button(1, 6, 5, 7, SCR_BACK, optTimeScr, &sda_sys_con);

    //lcd screen
    optLcdMins[0] = svpSGlobal.lcdShutdownTime + 48;
    pscg_add_text(1, 1, 10, 2, SCR_DISPLAY_SETTINGS_SCREEN, optLcdScr, &sda_sys_con);

    optLcdCol = pscg_add_button(1, 2, 8, 3, SCR_COLOR_SETTINGS, optLcdScr, &sda_sys_con);
    optLcdCalib = pscg_add_button(1, 3, 8, 4, SCR_LCD_CALIB, optLcdScr, &sda_sys_con);

    pscg_add_text(1, 4, 8, 5, SCR_DISPLAY_SHDN_TIME, optLcdScr, &sda_sys_con);
    optLcdNum = pscg_add_text(3, 5, 4, 6, optLcdMins, optLcdScr, &sda_sys_con);
    pscg_add_text(4, 5, 6, 6, (uint8_t *)"min", optLcdScr, &sda_sys_con);

    optLcdDecr = pscg_add_button(1, 5, 3, 6, (uint8_t *)"-", optLcdScr, &sda_sys_con);

    optLcdIncr = pscg_add_button(6, 5, 8, 6, (uint8_t *)"+", optLcdScr, &sda_sys_con);

    pscg_add_text(1, 6, 10, 7, SCR_BACKLIGHT, optLcdScr, &sda_sys_con);

    optLcdBacklight = pscg_add_slider_h(1, 7, 8, 8, 255 - MIN_BACKLIGHT_VALUE, svpSGlobal.lcdBacklight, optLcdScr, &sda_sys_con);

    optLcdBack = pscg_add_button(1, 10, 4, 11, SCR_BACK, optLcdScr, &sda_sys_con);
    optLcdStore = pscg_add_button(5, 10, 8, 11, SCR_STORE, optLcdScr, &sda_sys_con);

    //color screen
    pscg_add_text(1, 1, 10, 2, SCR_COLOR_SETTINGS_SCREEN, optColScr, &sda_sys_con);
    b_border=pscg_add_cbutton(1, 2, 7, 3, SCR_BORDER_COLOR, optColScr, &sda_sys_con);
    pscg_set_value(b_border,pscg_get_border_color(&sda_sys_con), &sda_sys_con);

    b_text=pscg_add_cbutton(1, 3, 7, 4, SCR_TEXT_COLOR, optColScr, &sda_sys_con);
    pscg_set_value(b_text,pscg_get_text_color(&sda_sys_con), &sda_sys_con);

    b_back=pscg_add_cbutton(1, 4, 7, 5, SCR_BG_COLOR, optColScr, &sda_sys_con);
    pscg_set_value(b_back,pscg_get_background_color(&sda_sys_con), &sda_sys_con);

    b_fill=pscg_add_cbutton(1, 5, 7, 6, SCR_FILL_COLOR, optColScr, &sda_sys_con);
    pscg_set_value(b_fill, pscg_get_fill_color(&sda_sys_con), &sda_sys_con);

    b_active=pscg_add_cbutton(1, 6, 7, 7, SCR_ACTIVE_COLOR, optColScr, &sda_sys_con);
    pscg_set_value(b_active,pscg_get_active_color(&sda_sys_con), &sda_sys_con);

    b_def=pscg_add_button(1, 8, 7, 9, SCR_SET_DEFAULTS, optColScr, &sda_sys_con);

    b_store=pscg_add_button(5, 10, 8, 11, SCR_STORE, optColScr, &sda_sys_con);

    optColBack=pscg_add_button(1, 10, 4, 11, SCR_BACK, optColScr, &sda_sys_con);

    //secu screen
    //
    pscg_add_text(1, 1, 10, 2, SCR_SECURITY_SCREEN, optSecuScr, &sda_sys_con);

    pscg_add_text(1, 3, 10, 4, SCR_OLD_PASSWORD, optSecuScr, &sda_sys_con);
    optSecuOld = pscg_add_text(1, 4, 8, 5, (uint8_t *)"", optSecuScr, &sda_sys_con);
    pscg_add_text(1, 5, 10, 6, SCR_NEW_PASSWORD, optSecuScr, &sda_sys_con);
    optSecuNew = pscg_add_text(1, 6, 8, 7, (uint8_t *)"", optSecuScr, &sda_sys_con);
    optSecuOldBtn = pscg_add_button(8, 4, 9, 5, (uint8_t *)"*", optSecuScr, &sda_sys_con);
    optSecuNewBtn = pscg_add_button(8, 6, 9, 7, (uint8_t *)"*", optSecuScr, &sda_sys_con);
    optSecuOk = pscg_add_button(1, 8, 4, 9, SCR_CHANGE_PASSWORD, optSecuScr, &sda_sys_con);
    optSecuMsg = pscg_add_text(4, 8, 10, 9, SCR_WRONG_PASSWORD, optSecuScr, &sda_sys_con);
    optSecuMsg2 = pscg_add_text(4, 8, 10, 9, SCR_PASSWORD_STORED, optSecuScr, &sda_sys_con);

    pscg_text_set_editable(optSecuNew, 1, &sda_sys_con);
    pscg_text_set_editable(optSecuOld, 1, &sda_sys_con);

    pscg_text_set_pwd(optSecuNew, 1, &sda_sys_con);
    pscg_text_set_pwd(optSecuOld, 1, &sda_sys_con);

    optSecuBack = pscg_add_button(1, 10, 4, 11, SCR_BACK, optSecuScr, &sda_sys_con);

    return optScreen;
  }

  if(init == 2) {
    //de-init

  }

  //loop top
  if (top == 1) {

    if (gr2_clicked(optTimSel, &sda_sys_con)) {
      mainScr = optTimeScr;
      setRedrawFlag();
    }

    if (gr2_clicked(optLcdSel, &sda_sys_con)) {
      mainScr = optLcdScr;
      pscg_set_value(optLcdBacklight, svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE, &sda_sys_con);
      setRedrawFlag();
    }

    if (gr2_clicked(optInfoSel, &sda_sys_con)) {
      mainScr = optInfoScr;
      svp_opt_info(2); // reset position of the internal screen
      setRedrawFlag();
    }

    if (gr2_clicked(optDbgSel, &sda_sys_con)) {
      mainScr = optDbgScr;
      svp_opt_debug(2); // reload config of dbg serial
      setRedrawFlag();
    }
    svp_opt_debug(0);

    if (gr2_clicked(optSecuSel, &sda_sys_con)) {
      mainScr = optSecuScr;
      optSecuNewStr[0] = 0;
      optSecuOldStr[0] = 0;
      pscg_set_visible(optSecuMsg, 0, &sda_sys_con);
      pscg_set_visible(optSecuMsg2, 0, &sda_sys_con);
      setRedrawFlag();
    }

    /*Card unmounting and apps reloader*/
    if (gr2_clicked(optMntSel, &sda_sys_con)) {
      if (sd_mounted == 1) {
        svmCloseAll();
        sdaSlotOnTop(2);
        svp_umount();
        sd_mounted = 0;
        pscg_set_str(optMntSel, SCR_SD_MOUNT, &sda_sys_con);
        prac_screen = slotScreen[1];
        slotScreen[1] = pscg_add_screen(&sda_sys_con);
        pscg_add_text(1, 1, 10, 2, SCR_SD_NOT_PRESENT_WARNING, slotScreen[1], &sda_sys_con);
      } else {
        if(svp_mount()) {
          pscg_set_str(optMntSel, SCR_CARD_ERROR_REPEAT, &sda_sys_con);
        } else {
          pscg_set_str(optMntSel, SD_UMOUNT, &sda_sys_con);
          // little hack for reload of app screen
          pscg_destroy_screen( slotScreen[1], &sda_sys_con);
          pscg_destroy_screen( prac_screen, &sda_sys_con);
          slotScreen[1] = svp_appScreen(1, 0);
          sd_mounted = 1;
          sdaSlotOnTop(2);
        }
      }
    }

//============================ Time setup ======================================
    if (gr2_clicked(optTimeBtn, &sda_sys_con)) {
      timeOvrId = time_overlay_init();
      time_overlay_set_time(timeOvrId, svpSGlobal.hour, svpSGlobal.min);
    }

    if (gr2_clicked(optDateBtn, &sda_sys_con)) {
     dateOvrId = date_overlay_init(svpSGlobal.year, svpSGlobal.month, svpSGlobal.day);
     printf("Old date: y:%u d:%u wkd:%u mon:%u %u:%u \n",
             svpSGlobal.year,
             svpSGlobal.day,
             svpSGlobal.weekday,
             svpSGlobal.month,
             svpSGlobal.hour,
             svpSGlobal.min
           );
    }

    date_overlay_update(dateOvrId);

    time_overlay_update(timeOvrId);

    if(time_overlay_get_ok(timeOvrId)) {

      #ifdef PLATFORM_PC
      printf("setting time: %u:%u (Does not work in simulation)\n", time_overlay_get_hours(timeOvrId), time_overlay_get_minutes(timeOvrId));
      #else
      printf("Old time: y:%u d:%u wkd:%u mon:%u %u:%u \n",svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, svpSGlobal.hour, svpSGlobal.min);
      rtc_set_time(svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, (uint8_t) time_overlay_get_hours(timeOvrId), (uint8_t) time_overlay_get_minutes(timeOvrId), 0);
      printf("New time: y:%u d:%u wkd:%u mon:%u %u:%u \n",svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, svpSGlobal.hour, svpSGlobal.min);
      #endif

      time_overlay_clear_ok(timeOvrId);
      timeOvrId = 0;
    }

    if(date_overlay_get_ok(dateOvrId)) {
#ifndef PC
      rtc_set_time(date_overlay_get_year(dateOvrId), date_overlay_get_day(dateOvrId), svpSGlobal.weekday, date_overlay_get_month(dateOvrId),svpSGlobal.hour, svpSGlobal.min, svpSGlobal.sec);
      printf("New date: y:%u d:%u wkd:%u mon:%u %u:%u \n",svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, svpSGlobal.hour, svpSGlobal.min);
#else
      printf("setting date locally: %u. %u. %u \n", date_overlay_get_day(dateOvrId), date_overlay_get_month(dateOvrId), date_overlay_get_year(dateOvrId));
      svpSGlobal.day=date_overlay_get_day(dateOvrId);
      svpSGlobal.month=date_overlay_get_month(dateOvrId);
      if (date_overlay_get_year(dateOvrId) > 2005) {
        svpSGlobal.year=date_overlay_get_year(dateOvrId);
      }
#endif
      svpSGlobal.dateUpdated = 1;
      date_overlay_clear_ok(dateOvrId);
      dateOvrId = 0;
    }

    if (gr2_clicked(optTimBack, &sda_sys_con)) {
      mainScr = slotScreen[2];
      setRedrawFlag();
    }

//============================ Display setup ===================================

    if (gr2_clicked(optLcdBack, &sda_sys_con)) {
      mainScr = slotScreen[2];
      setRedrawFlag();
    }

    if (gr2_clicked(optLcdCol, &sda_sys_con)) {
      mainScr = optColScr;
      graphic_is_stored = 0;
      // store for possible back out of settings
      border_color_bkp = pscg_get_border_color(&sda_sys_con);
      text_color_bkp = pscg_get_text_color(&sda_sys_con);
      background_color_bkp = pscg_get_background_color(&sda_sys_con);
      fill_color_bkp = pscg_get_fill_color(&sda_sys_con);
      active_color_bkp = pscg_get_active_color(&sda_sys_con);
      // reload the color input values
      pscg_set_value(b_border, pscg_get_border_color(&sda_sys_con), &sda_sys_con);
      pscg_set_value(b_text, pscg_get_text_color(&sda_sys_con), &sda_sys_con);
      pscg_set_value(b_back, pscg_get_background_color(&sda_sys_con), &sda_sys_con);
      pscg_set_value(b_fill, pscg_get_fill_color(&sda_sys_con), &sda_sys_con);
      pscg_set_value(b_active, pscg_get_active_color(&sda_sys_con), &sda_sys_con);
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
        pscg_set_modified(optLcdNum, &sda_sys_con);
      }
    }

    if (gr2_clicked(optLcdIncr, &sda_sys_con)) {
      if (svpSGlobal.lcdShutdownTime < 9) {
        svpSGlobal.lcdShutdownTime++;
        optLcdMins[0] = svpSGlobal.lcdShutdownTime + 48;
        pscg_set_modified(optLcdNum, &sda_sys_con);
      }
    }

    if (pscg_get_event(optLcdBacklight, &sda_sys_con)) {
      if ((pscg_get_value(optLcdBacklight, &sda_sys_con) + MIN_BACKLIGHT_VALUE) > 255) {
        svpSGlobal.lcdBacklight = 255;
      } else {
        svpSGlobal.lcdBacklight = (uint8_t) (pscg_get_value(optLcdBacklight, &sda_sys_con) + MIN_BACKLIGHT_VALUE);
      }

      svp_set_backlight(svpSGlobal.lcdBacklight);
    }
    pscg_set_event(optLcdBacklight, EV_NONE, &sda_sys_con);

    if (pscg_get_value(optLcdBacklight, &sda_sys_con) != svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE) {
      pscg_set_value(optLcdBacklight, svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE, &sda_sys_con);
    }

    if (gr2_clicked(optLcdStore, &sda_sys_con)) {
      sda_store_config();
    }

//============================ Color setup =====================================

    // border color
    if (gr2_clicked(b_border, &sda_sys_con)) {
      bcolOvrId = color_overlay_init();
      color_overlay_set_color(bcolOvrId, pscg_get_border_color(&sda_sys_con));
    }

    color_overlay_update(bcolOvrId);

    if (color_overlay_get_ok(bcolOvrId)){
       pscg_set_border_color(color_overlay_get_color(bcolOvrId), &sda_sys_con);
       color_overlay_clear_ok(bcolOvrId);
       pscg_set_value(b_border, pscg_get_border_color(&sda_sys_con), &sda_sys_con);
    }

    //text color
    if (gr2_clicked(b_text, &sda_sys_con)) {
      txcolOvrId = color_overlay_init();
      color_overlay_set_color(txcolOvrId, pscg_get_text_color(&sda_sys_con));
    }

    color_overlay_update(txcolOvrId);

    if (color_overlay_get_ok(txcolOvrId)) {
       pscg_set_text_color(color_overlay_get_color(txcolOvrId), &sda_sys_con);
       color_overlay_clear_ok(txcolOvrId);
       pscg_set_value(b_text, pscg_get_text_color(&sda_sys_con), &sda_sys_con);
    }

    //background
    if (gr2_clicked(b_back, &sda_sys_con)) {
      bgcolOvrId = color_overlay_init();
      color_overlay_set_color(bgcolOvrId, pscg_get_background_color(&sda_sys_con));
    }

    color_overlay_update(bgcolOvrId);

    if (color_overlay_get_ok(bgcolOvrId)) {
       pscg_set_background_color(color_overlay_get_color(bgcolOvrId), &sda_sys_con);
       color_overlay_clear_ok(bgcolOvrId);
       pscg_set_value(b_back, pscg_get_background_color(&sda_sys_con), &sda_sys_con);
    }

    //fill
    if (gr2_clicked(b_fill, &sda_sys_con)) {
      ficolOvrId=color_overlay_init();
      color_overlay_set_color(ficolOvrId, pscg_get_fill_color(&sda_sys_con));
    }

    color_overlay_update(ficolOvrId);

    if (color_overlay_get_ok(ficolOvrId)) {
       pscg_set_fill_color(color_overlay_get_color(ficolOvrId), &sda_sys_con);
       color_overlay_clear_ok(ficolOvrId);
       pscg_set_value(b_fill, pscg_get_fill_color(&sda_sys_con), &sda_sys_con);
    }

    //active
    if (gr2_clicked(b_active, &sda_sys_con)) {
      actcolOvrId=color_overlay_init();
      color_overlay_set_color(actcolOvrId, pscg_get_active_color(&sda_sys_con));
    }

    color_overlay_update(actcolOvrId);

    if (color_overlay_get_ok(actcolOvrId)) {
       pscg_set_active_color(color_overlay_get_color(actcolOvrId), &sda_sys_con);
       color_overlay_clear_ok(actcolOvrId);
       pscg_set_value(b_active,pscg_get_active_color(&sda_sys_con), &sda_sys_con);
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
      pscg_set_value(b_border,pscg_get_border_color(&sda_sys_con), &sda_sys_con);
      pscg_set_value(b_text,pscg_get_text_color(&sda_sys_con), &sda_sys_con);
      pscg_set_value(b_back,pscg_get_background_color(&sda_sys_con), &sda_sys_con);
      pscg_set_value(b_fill, pscg_get_fill_color(&sda_sys_con), &sda_sys_con);
      pscg_set_value(b_active,pscg_get_active_color(&sda_sys_con), &sda_sys_con);
      setRedrawFlag();
    }

    if (gr2_clicked(optColBack, &sda_sys_con)) {
      if (!graphic_is_stored) {
        pscg_set_border_color(border_color_bkp, &sda_sys_con);
        pscg_set_text_color(text_color_bkp, &sda_sys_con);
        pscg_set_background_color(background_color_bkp, &sda_sys_con);
        pscg_set_fill_color(fill_color_bkp, &sda_sys_con);
        pscg_set_active_color(active_color_bkp, &sda_sys_con);
      }
      mainScr = optLcdScr;
      setRedrawFlag();
    }

//============================ Info ============================================
    svp_opt_info(0);

//============================ Notifications ===================================
    svp_opt_notifications(0);

//============================ Sound options ===================================

    if (gr2_clicked(optSound, &sda_sys_con)) {
      svp_opt_notifications(2); // reload current settings
      mainScr = optNotifyScr; //krapet ugly
      setRedrawFlag();
    }

//============================ Security options ================================

    if (gr2_clicked(optSecuBack, &sda_sys_con)) {
      mainScr = slotScreen[2];
      setRedrawFlag();
    }

    svp_input_handler(optSecuNewStr, 32, optSecuNew);

    svp_input_handler(optSecuOldStr, 32, optSecuOld);

    if (gr2_clicked(optSecuNewBtn, &sda_sys_con)) {
      pscg_text_set_pwd(optSecuNew, 1 - pscg_text_get_pwd(optSecuNew, &sda_sys_con), &sda_sys_con);
    }

    if (gr2_clicked(optSecuOldBtn, &sda_sys_con)) {
      pscg_text_set_pwd(optSecuOld, 1 - pscg_text_get_pwd(optSecuOld, &sda_sys_con), &sda_sys_con);
    }

    if (gr2_clicked(optSecuOk, &sda_sys_con)) {
      uint8_t retval;
      retval = svp_crypto_unlock(optSecuOldStr);
      if (retval != 0) {
        if (retval == 2) {
          pscg_set_visible(optSecuMsg, 1, &sda_sys_con);
          pscg_set_visible(optSecuMsg2, 0, &sda_sys_con);
        } else if(retval == 3) {
          pscg_set_grayout(optSecuOk, 1, &sda_sys_con);
          pscg_set_grayout(optSecuNew, 1, &sda_sys_con);
          pscg_set_grayout(optSecuOld, 1, &sda_sys_con);
        }
      } else {
        pscg_set_visible(optSecuMsg2, 1, &sda_sys_con);
        pscg_set_visible(optSecuMsg, 0, &sda_sys_con);
        svp_crypto_change_key(optSecuNewStr);
        svp_crypto_lock();
      }
      optSecuNewStr[0] = 0;
      optSecuOldStr[0] = 0;

      pscg_set_modified(optSecuNew, &sda_sys_con);
      pscg_set_modified(optSecuOld, &sda_sys_con);
      hideKeyboard();
    }
  }
  return 0;
}

