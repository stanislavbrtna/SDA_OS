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

static uint16_t task_switcher;
static uint16_t task_switcher_inner;
static uint16_t task_overlay;
static uint8_t valid;

static uint16_t appButtons[MAX_OF_SAVED_PROC];
static uint16_t appButtonsClose[MAX_OF_SAVED_PROC];
static uint16_t appPid[MAX_OF_SAVED_PROC];
static uint16_t ok;
static uint16_t close_all;
static uint16_t scrollbar;
static uint16_t numberOfApps;

static uint8_t niceSuspendName[MAX_OF_SAVED_PROC][35];

//#define APP_SWITCHER_DEBUG

static void reloadNiceNames() {
  uint8_t *buff;
  uint32_t len;
  uint16_t slash;

  for(uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    slash = 0;
    buff = svmGetSuspendedName(x);

    if (buff == 0) {
      continue;
    }

    len = sda_strlen(buff);

    for(uint16_t i = 0; i < len; i++) {
      if (buff[i] == '/') {
        slash = i;
      }
    }

    if (slash == 0) {
      sda_strcp(buff, niceSuspendName[x], 35);
    } else {
      sda_strcp(buff + slash + 1, niceSuspendName[x], 35);
    }

  }
}

void taskSwitcherDestructor() {
  gr2_destroy(task_switcher, &sda_sys_con);
#ifdef APP_SWITCHER_DEBUG
  printf("task switcher destructor called\n");
#endif
  valid = 0;
}

void taskSwitcherOpen() {
  if (valid == 1) {
#ifdef APP_SWITCHER_DEBUG
    printf("failed to open already valid\n");
#endif
    return;
  }
#ifdef APP_SWITCHER_DEBUG
  printf("task switcher opened\n");
#endif

  if (svpSGlobal.sdaDeviceLock == DEVICE_LOCKED) {
    return;
  }

  sda_keyboard_hide();

  uint16_t n = 0;
  gr2_set_relative_init(1, &sda_sys_con);
  task_switcher = gr2_add_screen(&sda_sys_con);
  gr2_set_cell_spacing(task_switcher, 2, 1, 2, 1, &sda_sys_con);
  gr2_set_x_cell(task_switcher, 19, &sda_sys_con);
  gr2_set_y_cell(task_switcher, 18, &sda_sys_con);
  gr2_set_yscroll(task_switcher, -8, &sda_sys_con);
  gr2_set_xscroll(task_switcher, -12, &sda_sys_con);

  task_switcher_inner = gr2_add_screen(&sda_sys_con);

  gr2_set_screen(task_switcher_inner, task_switcher, &sda_sys_con);
  gr2_set_x1y1x2y2(task_switcher_inner, 0, 2, 12, 14 - svpSGlobal.lcdLandscape*6, &sda_sys_con);

  gr2_set_cell_spacing(task_switcher_inner, 1, 2, 1, 1, &sda_sys_con);
  gr2_set_yscroll(task_switcher_inner, -5, &sda_sys_con);
  gr2_set_xscroll_initial(task_switcher_inner, -4, &sda_sys_con);
  gr2_set_x_cell(task_switcher_inner, 31, &sda_sys_con);

  gr2_add_text(
    0, 0, 14, 2,
    SWITCH_RUNNING_APPS,
    task_switcher,
    &sda_sys_con
  );
  scrollbar = gr2_add_slider_v(
    12, 2, 2, 14 - svpSGlobal.lcdLandscape*6,
    MAX_OF_SAVED_PROC * 32 - 7*32,
    0,
    task_switcher,
    &sda_sys_con
  );

  for(uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    appPid[n] = svmGetSuspendedPid(x);

    if(appPid[n] == 0) {
      continue;
    }

    reloadNiceNames();

    appButtons[n] = gr2_add_button(
      0, n, 6, 1,
      niceSuspendName[x],
      task_switcher_inner,
      &sda_sys_con
    );

    appButtonsClose[n] = gr2_add_button(
      6, n, 1, 1,
      (uint8_t *)"-",
      task_switcher_inner,
      &sda_sys_con
    );
    gr2_text_set_align(appButtonsClose[n], GR2_ALIGN_CENTER, &sda_sys_con);

    n++;
  }
  numberOfApps = n;

  ok = gr2_add_button(9, 16 - svpSGlobal.lcdLandscape*6, 3, 2, OVRL_OK, task_switcher, &sda_sys_con);

  close_all = gr2_add_button(
    0, 16 - svpSGlobal.lcdLandscape*6, 7, 2,
    SWITCH_CLOSE_ALL,
    task_switcher,
    &sda_sys_con
  );

  gr2_text_set_align(ok, GR2_ALIGN_CENTER, &sda_sys_con);
  gr2_text_set_align(close_all, GR2_ALIGN_CENTER, &sda_sys_con);

  task_overlay = setOverlayScreen(task_switcher, &sda_sys_con);
  setOverlayX1(16 + svpSGlobal.lcdLandscape*80);
  setOverlayY1(32 + 16);
  setOverlayX2(320 - 16 + svpSGlobal.lcdLandscape*80);
  setOverlayY2(480 - 80 - 8 - 104*svpSGlobal.lcdLandscape);

  setOverlayDestructor(taskSwitcherDestructor);
  valid = 1;

  if (numberOfApps == 0) {
    gr2_set_grayout(close_all, 1, &sda_sys_con);
    gr2_set_grayout(scrollbar, 1, &sda_sys_con);
  }
  gr2_set_relative_init(0, &sda_sys_con);
}


void taskSwitcherUpdate() {
  if (valid == 0) {
    return;
  }

  for(uint16_t x = 0; x < numberOfApps; x++) {
    if (gr2_get_event(appButtons[x], &sda_sys_con) == EV_RELEASED) {
      sda_keyboard_hide();
      if (svmWake(appPid[x])) {
        sda_show_error_message((uint8_t *)"Error occured while waking app.");
      }
      setRedrawFlag();
      destroyOverlay();
      return;
    }

    if (gr2_get_event(appButtonsClose[x], &sda_sys_con) == EV_RELEASED) {
      svmClose(appPid[x]);
      int32_t prevScroll = gr2_get_value(scrollbar, &sda_sys_con);
      destroyOverlay();
      taskSwitcherOpen();
      gr2_set_value(scrollbar, prevScroll, &sda_sys_con);
      return;
    }

    gr2_set_event(appButtons[x], EV_NONE, &sda_sys_con);
    gr2_set_event(appButtonsClose[x], EV_NONE, &sda_sys_con);
  }

  if (gr2_get_event(ok, &sda_sys_con) == EV_RELEASED) {
    setRedrawFlag();
    destroyOverlay();
    return;
  }
  gr2_set_event(ok, EV_NONE, &sda_sys_con);

  if (gr2_get_event(close_all, &sda_sys_con) == EV_RELEASED) {
    svmCloseAll();
    destroyOverlay();
    return;
  }
  gr2_set_event(close_all, EV_NONE, &sda_sys_con);

  gr2_set_yscroll(
    task_switcher_inner,
    gr2_get_value(scrollbar, &sda_sys_con) - 5,
    &sda_sys_con
  );

  sda_screen_button_handler(task_switcher, ok, &sda_sys_con);
}
