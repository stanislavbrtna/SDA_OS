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

#include "SDA_OS.h"

// GR2 system data structure
pscgElement sda_system_gr2_elements[SDA_SYS_ELEM_MAX];
pscgScreen sda_system_gr2_screens[SDA_SYS_SCREEN_MAX];
gr2context sda_sys_con;

// GR2 app data structure
pscgElement sda_app_gr2_elements[SDA_APP_ELEM_MAX];
pscgScreen sda_app_gr2_screens[SDA_APP_SCREEN_MAX];
gr2context sda_app_con;

gr2context * sda_current_con;

// system overlay
uint16_t overlayScr;
gr2context * overlayCont;
uint16_t overlayX1;
uint16_t overlayX2;
uint16_t overlayY1;
uint16_t overlayY2;
uint16_t ov_id; // current overlay id

uint8_t soft_error_flag; // 1 if error overlay is displayed
uint8_t timeUpdateFlag; // 1 if time is updated

uint8_t mainDir[258]; // name of main directory

// battery overlay handling
volatile uint8_t systemBattClick;

// variables used in main loop
uint8_t kbdVisibleOld;
uint8_t oldsec;
uint8_t kbdRedraw;
psvcKbdLayout kbdLayout;

// SVS wrapper headers
void pcBasicWrapInit();
void svsGr2WrapInit();


void test_code() {
  // Testcode section is used for testing new things, it is called once after init
  return;
}


// misc functions
void svp_switch_main_dir() {
  svp_chdir(mainDir);
}


void setRedrawFlag() {
  svp_set_irq_redraw();
  svpSGlobal.systemRedraw = 1;
}


static void set_init_struct_defaults() {
  svpSGlobal.uptime = 0;
  svpSGlobal.lcdOnTime = 0;
  svpSGlobal.dateUpdated = 0;
  svpSGlobal.systemRedraw = 0;
  svpSGlobal.systemXBtnClick = 0;
  svpSGlobal.systemXBtnVisible = 1;
  svpSGlobal.kbdVisible = 0;
  svpSGlobal.mute = 0;

  // 101 is non-valid init value, displays questionmark
  svpSGlobal.battPercentage = 101;
  svpSGlobal.battString[0] = ' ';
  svpSGlobal.battString[1] = ' ';
  svpSGlobal.battString[2] = ' ';
  svpSGlobal.battString[3] = '?';
  svpSGlobal.battString[4] = 'V';
  svpSGlobal.battString[5] = 0;

  svpSGlobal.pwrType = POWER_BATT;
  svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
  svpSGlobal.lcdBacklight = 255;
  svpSGlobal.sdaDeviceLock = DEVICE_UNLOCKED;
}


