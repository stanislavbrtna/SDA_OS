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

uint16_t slotScreen[APP_SLOT_MAX];
gr2context * slotScreenContext[APP_SLOT_MAX];

uint16_t slotOnTop[APP_SLOT_MAX];
uint16_t slotValid[APP_SLOT_MAX];

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
uint8_t sleepLock; // flag to disable automatic sleep

uint8_t mainDir[258]; // name of main directory

uint8_t prev_top_slot;

// time of the last input
static uint32_t lastInputTime;

// battery overlay handling
volatile uint8_t systemBattClick;

/*****************************************************************************/
/*                       headers of svs wrappers                             */
/*****************************************************************************/
void pcBasicWrapInit();
void svsGr2WrapInit();

/*****************************************************************************/
/*                            misc SDA functions                             */
/*****************************************************************************/

void testCode() {
  // Testcode section is used for testing new things, it is called once after init
  return;
}

void sdaSlotSetValid(uint16_t slot) {
  slotValid[slot] = 1;
}

void sdaSlotSetInValid(uint16_t slot) {
  slotValid[slot] = 0;
}

uint16_t sdaSlotGetValid(uint16_t slot) {
  return slotValid[slot];
}

void sdaSetSleepLock(uint8_t val) {
  sleepLock = val;
}

uint16_t sdaGetSlotScreen(uint8_t slot) {
  return slotScreen[slot];
}


void svp_switch_main_dir() {
  svp_chdir(mainDir);
}

void setRedrawFlag() {
  svp_set_irq_redraw();
  svpSGlobal.systemRedraw = 1;
}


void sdaSlotOnTop(uint8_t slot) {
  uint8_t x;
  if (!(slotValid[slot]) && (slot <= APP_SLOT_MAX)) {
    printf("sdaSlotOnTop: invalid slot!\n");
    return;
  }

  for (x = 0; x < APP_SLOT_MAX; x++) {
    slotOnTop[x] = 0;
  }
  slotOnTop[slot] = 1;
  if ((mainScr != slotScreen[slot]) || (sda_current_con != slotScreenContext[slot])) {
    mainScr = slotScreen[slot];
    sda_current_con = slotScreenContext[slot];
    setRedrawFlag();
  }

  prev_top_slot = slot;
}

uint8_t sda_get_top_slot() {
  uint8_t x;
  for (x = 0; x < APP_SLOT_MAX; x++) {
    if (slotOnTop[x] == 1 && slotValid[x]) {
      return x;
    }
  }
  return 0;
}

void set_init_struct_defaults() {
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

  svpSGlobal.powerState = PWR_MID;
  svpSGlobal.pwrType = POWER_BATT;
  svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
  svpSGlobal.lcdBacklight = 255;
}

// just simple check for now
void sda_check_fs() {
  if (svp_fexists((uint8_t *)"svp.cfg") == 0) {
    printf("Config file not found!\n");

    // halt for now
    // TODO: check and rebuild directory structure and configs

    LCD_Fill(LCD_MixColor(255, 0, 0));
    LCD_DrawText_ext(32, 100, 0xFFFF, (uint8_t *)"SDA Error:\nConfig file not found!\nFix SD card and press Reset.");

    LCD_DrawText_ext(32, 320, 0xFFFF, (uint8_t *)"SDA-OS v."SDA_OS_VERSION);
#ifdef PC
    getchar();
#else
    while(1);
#endif
  }
}

void sda_power_sleep() {
  if (svpSGlobal.lcdState == LCD_ON) {
    svp_set_lcd_state(LCD_OFF);
  }
  svpSGlobal.powerMode = SDA_PWR_MODE_SLEEP;
  system_clock_set_low();
  svpSGlobal.powerState = PWR_LOW;
}

void sda_lcd_on_handle() {
  system_clock_set_normal();
  svp_set_irq_redraw();
  svpSGlobal.powerState = PWR_MAX;
  svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
}

// wakes from sleep to low power mode with screen off,
// goes to sleep again after lcd shutdown time
void sda_interrupt_sleep() {
  if (svpSGlobal.powerMode == SDA_PWR_MODE_NORMAL) {
    return;
  }
  svpSGlobal.powerState = PWR_LOW;
  svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
  lastInputTime = svpSGlobal.uptime;
}

