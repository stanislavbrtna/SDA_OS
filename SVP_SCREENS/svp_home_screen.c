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

// homescreen handler
uint16_t svp_homeScreen(uint8_t init, uint8_t top) {
  static uint16_t screen;
  static uint16_t appsBtn;
  static uint16_t optBtn;
  static uint16_t lockBtn;
  static uint16_t text;
  static uint16_t time;
  static uint16_t date;
  static uint16_t oldtime;
  static uint8_t olddate;
  static sdaDeviceLockType oldLock;
  static uint16_t unlockOverlay;
  static uint8_t time_string[6];
  static uint8_t date_string[42];

  if (init == 1) {
    screen = gr2_add_screen(&sda_sys_con);
    text = gr2_add_text(0 , 1, 12, 3,(uint8_t *)"S!   PDA", screen, &sda_sys_con);
    time = gr2_add_text(3, 4, 12, 6,(uint8_t *)"??:??", screen, &sda_sys_con);
    date = gr2_add_text(3, 6, 12, 7,(uint8_t *)"?. ?. 20??", screen, &sda_sys_con);
    appsBtn = gr2_add_icon(2, 10, 5, 13,(uint8_t *)"", (uint8_t *)"Icons/apps.p16", screen, &sda_sys_con);
    optBtn = gr2_add_icon(6, 10, 9, 13,(uint8_t *)"", (uint8_t *)"Icons/options.p16", screen, &sda_sys_con);
    lockBtn = gr2_add_icon(4, 10, 7, 13,(uint8_t *)"", (uint8_t *)"Icons/lock.p16", screen, &sda_sys_con);
    gr2_set_visible(lockBtn, 0, &sda_sys_con);
    gr2_text_set_size(text, 70, &sda_sys_con);
    gr2_text_set_size(time, 70, &sda_sys_con);
    oldtime = 5566; // absurd value, so current minute would not equal oldmin and time would update after init
    oldLock = DEVICE_UNLOCKED;
    return screen;
  }

  // loop top
  if (top == 1) {
    svpSGlobal.systemXBtnVisible = 0;

    // time refresh
    if (((uint16_t)svpSGlobal.hour * 10) + (uint16_t)svpSGlobal.min != oldtime) {
      time_string[0] = svpSGlobal.hour / 10 + 48;
      time_string[1] = svpSGlobal.hour % 10 + 48;
      time_string[2] = ':';
      time_string[3] = svpSGlobal.min / 10 + 48;
      time_string[4] = svpSGlobal.min % 10 + 48;
      time_string[5] = 0;
      gr2_set_str(time, time_string, &sda_sys_con);
      oldtime = ((uint16_t)svpSGlobal.hour * 10) + (uint16_t)svpSGlobal.min;
    }

    // date refresh
    if ((svpSGlobal.day != olddate) || (svpSGlobal.dateUpdated)) {
      svp_write_date_string(date_string, 0, 1);
      gr2_set_str(date, date_string, &sda_sys_con);
      gr2_set_modified(date, &sda_sys_con);
      olddate = svpSGlobal.day;
      svpSGlobal.dateUpdated = 0;
    }

    // lock refresh
    if (svpSGlobal.sdaDeviceLock != oldLock) {
      if (svpSGlobal.sdaDeviceLock == DEVICE_LOCKED) {
        gr2_set_visible(lockBtn, 1, &sda_sys_con);
        gr2_set_visible(optBtn, 0, &sda_sys_con);
        gr2_set_visible(appsBtn, 0, &sda_sys_con);
      } else {
        gr2_set_visible(lockBtn, 0, &sda_sys_con);
        gr2_set_visible(optBtn, 1, &sda_sys_con);
        gr2_set_visible(appsBtn, 1, &sda_sys_con);
      }
      oldLock = svpSGlobal.sdaDeviceLock;
    }

    if (gr2_get_event(lockBtn, &sda_sys_con) == EV_RELEASED) {
      unlockOverlay = password_overlay_init();
    }
    gr2_set_event(lockBtn, EV_NONE, &sda_sys_con);

    password_overlay_update(unlockOverlay);

    if(password_overlay_get_ok(unlockOverlay) == 1) {
      password_overlay_clear_ok(unlockOverlay);
      svp_crypto_lock();
      svpSGlobal.sdaDeviceLock = DEVICE_UNLOCKED;
      rtc_write_locked(0);
    }

    if(password_overlay_get_ok(unlockOverlay) == 2) {
      password_overlay_clear_ok(unlockOverlay);
    }

    if (gr2_get_event(appsBtn, &sda_sys_con) == EV_RELEASED) {
      gr2_ki_unselect(screen, &sda_sys_con);
      sda_slot_on_top(1);
    }
    gr2_set_event(appsBtn, EV_NONE, &sda_sys_con);

    if (gr2_get_event(optBtn, &sda_sys_con) == EV_RELEASED) {
      gr2_ki_unselect(screen, &sda_sys_con);
      sda_slot_on_top(2);
    }
    gr2_set_event(optBtn, EV_NONE, &sda_sys_con);

    sda_screen_button_handler(screen, 0, &sda_sys_con);
  }
  //else: work in the background, noting for now
  return 0;
}
