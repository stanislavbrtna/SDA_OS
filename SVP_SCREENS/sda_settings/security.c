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

#include "settings.h"

uint16_t sda_settings_security_screen(uint8_t init) {
  static uint16_t optSecuOld;
  static uint16_t optSecuNew;

  static uint16_t optSecuOldBtn;
  static uint16_t optSecuNewBtn;
  static uint16_t resetBtn;

  static uint8_t optSecuNewStr[33];
  static uint8_t optSecuOldStr[33];
  static uint8_t unlockCountStr[4];
  static uint16_t optSecuOk;
  static uint16_t msgWrongPwd;
  static uint16_t msgPwdStored;
  static uint16_t msgKeyMismatch;
  static uint16_t optSecuSetPin;
  static uint16_t optSecuSetDist;
  static uint16_t unlockWithPin;
  static uint16_t unlockCountClear;
  static uint16_t unlockCount;
  static uint16_t pinOverlay;
  static uint16_t distOverlay;
  uint16_t optSecuScr;

  if (init == 1) {
    optSecuScr = gr2_add_screen(&sda_sys_con);

    uint8_t relInit = gr2_get_relative_init(&sda_sys_con);

    gr2_set_relative_init(1, &sda_sys_con);

    gr2_set_cell_space_bottom(optSecuScr, 4, &sda_sys_con);
    gr2_set_cell_space_left(optSecuScr, 4, &sda_sys_con);
    gr2_set_y_cell(optSecuScr, 36, &sda_sys_con);
    gr2_set_x_cell(optSecuScr, 36, &sda_sys_con);

    // Todo: why this offset? this is weird...
    gr2_set_y_offset(optSecuScr, -96, &sda_sys_con);
    gr2_set_x_offset(optSecuScr, -24, &sda_sys_con);

    gr2_add_text(1, 3, 10, 1, SCR_OLD_PASSWORD, optSecuScr, &sda_sys_con);
    optSecuOld = gr2_add_text(1, 4, 7, 1, (uint8_t *)"", optSecuScr, &sda_sys_con);
    gr2_add_text(1, 5, 8, 1, SCR_NEW_PASSWORD, optSecuScr, &sda_sys_con);
    optSecuNew = gr2_add_text(1, 6, 7, 1, (uint8_t *)"", optSecuScr, &sda_sys_con);
    optSecuOldBtn = gr2_add_button(8, 4, 1, 1, (uint8_t *)"*", optSecuScr, &sda_sys_con);
    optSecuNewBtn = gr2_add_button(8, 6, 1, 1, (uint8_t *)"*", optSecuScr, &sda_sys_con);

    optSecuOk = gr2_add_button(4, 7, 5, 1, SCR_CHANGE_PASSWORD, optSecuScr, &sda_sys_con);

    // Todo: fix these messages
    msgWrongPwd = gr2_add_text(1, 8, 10, 1, SCR_WRONG_PASSWORD, optSecuScr, &sda_sys_con);
    msgPwdStored = gr2_add_text(1, 8, 10, 1, SCR_PASSWORD_STORED, optSecuScr, &sda_sys_con);
    msgKeyMismatch = gr2_add_text(0, 8, 10, 1, SCR_KEY_MISMATCH, optSecuScr, &sda_sys_con);

    gr2_text_set_align(msgKeyMismatch, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_text_set_align(msgWrongPwd, GR2_ALIGN_RIGHT, &sda_sys_con);

    gr2_add_text(1, 9, 6, 1, SCR_SECU_PIN_SETUP, optSecuScr, &sda_sys_con);
    optSecuSetPin = gr2_add_button(1, 10, 4, 1, SCR_SECU_SET_PIN, optSecuScr, &sda_sys_con);
    optSecuSetDist = gr2_add_button(5, 10, 4, 1, SCR_SECU_SET_DISTRESS, optSecuScr, &sda_sys_con);

    unlockWithPin = gr2_add_checkbox(1, 11, 9, 1, SCR_SECU_PIN_UNLOCK, optSecuScr, &sda_sys_con);

    if (svpSGlobal.usePinForLock) {
      gr2_set_value(unlockWithPin, 1, &sda_sys_con);
    }

    unlockCountStr[0] = 0;

    sda_int_to_str(unlockCountStr, svpSGlobal.unlockCounter, sizeof(unlockCountStr));

    unlockCount = gr2_add_text(0, 12, 2, 1, unlockCountStr, optSecuScr, &sda_sys_con);

    gr2_add_text(2, 12, 5, 1, SCR_KEY_UNLOCK_COUNT , optSecuScr, &sda_sys_con);
    gr2_text_set_align(unlockCount, GR2_ALIGN_RIGHT, &sda_sys_con);
    unlockCountClear = gr2_add_button(7, 12, 2, 1, SCR_SECU_CLEAR, optSecuScr, &sda_sys_con);

    resetBtn = gr2_add_button(1, 13, 5, 1, SCR_RESET_KEY, optSecuScr, &sda_sys_con);

    gr2_text_set_align(resetBtn, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_text_set_align(optSecuSetPin, GR2_ALIGN_CENTER, &sda_sys_con);

    gr2_text_set_editable(optSecuNew, 1, &sda_sys_con);
    gr2_text_set_editable(optSecuOld, 1, &sda_sys_con);

    gr2_text_set_pwd(optSecuNew, 1, &sda_sys_con);
    gr2_text_set_pwd(optSecuOld, 1, &sda_sys_con);

    gr2_text_set_align(optSecuOk, GR2_ALIGN_CENTER, &sda_sys_con);

    gr2_set_relative_init(relInit, &sda_sys_con);

    return optSecuScr;
  }

  if (init == 2) {
    optSecuNewStr[0] = 0;
    optSecuOldStr[0] = 0;
    gr2_set_visible(msgWrongPwd, 0, &sda_sys_con);
    gr2_set_visible(msgPwdStored, 0, &sda_sys_con);
    gr2_set_visible(msgKeyMismatch, 0, &sda_sys_con);

    sda_int_to_str(unlockCountStr, svpSGlobal.unlockCounter, sizeof(unlockCountStr));

    if (svpSGlobal.usePinForLock) {
      gr2_set_value(unlockWithPin, 1, &sda_sys_con);
    }

    if (sda_crypto_get_if_set_up() == 0) {
      gr2_set_grayout(optSecuOld, 1, &sda_sys_con);
      gr2_set_grayout(optSecuSetPin, 1, &sda_sys_con);
      gr2_set_grayout(optSecuSetDist, 1, &sda_sys_con);
      gr2_set_grayout(unlockWithPin, 1, &sda_sys_con);
      gr2_set_grayout(unlockCountClear, 1, &sda_sys_con);
      gr2_set_grayout(resetBtn, 1, &sda_sys_con);
    } else {
      gr2_set_grayout(optSecuOld, 0, &sda_sys_con);
      gr2_set_grayout(optSecuSetPin, 0, &sda_sys_con);

      if(sda_crypto_get_if_pin_set_up()) {
        gr2_set_grayout(optSecuSetDist, 0, &sda_sys_con);
        gr2_set_grayout(unlockWithPin, 0, &sda_sys_con);
      } else {
        gr2_set_grayout(optSecuSetDist, 1, &sda_sys_con);
        gr2_set_grayout(unlockWithPin, 1, &sda_sys_con);
      }
      
      gr2_set_grayout(unlockCountClear, 0, &sda_sys_con);
      gr2_set_grayout(resetBtn, 0, &sda_sys_con);
    }

    return 0;
  }

  svp_input_handler(optSecuNewStr, 32, optSecuNew);

  if (sda_kbd_input_ret_detect(optSecuNewStr, 32)) {
    gr2_set_event(optSecuOk, EV_RELEASED, &sda_sys_con);
  }

  svp_input_handler(optSecuOldStr, 32, optSecuOld);

  if (sda_kbd_input_ret_detect(optSecuOldStr, 32)) {
    gr2_activate_text(optSecuNew, &sda_sys_con);
  }

  if (gr2_clicked(optSecuNewBtn, &sda_sys_con)) {
    gr2_text_set_pwd(optSecuNew, 1 - gr2_text_get_pwd(optSecuNew, &sda_sys_con), &sda_sys_con);
  }

  if (gr2_clicked(optSecuOldBtn, &sda_sys_con)) {
    gr2_text_set_pwd(optSecuOld, 1 - gr2_text_get_pwd(optSecuOld, &sda_sys_con), &sda_sys_con);
  }

  if (gr2_clicked(resetBtn, &sda_sys_con)) {
    sda_crypto_remove();
    sda_settings_security_screen(2);
  }

  if (gr2_clicked(optSecuSetPin, &sda_sys_con)) {
    pinOverlay = pin_overlay_init(1);
  }

  pin_overlay_update(pinOverlay);

  if (pin_overlay_get_ok(pinOverlay) == 3) {
    uint8_t buff[33];
    pin_overlay_get_pin(buff, sizeof(buff));
    sda_crypto_change_pin(buff);
    pin_overlay_clear_ok(pinOverlay);
    sda_settings_security_screen(2);
  } else if (pin_overlay_get_ok(pinOverlay)) {
    pin_overlay_clear_ok(pinOverlay);
  }

  if (gr2_clicked(optSecuSetDist, &sda_sys_con)) {
    distOverlay = pin_overlay_init(1);
  }

  pin_overlay_update(distOverlay);

  if (pin_overlay_get_ok(distOverlay) == 3) {
    uint8_t buff[33];
    pin_overlay_get_pin(buff, sizeof(buff));
    sda_crypto_change_distress(buff);
    pin_overlay_clear_ok(distOverlay);
  } else if (pin_overlay_get_ok(distOverlay)) {
    pin_overlay_clear_ok(distOverlay);
  }

  if (gr2_clicked(unlockWithPin, &sda_sys_con)) {
    svpSGlobal.usePinForLock = gr2_get_value(unlockWithPin, &sda_sys_con);
    sda_crypto_store_conf();
  }

  if (gr2_clicked(unlockCountClear, &sda_sys_con)) {
    svpSGlobal.unlockCounter = 0;
    sda_int_to_str(unlockCountStr, svpSGlobal.unlockCounter, sizeof(unlockCountStr));
    gr2_set_modified(unlockCount, &sda_sys_con);
  }

  if (gr2_clicked(optSecuOk, &sda_sys_con) && svp_strcmp(optSecuNewStr, (uint8_t *)"") == 0) {

    if (!sda_crypto_get_if_set_up()) {
      sda_crypto_reset(optSecuNewStr);
      sda_settings_security_screen(2);
      gr2_set_visible(msgPwdStored, 1, &sda_sys_con);
    } else {
      uint8_t retval = sda_crypto_unlock(optSecuOldStr);

      if (retval == 0) {
        sda_crypto_change_password(optSecuNewStr);
        sda_crypto_lock();
        sda_homescreen_lock_en();
      } else {
        gr2_set_visible(msgWrongPwd, 1, &sda_sys_con);
        gr2_set_visible(msgPwdStored, 0, &sda_sys_con);
        gr2_set_visible(msgKeyMismatch, 0, &sda_sys_con);
        gr2_set_visible(resetBtn, 1, &sda_sys_con);
      }
    }
    optSecuNewStr[0] = 0;
    optSecuOldStr[0] = 0;

    gr2_set_modified(optSecuNew, &sda_sys_con);
    gr2_set_modified(optSecuOld, &sda_sys_con);
    sda_keyboard_hide();
  }
  return 0;
}
