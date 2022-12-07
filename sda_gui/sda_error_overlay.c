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

// error overlays
uint16_t error_overlay_scr;
uint16_t error_overlay_ok;
uint16_t error_overlay = 0xFFFF;

extern uint8_t soft_error_flag;
extern uint8_t mainDir[258];

void sda_error_overlay_destructor() {
  setRedrawFlag();
  soft_error_flag = 0;
  error_overlay = 0xFFFF;
}


void svp_errSoftPrint(svsVM *s) {
  if (errCheck(s)) {
    sda_show_error_message((uint8_t *)s->errString);
  }
  svp_chdir(mainDir);
  svp_chdir((uint8_t *)"APPS");
  errSoftPrint(s);
  printf("\n");
}


void sda_show_error_message(uint8_t * text) {
  error_overlay_scr = gr2_add_screen(&sda_sys_con);
  gr2_set_x_cell(error_overlay_scr, 16, &sda_sys_con);
  gr2_add_text(
      2, 1, 14, 2,
      MSG_ERROR_OCCURED,
      error_overlay_scr,
      &sda_sys_con
  );
  gr2_text_set_fit(
      gr2_add_text(1, 2, 15, 7, text, error_overlay_scr, &sda_sys_con),
      1,
      &sda_sys_con
      );
  error_overlay_ok
    = gr2_add_button(6, 8, 10, 9, OVRL_OK, error_overlay_scr, &sda_sys_con);
  error_overlay = setOverlayScreen(error_overlay_scr, &sda_sys_con);
  setOverlayDestructor(sda_error_overlay_destructor);
  soft_error_flag = 1;
  printf("sda_show_error_message: %s\n", text);
}

void sda_error_overlay_handle() {
  if (error_overlay != getOverlayId()) {
    return;
  }

  if (gr2_clicked(error_overlay_ok, &sda_sys_con)) {
    destroyOverlay();
    return;
  }
  sda_screen_button_handler(error_overlay_scr, error_overlay_ok, &sda_sys_con);
}