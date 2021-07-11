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

#include "sda_system_overlays.h"

static uint16_t tov_screen;
static uint16_t tov_pm1;
static uint16_t tov_pm2;
static uint16_t tov_mm1;
static uint16_t tov_mm2;
static uint16_t tov_tm1;
static uint16_t tov_tm2;

static uint16_t tov_ph1;
static uint16_t tov_ph2;
static uint16_t tov_mh1;
static uint16_t tov_mh2;
static uint16_t tov_th1;
static uint16_t tov_th2;

static uint16_t tov_ok;
static uint16_t tov_id = 0xFFFF;
static uint16_t tov_cancel;

static uint16_t tov_minutes;
static uint16_t tov_hours;
static uint16_t tov_done;

static uint8_t tov_min1_str[2];
static uint8_t tov_min2_str[2];

static uint8_t tov_hr1_str[2];
static uint8_t tov_hr2_str[2];

uint16_t time_overlay_init() {
  tov_hours = 0;
  tov_minutes = 0;
  tov_done = 0;
  tov_min1_str[0] = '0';
  tov_min2_str[0] = '0';
  tov_hr1_str[0] = '0';
  tov_hr2_str[0] = '0';

  hideKeyboard();

  tov_screen = pscg_add_screen(sda_current_con);

  pscg_add_text(1, 0, 8, 1, OVRL_ENTER_TIME, tov_screen, sda_current_con);

  tov_th1 = pscg_add_text(1, 2, 2, 3, tov_min1_str, tov_screen, sda_current_con);
  tov_th2 = pscg_add_text(2, 2, 3, 3, tov_min2_str, tov_screen, sda_current_con);

  tov_ph1 = pscg_add_button(1, 1, 2, 2, (uint8_t *)"+", tov_screen, sda_current_con);
  tov_ph2 = pscg_add_button(2, 1, 3, 2, (uint8_t *)"+", tov_screen, sda_current_con);

  tov_mh1 = pscg_add_button(1, 3, 2, 4, (uint8_t *)"-", tov_screen, sda_current_con);
  tov_mh2 = pscg_add_button(2, 3, 3, 4, (uint8_t *)"-", tov_screen, sda_current_con);

  tov_th1 = pscg_add_text(1, 2, 2, 3, tov_hr1_str, tov_screen, sda_current_con);
  tov_th2 = pscg_add_text(2, 2, 3, 3, tov_hr2_str, tov_screen, sda_current_con);

  pscg_text_set_align(
    pscg_add_text(3, 2, 4, 3, (uint8_t *)":", tov_screen, sda_current_con),
    GR2_ALIGN_CENTER,
    sda_current_con
  );

  tov_tm1 = pscg_add_text(4, 2, 5, 3, tov_min1_str, tov_screen, sda_current_con);
  tov_tm2 = pscg_add_text(5, 2, 6, 3, tov_min2_str, tov_screen, sda_current_con);

  tov_pm1 = pscg_add_button(4, 1, 5, 2, (uint8_t *)"+", tov_screen, sda_current_con);
  tov_pm2 = pscg_add_button(5, 1, 6, 2, (uint8_t *)"+", tov_screen, sda_current_con);

  tov_mm1 = pscg_add_button(4, 3, 5, 4, (uint8_t *)"-", tov_screen, sda_current_con);
  tov_mm2 = pscg_add_button(5, 3, 6, 4, (uint8_t *)"-", tov_screen, sda_current_con);

  tov_ok = pscg_add_button(4, 5, 6, 6, OVRL_OK, tov_screen, sda_current_con);
  tov_cancel = pscg_add_button(1, 5, 3, 6, OVRL_CANCEL, tov_screen, sda_current_con);

  pscg_text_set_align(tov_ok, GR2_ALIGN_CENTER, sda_current_con);
  pscg_text_set_align(tov_cancel, GR2_ALIGN_CENTER, sda_current_con);

  pscg_set_xscroll(tov_screen, -16, sda_current_con);

  tov_id = setOverlayScreen(tov_screen, sda_current_con);

  setOverlayY2(272);
  setOverlayDestructor(time_overlay_destructor);

  return tov_id;
}

void time_overlay_destructor() {
  tov_done = 2; // set done to cancel
  pscg_clear_screen_ev(tov_screen, sda_current_con);
  pscg_destroy(tov_screen, sda_current_con);
  setRedrawFlag();
  overlayDestructorDone();
}