static void sda_main_init() {
  printf(
      "SDA OS Init: %d:%d:%d %d.%d. %d\n",
      svpSGlobal.hour,
      svpSGlobal.min,
      svpSGlobal.sec,
      svpSGlobal.day,
      svpSGlobal.month,
      svpSGlobal.year
    );
  led_set_pattern(LED_ALARM);
  LCD_Set_Sys_Font(18);
  svp_setMounted(1); // because SD or FS is already mounted

  sda_check_fs(); // but better to be sure

  // init wrappers
  svsDirectSWrapInit();
  pcBasicWrapInit();
  svsGr2WrapInit();
  sda_files_wrapper_init();
  svsSVPWrapInit();

  gr2_InitContext(
      &sda_sys_con,
      sda_system_gr2_elements,
      SDA_SYS_ELEM_MAX - 1,
      sda_system_gr2_screens,
      SDA_SYS_SCREEN_MAX - 1
  );

  gr2_InitContext(
      &sda_app_con,
      sda_app_gr2_elements,
      SDA_APP_ELEM_MAX - 1,
      sda_app_gr2_screens,
      SDA_APP_SCREEN_MAX - 1
  );

  // get default
  svp_getcwd(mainDir, 256);

  svp_crypto_init();

  set_init_struct_defaults();

  oldsec = 99;
  kbdVisibleOld = 0;
  kbdRedraw = 0;

  init_kblayout_standard(&kbdLayout);

  // loading config from SD
  sda_load_config();

  // initialize screens: home, apps and options
  sda_slot_init(0, svp_homeScreen(1, 0), &sda_sys_con, 1, 1);
  sda_slot_init(1, svp_appScreen(1, 0), &sda_sys_con, 1, 0);
  sda_slot_init(2, svp_optScreen(1, 0), &sda_sys_con, 1, 0);
  sdaSvmRun(1, 0);
  sda_slot_init(4, 0, &sda_app_con, 0, 0);

  mainScr = slotScreen[0];
  sda_current_con = &sda_sys_con;
  led_set_pattern(LED_ON);

  sdaReloadAlarms();
  sdaPrintActiveAlarm();

  sda_crypto_keyfile_boot_check();

  // screen redraw for the first time
  tick_lock = SDA_LOCK_LOCKED;
  LCD_setDrawArea(0, 0, SDA_LCD_W, SDA_LCD_H);
  pscg_draw_screen(0, 32, 319, 479, mainScr, 1, &sda_sys_con);
  tick_lock = SDA_LOCK_UNLOCKED;
  led_set_pattern(LED_OFF);

  // check for screen lock
  if (rtc_read_locked() == 1) {
    svpSGlobal.sdaDeviceLock = DEVICE_LOCKED;
    sda_slot_on_top(0);
  }
  
}

static void sda_main_process_touch() {
  uint8_t scr_touch_retval = 0;

#ifdef TOUCHTEST
  if (svpSGlobal.touchType == EV_PRESSED){
    printf("touchValid! PRESSED x: %d y:%d\n", svpSGlobal.touchX,svpSGlobal.touchY );
  }else if(svpSGlobal.touchType == EV_RELEASED){
    printf("touchValid! RELEASED x: %d y:%d\n", svpSGlobal.touchX,svpSGlobal.touchY );
  }else if(svpSGlobal.touchType == EV_HOLD){
    printf("touchValid! HOLD x: %d y:%d\n", svpSGlobal.touchX,svpSGlobal.touchY );
  }
#endif

  if (svpSGlobal.kbdVisible == 1) { // if there is a keyboard
    uint8_t retVal = 0;
    touch_lock = SDA_LOCK_LOCKED;

    retVal
      = svp_touch_keyboard(
            0,
            319,
            &kbdLayout,
            svpSGlobal.touchX,
            svpSGlobal.touchY,
            svpSGlobal.kbdKeyStr,
            svpSGlobal.touchType
          );
    touch_lock = SDA_LOCK_UNLOCKED;

    if (retVal != 0) {
      if (retVal == 2) { // esc
        pscg_text_deactivate(&sda_sys_con);
        pscg_text_deactivate(&sda_app_con);
        svpSGlobal.kbdKeyStr[0] = 0;
        svpSGlobal.kbdVisible = 0;
      } else {
        if (svpSGlobal.kbdKeyStr[0] == 1) { // special button command
          sda_keyboard_set_layout(svpSGlobal.kbdKeyStr[1], &kbdLayout);
          svpSGlobal.kbdKeyStr[0] = 0;
          kbdRedraw = 1;
        } else {
          svpSGlobal.kbdFlag = 1;
        }
      }
    }
  }

  if (overlayScr == 0) { // if there is no overlay
    // touch is in main screen
    if ((svpSGlobal.touchType != EV_NONE)) {
      scr_touch_retval = pscg_touch_input(
        0,
        32,
        319,
        479 - 160 * svpSGlobal.kbdVisible,
        svpSGlobal.touchX,
        svpSGlobal.touchY,
        svpSGlobal.touchType,
        mainScr,
        sda_current_con
      );

      if (scr_touch_retval == 2) { // retval 2 means open the keyboard
        showKeyboard();
      }
    }
  } else {
    // touch in overlay
    if ((svpSGlobal.touchType != EV_NONE)) {
      scr_touch_retval = pscg_touch_input(
          overlayX1,
          overlayY1,
          overlayX2,
          overlayY2,
          svpSGlobal.touchX,
          svpSGlobal.touchY,
          svpSGlobal.touchType,
          overlayScr,
          overlayCont
        );

      if (scr_touch_retval == 2) { // retval 2 means open the keyboard
        showKeyboard();
      }
    }

    if (((svpSGlobal.touchX < overlayX1 - 10)
        || (svpSGlobal.touchX > overlayX2 + 10)
        || (svpSGlobal.touchY < overlayY1 - 10 && svpSGlobal.touchY > 32)
        || (svpSGlobal.touchY > overlayY2 + 10))
        && svpSGlobal.kbdVisible == 0 && kbdVisibleOld == 0
        && svpSGlobal.touchType == EV_RELEASED) {
      destroyOverlay();
      setRedrawFlag();
    }
  }
  sx_set_touch_ev(svpSGlobal.touchType, svpSGlobal.touchX, svpSGlobal.touchY);

}

