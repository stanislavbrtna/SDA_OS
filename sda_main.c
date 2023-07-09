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
gr2Element sda_system_gr2_elements[SDA_SYS_ELEM_MAX];
gr2Screen sda_system_gr2_screens[SDA_SYS_SCREEN_MAX];
gr2context sda_sys_con;

// GR2 app data structure
gr2Element sda_app_gr2_elements[SDA_APP_ELEM_MAX];
gr2Screen sda_app_gr2_screens[SDA_APP_SCREEN_MAX];
gr2context sda_app_con;

gr2context * sda_current_con;

uint8_t soft_error_flag; // 1 if error overlay is displayed
uint8_t timeUpdateFlag; // 1 if time is updated

uint8_t mainDir[258]; // name of main directory

// battery overlay handling
volatile uint8_t systemBattClick;

// variables used in main loop
uint8_t oldsec;

extern psvcKbdLayout kbdLayout;

// static headers
static void sda_main_init();
static void sda_main_handle_soft_buttons();
static void sda_main_run_autoexec();
static void sda_main_check_for_alarm();

// SVS wrapper headers
void pcBasicWrapInit();
void svsGr2WrapInit();


void test_code() {
  // Testcode section is used for testing new things, it is called once after init

  svmPrecacheFile("lib/fsl.svs");
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
  sda_svs_wrapper_init();

  gr2_init_context(
      &sda_sys_con,
      sda_system_gr2_elements,
      SDA_SYS_ELEM_MAX - 1,
      sda_system_gr2_screens,
      SDA_SYS_SCREEN_MAX - 1
  );

  gr2_init_context(
      &sda_app_con,
      sda_app_gr2_elements,
      SDA_APP_ELEM_MAX - 1,
      sda_app_gr2_screens,
      SDA_APP_SCREEN_MAX - 1
  );

  // get default
  svp_getcwd(mainDir, 256);

  svp_crypto_init();

  sda_set_init_struct_defaults();

  oldsec = 99;

  init_kblayout_standard(&kbdLayout);

  // loading config from SD
  sda_load_config();

  // initialize screens: home, apps and options
  sda_slot_init(0, svp_homeScreen(1, 0), &sda_sys_con, 1, 1);
  sda_homescreen_configure();

  sda_slot_init(1, svp_appScreen(1, 0), &sda_sys_con, 1, 0);
  sda_slot_init(2, svp_optScreen(1, 0), &sda_sys_con, 1, 0);
  svmRun(1, 0);
  sda_slot_init(4, 0, &sda_app_con, 0, 0);

  mainScr = slotScreen[0];
  sda_current_con = &sda_sys_con;
  led_set_pattern(LED_ON);

  sdaReloadAlarms();
  sdaPrintActiveAlarm();

  sda_crypto_keyfile_boot_check();

  // screen redraw for the first time
  tick_lock = SDA_LOCK_LOCKED;
  LCD_setDrawArea(
    0,
    0,
    SDA_LCD_W + 160 * svpSGlobal.lcdLandscape,
    SDA_LCD_H - 160 * svpSGlobal.lcdLandscape
  );
  gr2_draw_screen(0, 32, 319, 479, mainScr, 1, &sda_sys_con);
  tick_lock = SDA_LOCK_UNLOCKED;
  led_set_pattern(LED_OFF);

  // check for screen lock
  if (rtc_read_locked() == 1) {
    svpSGlobal.sdaDeviceLock = DEVICE_LOCKED;
    sda_slot_on_top(0);
  }
  
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
  sda_auth_overlay_handle();
  sda_clipboard_overlay_update();

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
    svmRun(0, sda_if_slot_on_top(4));
  }

  if (sda_slot_get_valid(sda_get_top_slot()) == 0) {
    sda_slot_on_top(0);
  }

  // handling misc stuff
  sda_main_handle_soft_buttons();
  sda_main_check_for_alarm();
  sdaSvmHandleTimers();
  sda_power_management_handler();
  sda_handle_battery_status();

  // cleaning input flags
  timeUpdateFlag = 0;
  sdaSetRedrawDetect(0);
  svpSGlobal.systemXBtnTime = 0; // foreced app kill flag

  svpSGlobal.touchValid = 0; //if the touch event was not handled, we discard it
  svpSGlobal.btnFlag = 0;

  // wait for input on UMC
  sda_power_wait_for_input();

  return 0;
}


static void sda_main_run_autoexec() {
  // autoexec sits in the APPS directory, it's executed upon boot, if found
  if (svp_fexists((uint8_t *)"autoexec.svs")) {
    if (svmLaunch((uint8_t *)"autoexec.svs", 0) == 1) {
      // if it loaded ok, we run it a few times for it to execute the exit call
      svmRun(0, 1);
      svmRun(0, 1);
      svmRun(0, 1);
      svmRun(0, 1);
    }
  }
  sda_slot_on_top(0);
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
      sda_set_landscape(0);
      sda_keyboard_hide();
      if(sda_get_prev_top_screen_slot() == 1
         || sda_get_prev_top_screen_slot() == 2)
      {
        sda_slot_on_top(0);
      } else {
        sda_slot_on_top(1);
      }
      
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


static void sda_main_check_for_alarm() {
  uint8_t notifAppName[APP_NAME_LEN];
  int32_t id;
  int32_t param;
  if (sdaGetCurentAlarm(&id, &param, notifAppName, sizeof(notifAppName))) {
    svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
    sda_alarm_set_flag(id, param);
    svmLaunch(notifAppName, 0);
  }
}

