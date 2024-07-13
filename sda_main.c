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

gr2context * sda_current_con;

uint8_t soft_error_flag; // 1 if error overlay is displayed
uint8_t timeUpdateFlag; // 1 if time is updated

uint8_t mainDir[258]; // name of main directory

// battery overlay handling
volatile uint8_t systemBattClick;

// time overlay handling
volatile uint8_t systemDateClick;

// variables used in main loop
uint8_t oldsec;

void sda_main_init();

// static headers
static void sda_main_handle_soft_buttons();
static void sda_main_run_autoexec();
static void sda_main_check_for_alarm();


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
  sdaSetRedrawDetect(1);
}


uint8_t sda_main_loop() {
  static uint8_t init;

  if (init == 0) {
    init = 1;
    sda_main_init();
    test_code();
  } else if (init == 1) {
    sda_main_run_autoexec();

    // precache apps according to precache.lst
    // this is aftear autoexec, so autoexec can generate
    // precache.lst or something.
    sda_precache();
    mainScr = slotScreen[0];
    sda_slot_on_top(SDA_SLOT_HOMESCREEN);
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
  sda_system_overlays_handler();

  // updating screens
  if (sda_slot_get_valid(SDA_SLOT_HOMESCREEN)) {
    svp_homeScreen(0, sda_if_slot_on_top(SDA_SLOT_HOMESCREEN));
  }

  if (sda_slot_get_valid(SDA_SLOT_APPLIST)) {
    svp_appScreen(0, sda_if_slot_on_top(SDA_SLOT_APPLIST));
  }

  if (sda_slot_get_valid(SDA_SLOT_SETTINGS)) {
    sda_settings_gui(0, sda_if_slot_on_top(SDA_SLOT_SETTINGS));
  }

  if (sda_slot_get_valid(SDA_SLOT_SVM)) {
    svmRun(0, sda_if_slot_on_top(SDA_SLOT_SVM));
  }

  if (sda_slot_get_valid(sda_get_top_slot()) == 0) {
    sda_slot_on_top(SDA_SLOT_HOMESCREEN);
  }

  // handling misc stuff
  sda_main_handle_soft_buttons();
  sda_main_check_for_alarm();
  sdaSvmHandleTimers();
  svmHandleUartCallbacks();
  sda_custom_icon_handle_svm_events();
  sda_power_management_handler();
  sda_handle_battery_status();

  sda_extensions_detect();

  // Misc drivers
  sda_keyboard_driver_update();

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


static void sda_main_handle_soft_buttons() {
  // top bar button handlers
  // handler for that big S! button
  if ((svpSGlobal.systemOptClick == CLICKED_SHORT)) {
    if(sda_get_top_slot() == SDA_SLOT_SVM && svmGetValid()) {
      svmHandleHomeButton();
    }
    
    if(sda_get_top_slot() != SDA_SLOT_HOMESCREEN) {

      svpSGlobal.systemXBtnClick = 0;
      svpSGlobal.systemXBtnVisible = 0;

      // destroy overlay if there is one
      if (getOverlayId() != 0) {
        destroyOverlay();
      }
      sda_set_landscape(0);
      sda_keyboard_hide();
      
      if(sda_get_top_slot() == SDA_SLOT_APPLIST) {
        sda_slot_on_top(SDA_SLOT_HOMESCREEN);
      } else {
        sda_prev_slot_on_top(SDA_SLOT_HOMESCREEN);
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

    if (svpSGlobal.systemPwrLongPress == 0 && sda_batt_overlay_shown()) {
      destroyOverlay();
    } else {
      sda_batt_overlay_init();
    }
    
    svpSGlobal.systemPwrLongPress = 0;
  }

  if (systemDateClick == 1) {
    systemDateClick = 0;

    if(sda_dc_overlay_shown() == 0) {
      sda_dc_overlay_init();
    } else {
      destroyOverlay();
    }
  }
}


static void sda_main_check_for_alarm() {
  uint8_t notifAppName[APP_NAME_LEN];
  int32_t id;
  int32_t param;
  if (sdaGetCurentAlarm(&id, &param, notifAppName, sizeof(notifAppName))) {
    svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
    svmAlarmSetFlag(id, param);
    svmLaunch(notifAppName, 0);
  }
}