static void sda_main_redraw() {
  // lock tick for redraw
  tick_lock = SDA_LOCK_LOCKED;
  LCD_setDrawArea(0, 0, SDA_LCD_W, SDA_LCD_H);
  if (svpSGlobal.systemRedraw == 1 || kbdRedraw) {
    sdaSetRedrawDetect(1);
  }

  if ((svpSGlobal.kbdVisible == 1 && kbdVisibleOld == 0)
      || (svpSGlobal.systemRedraw && svpSGlobal.kbdVisible == 1)
      || kbdRedraw
    ) {
    svp_draw_keyboard(0, 319, &kbdLayout);
    kbdRedraw = 0;
    if (kbdRedraw == 0) {
      svpSGlobal.systemRedraw = 1;
    }
  }

  if ((svpSGlobal.kbdVisible == 1) && (kbdVisibleOld == 0)) {
    // switch to standard layout when keyboard is openned
    init_kblayout_standard(&kbdLayout);
    kbdRedraw = 1;
  }

  if ((svpSGlobal.kbdVisible == 0) && (kbdVisibleOld == 1)) {
    svpSGlobal.systemRedraw = 1;
  }

  kbdVisibleOld = svpSGlobal.kbdVisible;

  if (overlayScr == 0) {
    LCD_setDrawArea(0, 0, SDA_LCD_W - 1, SDA_LCD_H - 160 * svpSGlobal.kbdVisible);
    if (svpSGlobal.systemRedraw == 1) {
      pscg_draw_screen(0, 32, 319, 479 - 160 * svpSGlobal.kbdVisible, mainScr, 1, sda_current_con);
      svpSGlobal.systemRedraw = 0;
    } else {
      pscg_draw_screen(0, 32, 319, 479 - 160 * svpSGlobal.kbdVisible, mainScr, 0, sda_current_con);
    }
  } else {
    if (svpSGlobal.systemRedraw == 1) {
      pscg_draw_screen(0, 32, 319, 479 - 160 * svpSGlobal.kbdVisible, mainScr, 1, sda_current_con);
      sda_draw_overlay_shadow(
            overlayX1,
            overlayY1,
            overlayX2,
            overlayY2,
            overlayCont
      );
      pscg_draw_screen(
            overlayX1,
            overlayY1,
            overlayX2,
            overlayY2,
            overlayScr,
            1,
            overlayCont
      );
      svpSGlobal.systemRedraw = 0;
    }
    sda_draw_overlay_shadow(
          overlayX1,
          overlayY1,
          overlayX2,
          overlayY2,
          overlayCont
    );
    pscg_draw_screen(overlayX1, overlayY1, overlayX2, overlayY2, overlayScr, 0, overlayCont);
  }
  pscg_draw_end(sda_current_con);
  if (overlayScr != 0) {
    pscg_draw_end(overlayCont);
  }

  tick_lock = SDA_LOCK_UNLOCKED;
}


static void sda_main_check_for_notifications() {
  uint8_t notifAppName[APP_NAME_LEN];
  int32_t id;
  int32_t param;
  if (sdaGetCurentAlarm(&id, &param, notifAppName, sizeof(notifAppName))) {
    svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
    setNotificationFlag(id, param);
    sdaSvmLaunch(notifAppName, 0);
  }
}


