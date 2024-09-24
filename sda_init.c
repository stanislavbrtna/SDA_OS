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

#include "SDA_OS.h"

// GR2 system data structure
gr2Element sda_system_gr2_elements[SDA_SYS_ELEM_MAX];
gr2Screen sda_system_gr2_screens[SDA_SYS_SCREEN_MAX];
gr2context sda_sys_con;

// GR2 app data structure
gr2Element sda_app_gr2_elements[SDA_APP_ELEM_MAX];
gr2Screen sda_app_gr2_screens[SDA_APP_SCREEN_MAX];
gr2context sda_app_con;

uint16_t sda_splash_screen;

extern psvcKbdLayout kbdLayout;
extern uint8_t oldsec;
extern uint8_t mainDir[258];


void sda_main_init() {
  printf(
      "SDA OS Init: %d:%d:%d %d.%d. %d\n",
      svpSGlobal.hour,
      svpSGlobal.min,
      svpSGlobal.sec,
      svpSGlobal.day,
      svpSGlobal.month,
      svpSGlobal.year
    );

  sda_resource_lock_init();
  led_set_pattern(LED_ALARM);
  LCD_Set_Sys_Font(18);
  svp_setMounted(1); // because SD or FS is already mounted

  sda_check_fs(); // but better to be sure

  // init svm (wrappers, string field, etc)
  svmBootInit();

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
  sda_slot_init(SDA_SLOT_HOMESCREEN, svp_homeScreen(1, 0), &sda_sys_con, 1, 1);
  sda_custom_icon_init();

  sda_slot_init(SDA_SLOT_APPLIST, svp_appScreen(1, 0), &sda_sys_con, 1, 0);
  sda_slot_init(SDA_SLOT_SETTINGS, sda_settings_gui(1, 0), &sda_sys_con, 1, 0);
  svmRun(1, 0);
  sda_slot_init(SDA_SLOT_SVM, 0, &sda_app_con, 0, 0);

  sda_splash_screen = gr2_add_screen(&sda_sys_con);
  gr2_set_str(sda_splash_screen, gr2_get_str(slotScreen[0], &sda_sys_con), &sda_sys_con);
  gr2_text_set_align(gr2_add_text(0, 2, 10, 3, SCR_LOADING_TEXT, sda_splash_screen, &sda_sys_con), GR2_ALIGN_CENTER, &sda_sys_con);
  gr2_set_param(sda_splash_screen, gr2_get_param(slotScreen[0], &sda_sys_con), &sda_sys_con);

  mainScr = sda_splash_screen;
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
    sda_slot_on_top(SDA_SLOT_HOMESCREEN);
  }
}


void sda_main_run_autoexec() {
  // autoexec sits in the APPS directory, it's executed upon boot, if found
  if (svp_fexists((uint8_t *)"autoexec.svs")) {
    svmSetNocacheFlag();
    if (svmLaunch((uint8_t *)"autoexec.svs", 0) == 1) {
      // if it loaded ok, we run it a few times for it to execute the exit call
      svmRun(0, 1);
      svmRun(0, 1);
      svmRun(0, 1);
      svmRun(0, 1);
    }
  }
  sda_slot_on_top(SDA_SLOT_HOMESCREEN);
}
