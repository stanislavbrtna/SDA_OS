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
static uint16_t passInput;
static uint8_t  passInputStr[33];
static uint16_t passButton;
static uint16_t passMessage;
static uint16_t okButton;
static uint16_t cancelButton;
static uint16_t povId;
static uint16_t povDone;
static uint8_t  kbdInit;


uint16_t password_overlay_init() {
  passInputStr[0] = 0;
  kbdInit = 0;
  
  screen = pscg_add_screen(&sda_sys_con);

  pscg_add_text(1, 0, 8, 1, OVRL_ENTER_PASSWORD, screen, &sda_sys_con);

  passInput = pscg_add_text(1, 2, 7, 3, (uint8_t *)"", screen, &sda_sys_con);

  passMessage = pscg_add_text(1, 3, 8, 4, SCR_WRONG_PASSWORD, screen, &sda_sys_con);
  pscg_set_visible(passMessage, 0, &sda_sys_con);

  pscg_text_set_editable(passInput, 1, &sda_sys_con);
  pscg_text_set_pwd(passInput, 1, &sda_sys_con);

  passButton = pscg_add_button(7, 2, 8, 3, (uint8_t *)"*", screen, &sda_sys_con);

  okButton = pscg_add_button(5, 5, 7, 6, OVRL_OK, screen, &sda_sys_con);
  cancelButton = pscg_add_button(2, 5, 4, 6, OVRL_CANCEL, screen, &sda_sys_con);

  pscg_text_set_align(okButton, GR2_ALIGN_CENTER, &sda_sys_con);
  pscg_text_set_align(cancelButton, GR2_ALIGN_CENTER, &sda_sys_con);

  povId = setOverlayScreen(screen, &sda_sys_con);

  pscg_set_xscroll(screen, 16, &sda_sys_con);

  setOverlayY2(288);
  setOverlayDestructor(time_overlay_destructor);

  pscg_activate_text(passInput, &sda_sys_con);
  
  povDone = 0;

  return povId;
}

void password_overlay_update(uint16_t ovId) {

  if (povId != ovId) {
    return;
  }

  if((povDone == 1) || (povDone == 2)) {
    return;
  }

  // TODO: fix this, displaying overlay and keyboard at the same cycle somewhat does not work.
  if (kbdInit < 5) {
    kbdInit++;
  } else if (kbdInit == 5) {
    showKeyboard();
    kbdInit++;
  }

  svp_input_handler(passInputStr, 32, passInput);

  if (gr2_clicked(passButton, &sda_sys_con)) {
    pscg_text_set_pwd(passInput, 1 - pscg_text_get_pwd(passInput, &sda_sys_con), &sda_sys_con);
  }

  if (pscg_get_event(okButton, &sda_sys_con) == EV_RELEASED) {

    if (svp_crypto_unlock(passInputStr)) {
      pscg_set_visible(passMessage, 1, &sda_sys_con);
    } else {
      destroyOverlay();
      hideKeyboard();
      povDone = 1;
      return;
    }
  }

  if (pscg_get_event(cancelButton, &sda_sys_con) == EV_RELEASED) {
    destroyOverlay();
    hideKeyboard();
    povDone = 2;
    return;
  }
  pscg_clear_screen_ev(screen, &sda_sys_con);

}


uint16_t password_overlay_get_ok(uint16_t ovId) {
  if (povId != ovId) {
    return 0;
  }
  if (povDone == 1) {
    return 1;
  } else {
    return 0;
  }
}


void password_overlay_clear_ok(uint16_t ovId) {
  if (povId != ovId) {
    return;
  }
  povDone = 0;
  povId = 0xFFFF;
}