static void sda_main_handle_soft_buttons() {
  // top bar button handlers
  // handler for that big S! button
  if ((svpSGlobal.systemOptClick == CLICKED_SHORT)) {
    if(sda_get_prev_top_screen_slot() != 0) {

      svpSGlobal.systemXBtnClick = 0;
      svpSGlobal.systemXBtnVisible = 0;

      // destroy overlay if there is one
      if (getOverlayId() != 0) {
        destroyOverlay();
      }

      hideKeyboard();
      sda_slot_on_top(0);
      svp_chdir(mainDir);
      svp_chdir((uint8_t *)"APPS");
      sda_set_sleep_lock(0);
    }
    svpSGlobal.systemOptClick = CLICKED_NONE;
  }

  // long press of the S! button
  if ((svpSGlobal.systemOptClick == CLICKED_LONG)) {
    taskSwitcherOpen();
    svpSGlobal.systemOptClick = CLICKED_NONE;
  }

  // batt button handler
  if ((systemBattClick == 1 || svpSGlobal.systemPwrLongPress == 1)) {
    systemBattClick = 0;

    if (svpSGlobal.systemPwrLongPress == 1) {
      svpSGlobal.lcdBacklight = 255;
      svp_set_backlight(svpSGlobal.lcdBacklight);
    }

    svpSGlobal.systemPwrLongPress = 0;
    sda_batt_overlay_init();
  }
}

static void sda_main_run_autoexec() {
  // autoexec sits in the APPS directory, it's executed upon boot, if found
  if (svp_fexists((uint8_t *)"autoexec.svs")) {
    if (sdaSvmLaunch((uint8_t *)"autoexec.svs", 0) == 1) {
      // if it loaded ok, we run it a few times for it to execute the exit call
      sdaSvmRun(0, 1);
      sdaSvmRun(0, 1);
      sdaSvmRun(0, 1);
      sdaSvmRun(0, 1);
    }
  }
  sda_slot_on_top(0);
}

uint8_t sda_main_loop() {
  static uint8_t init;

  if (init == 0) {
    init = 1;
    sda_main_init();
    test_code();
  } else if (init == 1) {
    sda_main_run_autoexec();
    init = 2;
  }

  if (svpSGlobal.sec != oldsec) {
    timeUpdateFlag = 1;
    oldsec = svpSGlobal.sec;
  }

  if (svpSGlobal.touchType != EV_NONE) {
    sda_main_process_touch();
  }
  svpSGlobal.touchType = EV_NONE;

  sda_store_buttons();
  sda_clr_button_ev();

  sda_main_redraw();

  // System overlays are updated before screens
  if (sda_batt_overlay_shown()) {
    batt_overlay_handle(0);
  }

  if (soft_error_flag == 1) {
    sda_error_overlay_handle();
  }

  taskSwitcherUpdate();

  // updating screens
  if (sda_slot_get_valid(0)) {
    svp_homeScreen(0, sda_if_slot_on_top(0));
  }

  if (sda_slot_get_valid(1)) {
    svp_appScreen(0, sda_if_slot_on_top(1));
  }

  if (sda_slot_get_valid(2)) {
    svp_optScreen(0, sda_if_slot_on_top(2));
  }

  if (sda_slot_get_valid(4)) {
    sdaSvmRun(0, sda_if_slot_on_top(4));
  }


  sda_main_handle_soft_buttons();

  sda_main_check_for_notifications();
  sdaSvmHandleTimers();
  sda_power_management_handler();
  sda_handle_battery_status();

  // wait for input on UMC
  sda_power_wait_for_input();

  // cleaning input flags
  timeUpdateFlag = 0;
  sdaSetRedrawDetect(0);
  svpSGlobal.systemXBtnTime = 0; // foreced app kill flag

  svpSGlobal.touchValid = 0; //if the touch event was not handled, we discard it
  svpSGlobal.btnFlag = 0;

  return 0;
}
