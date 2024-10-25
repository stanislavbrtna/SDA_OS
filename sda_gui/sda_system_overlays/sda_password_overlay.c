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
static uint16_t setupButton;
static uint16_t povId;
static uint16_t povDone;
static uint8_t  kbdInit;

void password_overlay_destructor();

uint16_t password_overlay_init() {
  passInputStr[0] = 0;
  kbdInit = 0;

  uint8_t rInit = gr2_get_relative_init(&sda_sys_con);
  gr2_set_relative_init(1, &sda_sys_con);
   
  screen = gr2_add_screen(&sda_sys_con);
  gr2_set_y_cell(screen, 16, &sda_sys_con);
  gr2_set_yscroll(screen, -14, &sda_sys_con);

  gr2_add_text(1, 0, 7, 2, OVRL_ENTER_PASSWORD, screen, &sda_sys_con);

  passInput = gr2_add_text(1, 4, 6, 2, (uint8_t *)"", screen, &sda_sys_con);

  passMessage = gr2_add_text(1, 6, 7, 2, SCR_WRONG_PASSWORD, screen, &sda_sys_con);
  gr2_set_visible(passMessage, 0, &sda_sys_con);

  gr2_text_set_editable(passInput, 1, &sda_sys_con);
  gr2_text_set_pwd(passInput, 1, &sda_sys_con);

  passButton = gr2_add_button(7, 4, 1, 2, (uint8_t *)"*", screen, &sda_sys_con);

  setupButton = gr2_add_button(2, 7, 5, 2, OVRL_SETUP_PASSWORD, screen, &sda_sys_con);
  gr2_set_visible(setupButton, 0, &sda_sys_con);

  okButton = gr2_add_button(5, 10, 2, 2, OVRL_OK, screen, &sda_sys_con);
  cancelButton = gr2_add_button(2, 10, 2, 2, OVRL_CANCEL, screen, &sda_sys_con);

  gr2_text_set_align(okButton, GR2_ALIGN_CENTER, &sda_sys_con);
  gr2_text_set_align(cancelButton, GR2_ALIGN_CENTER, &sda_sys_con);

  povId = setOverlayScreen(screen, &sda_sys_con);

  gr2_set_xscroll(screen, 16, &sda_sys_con);

  setOverlayY2(288);
 
  gr2_activate_text(passInput, &sda_sys_con);

  if (svp_crypto_get_if_set_up() == 0) {
    gr2_text_deactivate(&sda_sys_con);
    kbdInit = 7;
    gr2_set_grayout(passInput, 1, &sda_sys_con);
    gr2_set_grayout(okButton, 1, &sda_sys_con);
    gr2_set_grayout(passButton, 1, &sda_sys_con);
    gr2_text_set_pwd(passInput, 0, &sda_sys_con);
    gr2_set_str(passInput, OVRL_SEC_NOT_SET_UP, &sda_sys_con);
    gr2_set_visible(setupButton, 1, &sda_sys_con);
  } else {
    gr2_set_y1(okButton, 8, &sda_sys_con);
    gr2_set_y1(cancelButton, 8, &sda_sys_con);
    setOverlayY2(288 - 28);
  }
  
  povDone = 0;

  gr2_set_relative_init(rInit, &sda_sys_con);

  setOverlayDestructor(password_overlay_destructor);

  return povId;
}

void password_overlay_update(uint16_t ovId) {

  if (povId != ovId || ovId == 0 || povId != getOverlayId()) {
    return;
  }

  if((povDone == 1) || (povDone == 2)) {
    return;
  }

  sda_keyboard_show();
  
  svp_input_handler(passInputStr, 32, passInput);

  if(sda_kbd_input_ret_detect(passInputStr, 32) == 1) {
    gr2_set_event(okButton, EV_RELEASED, &sda_sys_con);
  }

  if (gr2_clicked(passButton, &sda_sys_con)) {
    gr2_text_set_pwd(passInput, 1 - gr2_text_get_pwd(passInput, &sda_sys_con), &sda_sys_con);
  }

  if (gr2_clicked(setupButton, &sda_sys_con)) {
    sda_keyboard_hide();
    destroyOverlay();
    povDone = 2;
    sda_settings_open_security();
    return;
  }

  if (gr2_get_event(okButton, &sda_sys_con) == EV_RELEASED) {

    if (svp_crypto_unlock(passInputStr)) {
      gr2_set_visible(passMessage, 1, &sda_sys_con);
    } else {
      destroyOverlay();
      sda_keyboard_hide();
      povDone = 1;
      return;
    }
  }

  if (gr2_get_event(cancelButton, &sda_sys_con) == EV_RELEASED) {
    destroyOverlay();
    sda_keyboard_hide();
    povDone = 2;
    return;
  }
  gr2_clear_screen_ev(screen, &sda_sys_con);

  sda_screen_button_handler(screen, cancelButton, &sda_sys_con);
}


uint16_t password_overlay_get_ok(uint16_t ovId) {
  if (povId != ovId || ovId == 0 || povId != getOverlayId()) {
    return 0;
  }

  if (povDone != 0) {
    return povDone;
  }

  return 0;
}


void password_overlay_clear_ok(uint16_t ovId) {
  if (povId != ovId || povId != getOverlayId()) {
    return;
  }
  povDone = 0;
  povId = 0xFFFF;
}


void password_overlay_destructor() {
  gr2_destroy(screen, &sda_sys_con);
  sda_keyboard_hide();
  setRedrawFlag();
}