void time_overlay_update(uint16_t ovId) {

  if (tov_id != ovId) {
    return;
  }

  if((tov_done == 1) || (tov_done == 2)) {
    return;
  }

  if (pscg_get_event(tov_ok, sda_current_con) == EV_RELEASED) {
    destroyOverlay();
    tov_done = 1;
    return;
  }

  if (pscg_get_event(tov_cancel, sda_current_con) == EV_RELEASED) {
    destroyOverlay();
    tov_done = 2;
    return;
  }
  //hours

  if (pscg_get_event(tov_ph1, sda_current_con) == EV_RELEASED) {
    if ((tov_hr1_str[0] - 48 < 1)
        || ((tov_hr2_str[0] - 48 < 4) && (tov_hr1_str[0] - 48 == 1))) {
        tov_hr1_str[0]++;
        pscg_set_modified(tov_th1, sda_current_con);
    }
  }

  if (pscg_get_event(tov_mh1, sda_current_con) == EV_RELEASED) {
    if (tov_hr1_str[0] - 48 != 0){
      tov_hr1_str[0]--;
      pscg_set_modified(tov_th1, sda_current_con);
    }
  }

  if (pscg_get_event(tov_mh2, sda_current_con) == EV_RELEASED){
    if (tov_hr2_str[0] - 48 != 0) {
      tov_hr2_str[0]--;
      pscg_set_modified(tov_th2, sda_current_con);
    } else {
      if (tov_hr1_str[0] - 48 != 0) {
        tov_hr1_str[0]--;
        tov_hr2_str[0] = '9';
      } else {
        tov_hr1_str[0] = '2';
        tov_hr2_str[0] = '3';
      }
      pscg_set_modified(tov_th1, sda_current_con);
      pscg_set_modified(tov_th2, sda_current_con);
    }
  }

  if (pscg_get_event(tov_ph2, sda_current_con) == EV_RELEASED) {
    if (tov_hr2_str[0] - 48 < 9) {
      if ((tov_hr1_str[0] - 48 < 2)) {
        tov_hr2_str[0]++;
      } else {
        if(tov_hr2_str[0] - 48 < 3) {
          tov_hr2_str[0]++;
        } else {
          tov_hr2_str[0] = '0';
          tov_hr1_str[0] = '0';
          pscg_set_modified(tov_th1, sda_current_con);
        }
      }
      pscg_set_modified(tov_th2, sda_current_con);
    } else {
      if (tov_hr1_str[0] - 48 < 2) {
        tov_hr1_str[0]++;
        pscg_set_modified(tov_th1, sda_current_con);
      }
      tov_hr2_str[0] = '0';
      pscg_set_modified(tov_th2, sda_current_con);
    }
  }

  //minutes
  if (pscg_get_event(tov_pm1, sda_current_con) == EV_RELEASED) {
    if (tov_min1_str[0] - 48 < 5) {
      tov_min1_str[0]++;
      pscg_set_modified(tov_tm1, sda_current_con);
    }
  }

  if (pscg_get_event(tov_mm1, sda_current_con) == EV_RELEASED) {
    if (tov_min1_str[0]-48 != 0) {
      tov_min1_str[0]--;
      pscg_set_modified(tov_tm1, sda_current_con);
    }
  }

  if (pscg_get_event(tov_mm2, sda_current_con) == EV_RELEASED) {
    if (tov_min2_str[0] - 48 != 0) {
      tov_min2_str[0]--;
      pscg_set_modified(tov_tm2, sda_current_con);
    } else {
      if (tov_min1_str[0] - 48 != 0) {
        tov_min2_str[0]='9';
        tov_min1_str[0]--;
      } else {
        tov_min1_str[0] = '5';
        tov_min2_str[0] = '9';
      }
      pscg_set_modified(tov_tm2, sda_current_con);
       pscg_set_modified(tov_tm1, sda_current_con);
    }
  }

  if (pscg_get_event(tov_pm2, sda_current_con) == EV_RELEASED) {
    if (tov_min2_str[0] - 48 < 9) {
      tov_min2_str[0]++;
      pscg_set_modified(tov_tm2, sda_current_con);
    } else {
      if (tov_min1_str[0] - 48 < 5) {
        tov_min1_str[0]++;
        tov_min2_str[0] = '0';
        pscg_set_modified(tov_tm2, sda_current_con);
        pscg_set_modified(tov_tm1, sda_current_con);
      } else {
        tov_min2_str[0] = '0';
        tov_min1_str[0] = '0';
        pscg_set_modified(tov_tm1, sda_current_con);
        pscg_set_modified(tov_tm2, sda_current_con);
      }
    }
  }

  pscg_clear_screen_ev(tov_screen, sda_current_con);
}

void time_overlay_set_time(uint16_t ovId, uint16_t hour, uint16_t min){
  if (tov_id != ovId){
    return;
  }

  if (hour > 23){
    return;
  }

  if (min > 59){
    return;
  }

  tov_hr1_str[0] = 48 + hour / 10;
  tov_hr2_str[0] = 48 + hour % 10;

  tov_min1_str[0] = 48+min / 10;
  tov_min2_str[0] = 48+min % 10;
}

uint16_t time_overlay_get_hours(uint16_t ovId) {
  if (tov_id != ovId) {
    return 0;
  }
  return ((tov_hr1_str[0] - 48) * 10 + (tov_hr2_str[0] - 48));
}

uint16_t time_overlay_get_minutes(uint16_t ovId) {
  if (tov_id != ovId) {
    return 0;
  }
  return ((tov_min1_str[0] - 48) * 10 + (tov_min2_str[0] - 48));
}

uint16_t time_overlay_get_ok(uint16_t ovId) {
  if (tov_id != ovId) {
    return 0;
  }
  if (tov_done == 1) {
    return 1;
  } else {
    return 0;
  }
}

void time_overlay_clear_ok(uint16_t ovId) {
  if (tov_id != ovId) {
    return;
  }
  tov_done = 0;
  tov_id = 0xFFFF;
}

