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

#include "sda_misc.h"

// globals
extern uint8_t timeUpdateFlag;
extern volatile uint16_t sdaAppCounter;

static gr2EventType keyEvLocal[6];

uint8_t sda_get_keyboard_key_flag() {
  if (svpSGlobal.kbdFlag == 1) {
    svpSGlobal.kbdFlag = 0;
    return 1;
  }
  return 0;
}


uint16_t sda_get_uptime() {
  return svpSGlobal.uptime;
}


uint8_t sda_get_time_update_flag() {
  return timeUpdateFlag;
}


void sda_set_time_update_flag() {
  timeUpdateFlag = 1;
}


void sda_keyboard_show() {
  svpSGlobal.kbdVisible = 1;
}


void sda_keyboard_hide() {
  gr2_text_deactivate(&sda_sys_con);
  gr2_text_deactivate(&sda_app_con);
  svpSGlobal.kbdKeyStr[0] = 0;
  svpSGlobal.kbdVisible = 0;
}


void set_sda_counter(uint16_t val) {
  sdaAppCounter = val;
}


uint16_t get_sda_counter() {
  return sdaAppCounter;
}


void gr2_error_callback(uint8_t *str, gr2context * c) {
  if (c == &sda_sys_con) {
    // error occured in system gui, perform reset
    printf("PSCG Error (system): %s\n", str);
    svs_hardErrHandler();
  } else {
    // error occured inside application
    // kill app, show error
    printf("PSCG Error (app): %s\n", str);
    svmSetError(str);
    svmKillRunning();
  }
}


void sda_clr_button_ev() {
  svpSGlobal.keyEv[0] = EV_NONE;
  svpSGlobal.keyEv[1] = EV_NONE;
  svpSGlobal.keyEv[2] = EV_NONE;
  svpSGlobal.keyEv[3] = EV_NONE;
  svpSGlobal.keyEv[4] = EV_NONE;
  svpSGlobal.keyEv[5] = EV_NONE;
}


gr2EventType sda_wrap_get_button(uint8_t num) {
  if (svpSGlobal.lcdLandscape == 0) {
    return keyEvLocal[num];
  } else {
    if (num == BUTTON_A || num == BUTTON_B) {
      return keyEvLocal[num];
    }
    if (num == BUTTON_LEFT) return keyEvLocal[BUTTON_UP];
    if (num == BUTTON_UP) return keyEvLocal[BUTTON_RIGHT];
    if (num == BUTTON_DOWN) return keyEvLocal[BUTTON_LEFT];
    if (num == BUTTON_RIGHT) return keyEvLocal[BUTTON_DOWN];
  }

  return EV_NONE;
}


void sda_wrap_clear_button(uint8_t num) {
  if (svpSGlobal.lcdLandscape == 0) {
    keyEvLocal[num] = EV_NONE;
  } else {
    if (num == 0 || num == 5) {
      keyEvLocal[num] = EV_NONE;
    }
    if (num == BUTTON_LEFT) keyEvLocal[BUTTON_UP] = EV_NONE;
    if (num == BUTTON_UP) keyEvLocal[BUTTON_RIGHT] = EV_NONE;
    if (num == BUTTON_DOWN) keyEvLocal[BUTTON_LEFT] = EV_NONE;
    if (num == BUTTON_RIGHT) keyEvLocal[BUTTON_DOWN] = EV_NONE;
  }
}


void sda_store_buttons() {
  for (uint16_t i = 0; i < 6; i++) {
    keyEvLocal[i] = svpSGlobal.keyEv[i];
  }
}


uint16_t sda_get_shadow_color16(uint16_t color) {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t round, round_light;
  r = (uint8_t)(((float)((color >> 11) & 0x1F) / 32) * 256);
  g = (uint8_t)(((float)(((color & 0x07E0) >> 5) & 0x3F) / 64) * 256);
  b = (uint8_t)(((float)(color & 0x1F) / 32) * 256);

  round = (uint8_t)(((uint16_t)r + (uint16_t)g + (uint16_t)b) / 4);
  round_light = (uint8_t)(((uint16_t)r + (uint16_t)g + (uint16_t)b) / 3);

  if ((round - round_light) < 20) {
    round += 20;
  }

  return LCD_MixColor(round, round, round);
}


