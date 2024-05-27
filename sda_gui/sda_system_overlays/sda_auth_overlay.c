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

#include "sda_system_overlays.h"

uint16_t auth_overlay_scr;
uint16_t auth_overlay_ok;
uint16_t auth_overlay_cancel;
uint16_t auth_overlay = 0;


void sda_show_auth_overlay_init() {
  gr2_set_relative_init(1, &sda_sys_con);
  auth_overlay_scr = gr2_add_screen(&sda_sys_con);

  gr2_set_x_cell(auth_overlay_scr, 16, &sda_sys_con);
  gr2_text_set_fit(
    gr2_add_text(
        1, 1, 15, 2,
        OVRL_GRANT_PERMISSIONS,
        auth_overlay_scr,
        &sda_sys_con
    ),
    1,
    &sda_sys_con
  );

  gr2_text_set_fit(
    gr2_add_text(
        1, 3, 15, 2,
        svmGetName(),
        auth_overlay_scr,
        &sda_sys_con
    ),
    1,
    &sda_sys_con
  );

  
  auth_overlay_ok = gr2_add_button(
                        4, 5, 4, 1,
                        OVRL_OK,
                        auth_overlay_scr,
                        &sda_sys_con
                    );
  
  auth_overlay_cancel = gr2_add_button(
                        10, 5, 4, 1,
                        OVRL_CANCEL,
                        auth_overlay_scr,
                        &sda_sys_con
                    );

  auth_overlay = setOverlayScreen(auth_overlay_scr, &sda_sys_con);
  
  gr2_set_relative_init(0, &sda_sys_con);

  setOverlayX1(16);
  setOverlayX2(304);
  setOverlayY2(280);
}

void sda_auth_overlay_handle() {
  if (auth_overlay != getOverlayId() || getOverlayId() == 0) {
    return;
  }

  if (gr2_clicked(auth_overlay_ok, &sda_sys_con)) {
    svmAuthorize();
    destroyOverlay();
    setRedrawFlag();
    return;
  }

  if (gr2_clicked(auth_overlay_cancel, &sda_sys_con)) {
    destroyOverlay();
    setRedrawFlag();
    return;
  }

  sda_screen_button_handler(auth_overlay_scr, auth_overlay_ok, &sda_sys_con);
}

uint8_t sda_auth_overlay_visible() {
  if (auth_overlay != getOverlayId() || getOverlayId() == 0) {
    return 0;
  }

  return 1;
}