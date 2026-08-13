/*
Copyright (c) 2021 Stanislav Brtna

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

extern gr2context sda_sys_con;

static uint16_t screen;
// static uint16_t passInput;
static uint8_t pinInputStr[33];
static uint16_t keypadButtons[12];
static uint16_t okButton;
static uint16_t cancelButton;
static uint16_t povId;
static uint16_t povDone;
static uint8_t povOnlyGet;
static uint16_t povCount;

const uint8_t *buttonNames[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "A", "B"};

void pin_overlay_destructor();

void switch_xy(uint16_t id1, uint16_t id2) {
  int16_t x, y;

  x = gr2_get_x1(id1, &sda_sys_con);
  y = gr2_get_y1(id1, &sda_sys_con);

  gr2_set_x1(id1, gr2_get_x1(id2, &sda_sys_con), &sda_sys_con);
  gr2_set_y1(id1, gr2_get_y1(id2, &sda_sys_con), &sda_sys_con);

  gr2_set_x1(id2, x, &sda_sys_con);
  gr2_set_y1(id2, y, &sda_sys_con);
}

uint16_t pin_overlay_init(uint8_t onlyGet) {
  pinInputStr[0] = 0;
  povCount = 0;

  povOnlyGet = onlyGet;

  uint8_t rInit = gr2_get_relative_init(&sda_sys_con);
  gr2_set_relative_init(1, &sda_sys_con);

  screen = gr2_add_screen(&sda_sys_con);
  gr2_set_y_cell(screen, 16, &sda_sys_con);
  gr2_set_yscroll(screen, -14, &sda_sys_con);

  gr2_add_text(1, 0, 7, 2, OVRL_ENTER_PASSWORD, screen, &sda_sys_con);

  for (uint16_t i = 0; i < 12; i++) {
    keypadButtons[i] = gr2_add_button(
        1 + (i % 3) * 2, 2 + (i / 3) * 4, 2, 4, buttonNames[i], screen, &sda_sys_con);
    gr2_text_set_size(keypadButtons[i], FONT_32, &sda_sys_con);
    gr2_text_set_align(keypadButtons[i], GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_set_param(keypadButtons[i], 10, &sda_sys_con);
  }

  if (!onlyGet) {
    for (uint16_t i = 0; i < 20; i++) {
      switch_xy(keypadButtons[svp_random() % 12], keypadButtons[svp_random() % 12]);
    }
  }

  okButton = gr2_add_button(4, 19, 3, 2, OVRL_OK, screen, &sda_sys_con);
  cancelButton = gr2_add_button(1, 19, 2, 2, OVRL_CANCEL, screen, &sda_sys_con);

  gr2_text_set_align(okButton, GR2_ALIGN_CENTER, &sda_sys_con);
  gr2_text_set_align(cancelButton, GR2_ALIGN_CENTER, &sda_sys_con);

  povId = setOverlayScreen(screen, &sda_sys_con);

  gr2_set_x_offset(screen, 0, &sda_sys_con);

  setOverlayY2(288 + 64 + 64 + 16);

  if (sda_crypto_get_if_set_up() == 0) {
    gr2_set_grayout(okButton, 1, &sda_sys_con);
  }

  povDone = 0;

  gr2_set_relative_init(rInit, &sda_sys_con);

  setOverlayDestructor(pin_overlay_destructor);

  return povId;
}

void pin_overlay_update(uint16_t ovId) {

  if (povId != ovId || ovId == 0 || povId != getOverlayId()) {
    return;
  }

  if ((povDone == 1) || (povDone == 2)) {
    return;
  }

  if (gr2_get_event(okButton, &sda_sys_con) == EV_RELEASED) {
    if (povOnlyGet) {
      destroyOverlay();
      povDone = 3;
      return;
    } else if (sda_crypto_verify_pin(pinInputStr)) {
      printf("Pin failed\n");
      svpSGlobal.unlockCounter++;
      destroyOverlay();
      povDone = 2;
      return;
    } else {
      destroyOverlay();
      povDone = 1;
      return;
    }
  }

  if (gr2_get_event(cancelButton, &sda_sys_con) == EV_RELEASED) {
    destroyOverlay();
    povDone = 2;
    return;
  }

  for (uint16_t i = 0; i < 12; i++) {

    if (gr2_get_event(keypadButtons[i], &sda_sys_con) == EV_RELEASED) {
      pinInputStr[povCount] = (gr2_get_str(keypadButtons[i], &sda_sys_con))[0];
      if (povCount < sizeof(pinInputStr) - 1) {
        povCount++;
      }
      pinInputStr[povCount] = 0;
    }
  }

  gr2_clear_screen_ev(screen, &sda_sys_con);

  sda_screen_button_handler(screen, cancelButton, &sda_sys_con);
}

uint16_t pin_overlay_get_ok(uint16_t ovId) {
  if (povId != ovId || ovId == 0) {
    return 0;
  }

  if (povDone != 0) {
    return povDone;
  }

  return 0;
}

void pin_overlay_clear_ok(uint16_t ovId) {
  if (povId != ovId || ovId == 0) {
    return;
  }
  povDone = 0;
  povId = 0xFFFF;
}

void pin_overlay_destructor() {
  gr2_destroy(screen, &sda_sys_con);
  setRedrawFlag();
  overlayDestructorDone();
}

void pin_overlay_get_pin(uint8_t *outBuffer, size_t len) {
  if (povOnlyGet) {
    sda_strcp(pinInputStr, outBuffer, len);
  } else {
    outBuffer[0] = 0;
  }
}