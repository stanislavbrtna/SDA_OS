/*
Copyright (c) 2023 Stanislav Brtna

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

// system overlay
uint16_t overlayScr;
gr2context * overlayCont;
uint16_t overlayX1;
uint16_t overlayX2;
uint16_t overlayY1;
uint16_t overlayY2;
uint16_t ov_id; // current overlay id


uint8_t kbdRedraw;
psvcKbdLayout kbdLayout;
uint8_t kbdVisibleOld;

extern sdaSvmMetadata svmMeta;


void sda_main_redraw() {
  uint8_t dirBuff[256];

  if(sda_get_top_slot() == 4 && svmGetValid() && svmMeta.useDrawRoot == 1) {
    svp_getcwd(dirBuff, 256);
    svp_switch_main_dir();
    svp_chdir(svmMeta.drawRoot);
  }
  

  // lock tick for redraw
  tick_lock = SDA_LOCK_LOCKED;
  LCD_setDrawArea(0, 0, SDA_LCD_W + 160 * svpSGlobal.lcdLandscape, SDA_LCD_H - 160 * svpSGlobal.lcdLandscape);
  if (svpSGlobal.systemRedraw == 1 || kbdRedraw) {
    sdaSetRedrawDetect(1);
  }

  if ((svpSGlobal.kbdVisible == 1 && kbdVisibleOld == 0)
      || (svpSGlobal.systemRedraw && svpSGlobal.kbdVisible == 1)
      || kbdRedraw
    ) {
    if (svpSGlobal.lcdLandscape) {
      LCD_FillRect(1, 320 - 160, 80, 319, sda_current_con->background_color);
      LCD_FillRect(401, 320 - 160, 479, 319, sda_current_con->background_color);
    }
    svp_draw_keyboard(80*svpSGlobal.lcdLandscape, 319 - 160*svpSGlobal.lcdLandscape, &kbdLayout);
    if (kbdRedraw == 0) {
      svpSGlobal.systemRedraw = 1;
    }
    kbdRedraw = 0;
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

  LCD_setDrawArea(
    0,
    0,
    SDA_LCD_W - 1 + 160 * svpSGlobal.lcdLandscape,
    SDA_LCD_H - 160 * svpSGlobal.kbdVisible - 160 * svpSGlobal.lcdLandscape
  );
  if (overlayScr == 0) {
    if (svpSGlobal.systemRedraw == 1) {
      gr2_draw_screen(
        0,
        32,
        319 + 160 * svpSGlobal.lcdLandscape,
        479 - 160 * svpSGlobal.kbdVisible - 160 * svpSGlobal.lcdLandscape,
        mainScr,
        1,
        sda_current_con
      );
      svpSGlobal.systemRedraw = 0;
    } else {
      gr2_draw_screen(
        0,
        32,
        319 + 160 * svpSGlobal.lcdLandscape,
        479 - 160 * svpSGlobal.kbdVisible - 160*svpSGlobal.lcdLandscape,
        mainScr,
        0,
        sda_current_con
      );
    }
  } else {
    if (svpSGlobal.systemRedraw == 1) {
      if (mainScr != 0) {
        gr2_draw_screen(
          0,
          32,
          319 + 160 * svpSGlobal.lcdLandscape,
          479 - 160 * svpSGlobal.kbdVisible - 160*svpSGlobal.lcdLandscape,
          mainScr,
          1,
          sda_current_con
        );
      }
      sda_draw_overlay_shadow(
            overlayX1,
            overlayY1,
            overlayX2,
            overlayY2,
            overlayCont
      );
      gr2_draw_screen(
            overlayX1,
            overlayY1,
            overlayX2,
            overlayY2,
            overlayScr,
            1,
            overlayCont
      );
      LCD_DrawRectangle(overlayX1 - 1 , overlayY1 - 1, overlayX2 + 1, overlayY2 + 1, sda_current_con->border_color);
      svpSGlobal.systemRedraw = 0;
    }
    if(overlayCont->pscgElements[overlayScr].modified) {
      sda_draw_overlay_shadow(
            overlayX1,
            overlayY1,
            overlayX2,
            overlayY2,
            overlayCont
      );
    }
    LCD_DrawRectangle(overlayX1 - 1 , overlayY1 - 1, overlayX2 + 1, overlayY2 + 1, sda_current_con->border_color);
    gr2_draw_screen(overlayX1, overlayY1, overlayX2, overlayY2, overlayScr, 0, overlayCont);
  }
  gr2_draw_end(sda_current_con);
  
  if (overlayScr != 0) {
    gr2_draw_end(overlayCont);
  }

  if(sda_get_top_slot() == 4 && svmGetValid() && svmMeta.useDrawRoot == 1) {
    svp_switch_main_dir();
    svp_chdir(dirBuff);
  }

  tick_lock = SDA_LOCK_UNLOCKED;
}


void sda_main_process_touch() {
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
            80*svpSGlobal.lcdLandscape,
            319 - 160*svpSGlobal.lcdLandscape,
            &kbdLayout,
            svpSGlobal.touchX,
            svpSGlobal.touchY,
            svpSGlobal.kbdKeyStr,
            svpSGlobal.touchType
          );
    touch_lock = SDA_LOCK_UNLOCKED;

    if (retVal != 0) {
      if (retVal == 2) { // esc
        gr2_text_deactivate(&sda_sys_con);
        gr2_text_deactivate(&sda_app_con);
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
      scr_touch_retval = gr2_touch_input(
        0,
        32,
        319 + 160*svpSGlobal.lcdLandscape,
        479 - 160 * svpSGlobal.kbdVisible - 160*svpSGlobal.lcdLandscape,
        svpSGlobal.touchX,
        svpSGlobal.touchY,
        svpSGlobal.touchType,
        mainScr,
        sda_current_con
      );

      if (scr_touch_retval == 2) { // retval 2 means open the keyboard
        if (svpSGlobal.inputMethod == ON_SCREEN_KEYBOARD) {
          sda_keyboard_show();
        }
        svpSGlobal.kbdKeyStr[0] = 0;
      }
    }
  } else {
    // touch in overlay
    if ((svpSGlobal.touchType != EV_NONE)) {
      scr_touch_retval = gr2_touch_input(
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
        sda_keyboard_show();
      }
    }

    if (((svpSGlobal.touchX < overlayX1 - 10)
        || (svpSGlobal.touchX > overlayX2 + 10)
        || (svpSGlobal.touchY < overlayY1 - 10 && svpSGlobal.touchY > 32)
        || (svpSGlobal.touchY > overlayY2 + 10))
        && svpSGlobal.kbdVisible == 0 && kbdVisibleOld == 0
        && svpSGlobal.touchType == EV_PRESSED) {
      destroyOverlay();
      setRedrawFlag();
    }
  }
  sx_set_touch_ev(svpSGlobal.touchType, svpSGlobal.touchX, svpSGlobal.touchY);

}