void sda_lcd_off_handler() {
  svpSGlobal.powerState = PWR_LOW;
  svpSGlobal.powerMode = SDA_PWR_MODE_SLEEP;

  if ((wrap_get_lcdOffButtons() == 1 && slotOnTop[4] && slotValid[4]) || sdaSvmIsTimerSet()) {
    svpSGlobal.powerSleepMode = SDA_PWR_MODE_SLEEP_LOW;
  } else if (sdaGetActiveAlarm() == 1) {
    svpSGlobal.powerSleepMode = SDA_PWR_MODE_SLEEP_NORMAL;
  } else {
    svpSGlobal.powerSleepMode = SDA_PWR_MODE_SLEEP_DEEP;
  }

}

void sda_power_main_handler() {
  static lcdStateType lcdStateOld;
  static volatile uint32_t lcdOffBlinkTimer;
  static volatile uint32_t btnSleepTimer;
  static uint32_t pwrDelay;

  if (svpSGlobal.touchValid) {
    lastInputTime = svpSGlobal.uptime;
    svpSGlobal.powerState = PWR_MAX;
  }

  // when lcd is turned ON
  if ((svpSGlobal.lcdState == LCD_ON) && (lcdStateOld == LCD_OFF)) {
    lastInputTime = svpSGlobal.uptime;
    sda_lcd_on_handle();
    lcdOffBlinkTimer = 0;
    led_set_pattern(LED_OFF);
  }

  // when lcd is turned OFF
  if ((svpSGlobal.lcdState == LCD_OFF) && (lcdStateOld == LCD_ON)) {

    if (((wrap_get_lcdOffButtons() == 1) && slotValid[4] && slotOnTop[4]) || sdaSvmIsTimerSet()) {
      lcdOffBlinkTimer = svpSGlobal.uptimeMs + 100;
    } else if (sdaGetActiveAlarm()){
      lcdOffBlinkTimer = svpSGlobal.uptimeMs + 500;
    } else {
      lcdOffBlinkTimer = svpSGlobal.uptimeMs + 1000;
    }
    led_set_pattern(LED_ON);
    sda_lcd_off_handler();
  }

  if ((lcdOffBlinkTimer != 0) && (lcdOffBlinkTimer < svpSGlobal.uptimeMs)) {
    led_set_pattern(LED_OFF);
    lcdOffBlinkTimer = 0;
    // after we blink the led, system will underclock itself
    // to gave time for apps or system to do stuff after lcd shutdown
    system_clock_set_low();
  }

  if ((btnSleepTimer != 0) && (btnSleepTimer < svpSGlobal.uptimeMs)) {
    btnSleepTimer = 0;
    // to handle the scenario when button is pressed with screen off
    system_clock_set_low();
    sda_power_sleep();
  }

  // lcd auto shut down, this must be at the bottom, so it does not turn off before the sda wakes.
  if (((svpSGlobal.lcdShutdownTime * 60) < (svpSGlobal.uptime - lastInputTime))
        && (sleepLock == 0) && (svpSGlobal.powerMode != SDA_PWR_MODE_SLEEP)) {
    sda_power_sleep();
  }

  lcdStateOld = svpSGlobal.lcdState;

  // mid power after 30s
  if ((15 < (svpSGlobal.uptime - lastInputTime))
        && (svpSGlobal.powerState == PWR_MAX)) {
    svpSGlobal.powerState = PWR_MID;
  }

  if (svpSGlobal.powerMode == SDA_PWR_MODE_SLEEP) {
    return;
  }

  if (svpSGlobal.powerState == PWR_MAX) {
    pwrDelay = 10000;
  }

  if (svpSGlobal.powerState == PWR_MID) {
    pwrDelay = 22000;
  }

  if (svpSGlobal.powerState == PWR_LOW) {
    pwrDelay = 50000;
  }

  for (uint32_t x = 0; x < pwrDelay; x++) { // waiting for next touch event
  #ifdef PC
      break;
  #endif
    if (svpSGlobal.touchValid == 1) {
      lastInputTime = svpSGlobal.uptime;
      break;
    }

    if (svpSGlobal.btnFlag == 1) {
      svpSGlobal.btnFlag = 0;
      lastInputTime = svpSGlobal.uptime;
      svpSGlobal.powerState = PWR_MAX;
      // TODO: this needs cleanup, also it does not work
      // when called from lcd off, we go to sleep after 30s
      if (svpSGlobal.lcdState == LCD_OFF) {
        btnSleepTimer = svpSGlobal.uptimeMs + 5000;
      }
      break;
    }
  }
}

/*****************************************************************************/
/*                             SDA main loop                                 */
/*****************************************************************************/