void sda_draw_overlay_shadow(
  int16_t overlayX1,
  int16_t overlayY1,
  int16_t overlayX2,
  int16_t overlayY2,
  gr2context *c
) {
  LCD_drawArea area;

  LCD_getDrawArea(&area);
  LCD_setDrawArea(overlayX1, overlayY1, overlayX2 + 11, overlayY2 +11);
  LCD_FillRect(
        overlayX2 + 1,
        overlayY1 + 10,
        overlayX2 + 11,
        overlayY2 + 1,
        sda_get_shadow_color16(c->background_color)
  );
  LCD_FillRect(
        overlayX1 + 10,
        overlayY2 + 1,
        overlayX2 + 11,
        overlayY2 + 11,
        sda_get_shadow_color16(c->background_color)
  );
  LCD_setDrawAreaS(&area);
}


void sda_sw_halt_screen(uint8_t * message) {
  LCD_Fill(LCD_MixColor(255, 0, 0));
  LCD_DrawText_ext(32, 100, 0xFFFF, (uint8_t *)"SDA Error:");
  LCD_DrawText_ext(32, 130, 0xFFFF, message);

  LCD_DrawText_ext(32, 320, 0xFFFF, (uint8_t *)"SDA-OS v."SDA_OS_VERSION);

  printf("%s: %s\n", __FUNCTION__, message);
#ifdef PC
  getchar();
#else
  while(1);
#endif
}


void sda_check_fs() {
  // check for config file
  if (svp_fexists((uint8_t *)"svp.cfg") == 0) {
    printf("Config file not found!, going with defaults\n");

    // create new empty conf file, sda_load_config() will fill it with defaults
    if (sda_fs_touch((uint8_t *)"svp.cfg")) {
      sda_sw_halt_screen((uint8_t *)"Cannot create system config file!\nFix SD card and press Reset.");
    }
  }

  // create main directories
  if(
    sda_fs_check_and_create_dir((uint8_t *)"APPS")
    || sda_fs_check_and_create_dir((uint8_t *)"DATA")
    || sda_fs_check_and_create_dir((uint8_t *)"DATA/appdata")
    || sda_fs_check_and_create_dir((uint8_t *)"DATA/settings")
    || sda_fs_check_and_create_dir((uint8_t *)"APPS/cache")
    || sda_fs_check_and_create_dir((uint8_t *)"APPS/lib")
    || sda_fs_check_and_create_dir((uint8_t *)"APPS/Icons")
  ) {
    sda_sw_halt_screen((uint8_t *)"Cannot create system files!\nFix SD card and press Reset.");
  }    
}


void sda_set_landscape(uint8_t val) {
  if (val == 1) {
    LCD_set_orientation(OR_ROT_RIGHT);
    svpSGlobal.lcdLandscape = 1;
  } else {
    LCD_set_orientation(OR_NORMAL);
    svpSGlobal.lcdLandscape = 0;
  }
  setRedrawFlag();
}


void sda_set_time(
  uint16_t year,
  uint8_t  day,
  uint8_t  wkday,
  uint8_t  month,
  uint8_t  hour,
  uint8_t  min,
  uint8_t  sec
) {
  #ifdef PLATFORM_PC
  printf("setting time: %u:%u %u. %u. %u (Only date setting works in simulation)\n", hour, min, day, month, year);
  svpSGlobal.day = day;
  svpSGlobal.month = month;
  if (year > 2005) {
    svpSGlobal.year = year;
  }
  #else
  printf("Old time: y:%u d:%u wkd:%u mon:%u %u:%u \n",svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, svpSGlobal.hour, svpSGlobal.min);
  rtc_set_time(year, day, wkday, month, hour, min, sec);
  printf("New time: y:%u d:%u wkd:%u mon:%u %u:%u \n",svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, svpSGlobal.hour, svpSGlobal.min);
  #endif

  svpSGlobal.dateUpdated = 1;
}


void sda_set_init_struct_defaults() {
  svpSGlobal.uptime = 0;
  svpSGlobal.lcdOnTime = 0;
  svpSGlobal.dateUpdated = 0;
  svpSGlobal.systemRedraw = 0;
  svpSGlobal.lcdLandscape = 0;
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

  svpSGlobal.breakP16Draw = 0;
}
