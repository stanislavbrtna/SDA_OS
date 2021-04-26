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
extern uint8_t sleepLock;

static gr2EventType keyEvLocal[6];

uint8_t getKbdKey() {
  if (svpSGlobal.kbdFlag == 1) {
    svpSGlobal.kbdFlag = 0;
    return 1;
  }
  return 0;
}


uint16_t sda_get_uptime() {
  return svpSGlobal.uptime;
}


uint8_t getTimeUpdateFlag() {
  return timeUpdateFlag;
}


void setTimeUpdateFlag() {
  timeUpdateFlag = 1;
}


void showKeyboard() {
  svpSGlobal.kbdVisible = 1;
}


void hideKeyboard() {
  pscg_text_deactivate(&sda_sys_con);
  pscg_text_deactivate(&sda_app_con);
  svpSGlobal.kbdKeyStr[0] = 0;
  svpSGlobal.kbdVisible = 0;
}


void set_sda_counter(uint16_t val) {
  sdaAppCounter = val;
}


uint16_t get_sda_counter() {
  return sdaAppCounter;
}


void pscg_error_callback(uint8_t *str, gr2context * c) {
  if (c == &sda_sys_con) {
    // error occured in system gui, perform reset
    printf("PSCG Error (system): %s\n", str);
    svs_hardErrHandler();
  } else {
    // error occured inside application
    // kill app, show error
    printf("PSCG Error (app): %s\n", str);
    sdaSvmSetError(str);
    sdaSvmKillApp();
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
  return keyEvLocal[num];
}


void sda_wrap_clear_button(uint8_t num) {
  keyEvLocal[num] = EV_NONE;
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
}


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