uint8_t sda_main_loop() {
  static uint8_t init;
  static uint8_t kbdVisibleOld;
  static uint8_t oldsec;

  uint8_t scr_touch_retval = 0;
  static uint8_t kbdRedraw;

  static psvcKbdLayout kbdLayout;

  if (init == 0) {
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

    // Init
    svp_crypto_init();

    set_init_struct_defaults();

    oldsec = 99;
    kbdVisibleOld = 0;
    kbdRedraw = 0;

    init_kblayout_standard(&kbdLayout);

    // loading config from SD
    sda_load_config();

    // initialize screens: home, apps and options
    slotScreen[0] = svp_homeScreen(1, 0);
    slotScreenContext[0] = &sda_sys_con;
    slotValid[0] = 1;
    slotOnTop[0] = 1;

    slotScreen[1] = svp_appScreen(1, 0);
    slotScreenContext[1] = &sda_sys_con;
    slotValid[1] = 1;
    slotOnTop[1] = 0;

    slotScreen[2] = svp_optScreen(1, 0);
    slotScreenContext[2] = &sda_sys_con;
    slotValid[2] = 1;
    slotOnTop[2] = 0;

    sdaSvmRun(1, 0);

    slotScreenContext[4] = &sda_app_con;
    mainScr = slotScreen[0];
    sda_current_con = &sda_sys_con;
    led_set_pattern(LED_ON); // after init, we set led to on

    sdaReloadAlarms();
    sdaPrintActiveAlarm();

    // screen redraw for the first time
    tick_lock = SDA_LOCK_LOCKED;
    LCD_setDrawArea(0, 0, SDA_LCD_W, SDA_LCD_H);
    pscg_draw_screen(0, 32, 319, 479, mainScr, 1, &sda_sys_con);
    tick_lock = SDA_LOCK_UNLOCKED;
    init = 1;
    led_set_pattern(LED_OFF);

    testCode();
  } else if (init == 1) {
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
    sdaSlotOnTop(0);
    init = 2;
  }

  if (svpSGlobal.sec != oldsec) {
    timeUpdateFlag = 1;
    oldsec = svpSGlobal.sec;
  }

  if (svpSGlobal.touchType != EV_NONE) {
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
  sda_store_buttons();
  svp_clr_button_ev();
  svpSGlobal.touchType = EV_NONE;
/*****************************************************************************/
/*                             screen redraw                                 */
/*****************************************************************************/

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

/*****************************************************************************/
/*                            update of screens                              */
/*****************************************************************************/

  // System overlays are updated before screens
  if (sda_batt_overlay_shown()) {
    batt_overlay_handle(0);
  }

  if (soft_error_flag == 1) {
    sda_error_overlay_handle();
  }

  taskSwitcherUpdate();

  // updating screens
  if (slotValid[0]) {
    svp_homeScreen(0, slotOnTop[0]);
  }

  if (slotValid[1]) {
    svp_appScreen(0, slotOnTop[1]);
  }

  if (slotValid[2]) {
    svp_optScreen(0, slotOnTop[2]);
  }

  if (slotValid[4]) {
    sdaSvmRun(0, slotOnTop[4]);
  }


  // top bar button handlers
  // handler for that big S! button
  if ((svpSGlobal.systemOptClick == CLICKED_SHORT)) {
    if(prev_top_slot != 0) {

      svpSGlobal.systemXBtnClick = 0;
      svpSGlobal.systemXBtnVisible = 0;

      // destroy overlay if there is one
      if (getOverlayId() != 0) {
        destroyOverlay();
      }

      hideKeyboard();
      sdaSlotOnTop(0);
      svp_chdir(mainDir);
      svp_chdir((uint8_t *)"APPS");
      sleepLock = 0;
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

/*****************************************************************************/
/*                          end of main loop                                 */
/*****************************************************************************/

  // cleaning input flags
  svpSGlobal.touchValid = 0; //if the touch event was not handled, we discard it
  svpSGlobal.btnFlag = 0;
  timeUpdateFlag = 0;
  sdaSetRedrawDetect(0);
  svpSGlobal.systemXBtnTime = 0; // foreced app kill flag

  // check for notification
  uint8_t notifAppName[APP_NAME_LEN];
  int32_t id;
  int32_t param;
  if (sdaGetCurentAlarm(&id, &param, notifAppName, sizeof(notifAppName))) {
    svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
    setNotificationFlag(id, param);
    sdaSvmLaunch(notifAppName, 0);
  }

  // local timers
  sdaSvmHandleTimers();

  // power management
  sda_power_main_handler();

  // battery status handling
  sda_handle_battery_status();

  return 0;
}
