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
  static uint16_t optSecuBack;
  static uint16_t optSecuOld;
  static uint16_t optSecuNew;

  static uint16_t optSecuOldBtn;
  static uint16_t optSecuNewBtn;
  static uint16_t resetBtn;

  static uint8_t optSecuNewStr[33];
  static uint8_t optSecuOldStr[33];
  static uint16_t optSecuOk;
  static uint16_t msgWrongPwd;
  static uint16_t msgPwdStored;
  static uint16_t msgKeyMismatch;
  static uint16_t optSecuLock;
  uint16_t optSecuScr;

  if (init == 1) {
    optSecuScr = gr2_add_screen(&sda_sys_con);

    gr2_add_text(1, 1, 10, 2, SCR_SECURITY_SCREEN, optSecuScr, &sda_sys_con);

    gr2_add_text(1, 3, 10, 4, SCR_OLD_PASSWORD, optSecuScr, &sda_sys_con);
    optSecuOld = gr2_add_text(1, 4, 8, 5, (uint8_t *)"", optSecuScr, &sda_sys_con);
    gr2_add_text(1, 5, 10, 6, SCR_NEW_PASSWORD, optSecuScr, &sda_sys_con);
    optSecuNew = gr2_add_text(1, 6, 8, 7, (uint8_t *)"", optSecuScr, &sda_sys_con);
    optSecuOldBtn = gr2_add_button(8, 4, 9, 5, (uint8_t *)"*", optSecuScr, &sda_sys_con);
    optSecuNewBtn = gr2_add_button(8, 6, 9, 7, (uint8_t *)"*", optSecuScr, &sda_sys_con);
    optSecuLock = gr2_add_button(2, 10, 8, 11, SCR_LOCK_DEVICE, optSecuScr, &sda_sys_con);
    optSecuOk = gr2_add_button(2, 8, 8, 9, SCR_CHANGE_PASSWORD, optSecuScr, &sda_sys_con);
    msgWrongPwd = gr2_add_text(1, 7, 9, 8, SCR_WRONG_PASSWORD, optSecuScr, &sda_sys_con);
    msgPwdStored = gr2_add_text(1, 7, 9, 8, SCR_PASSWORD_STORED, optSecuScr, &sda_sys_con);
    msgKeyMismatch = gr2_add_text(0, 7, 10, 8, SCR_KEY_MISMATCH, optSecuScr, &sda_sys_con);

    gr2_text_set_align(msgKeyMismatch, GR2_ALIGN_CENTER, &sda_sys_con);

    resetBtn = gr2_add_button(2, 9, 8, 10, SCR_RESET_KEY, optSecuScr, &sda_sys_con);
    gr2_text_set_align(resetBtn, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_text_set_align(optSecuLock, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_set_visible(resetBtn, 0, &sda_sys_con);

    gr2_text_set_align(msgWrongPwd, GR2_ALIGN_RIGHT, &sda_sys_con);
    gr2_text_set_align(msgPwdStored, GR2_ALIGN_RIGHT, &sda_sys_con);

    gr2_text_set_editable(optSecuNew, 1, &sda_sys_con);
    gr2_text_set_editable(optSecuOld, 1, &sda_sys_con);

    gr2_text_set_pwd(optSecuNew, 1, &sda_sys_con);
    gr2_text_set_pwd(optSecuOld, 1, &sda_sys_con);

    optSecuBack = gr2_add_button(1, 12, 4, 13, SCR_BACK, optSecuScr, &sda_sys_con);

    gr2_text_set_align(optSecuBack, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_text_set_align(optSecuOk, GR2_ALIGN_CENTER, &sda_sys_con);

    return optSecuScr;
  }

  if (init == 2) {
    optSecuNewStr[0] = 0;
    optSecuOldStr[0] = 0;
    gr2_set_visible(msgWrongPwd, 0, &sda_sys_con);
    gr2_set_visible(msgPwdStored, 0, &sda_sys_con);
    gr2_set_visible(msgKeyMismatch, 0, &sda_sys_con);

    if (svp_crypto_get_if_set_up() == 0) {
      gr2_set_grayout(optSecuOld, 1, &sda_sys_con);
      gr2_set_grayout(optSecuLock, 1, &sda_sys_con);
    } else {
      gr2_set_grayout(optSecuOld, 0, &sda_sys_con);
      gr2_set_grayout(optSecuLock, 0, &sda_sys_con);
    }

    return optSecuBack;
  }

  if (gr2_clicked(optSecuBack, &sda_sys_con)) {
    mainScr = slotScreen[2];
    sda_keyboard_hide();
    setRedrawFlag();
  }

  svp_input_handler(optSecuNewStr, 32, optSecuNew);

  svp_input_handler(optSecuOldStr, 32, optSecuOld);

  if (gr2_clicked(optSecuNewBtn, &sda_sys_con)) {
    gr2_text_set_pwd(optSecuNew, 1 - gr2_text_get_pwd(optSecuNew, &sda_sys_con), &sda_sys_con);
  }

  if (gr2_clicked(optSecuOldBtn, &sda_sys_con)) {
    gr2_text_set_pwd(optSecuOld, 1 - gr2_text_get_pwd(optSecuOld, &sda_sys_con), &sda_sys_con);
  }

  if (gr2_clicked(resetBtn, &sda_sys_con)) {
    svp_crypto_reset_os_keyfile();
    gr2_set_visible(resetBtn, 0, &sda_sys_con);
    gr2_set_visible(msgKeyMismatch, 0, &sda_sys_con);
  }

  if (gr2_clicked(optSecuLock, &sda_sys_con)) {
    svpSGlobal.sdaDeviceLock = DEVICE_LOCKED;
    sda_slot_on_top(SDA_SLOT_HOMESCREEN);
    rtc_write_locked(1);
  }

  if (gr2_clicked(optSecuOk, &sda_sys_con) && svp_strcmp(optSecuNewStr, "") == 0) {
    uint8_t retval;
    if (svp_crypto_get_if_set_up() == 0) {
      retval = svp_crypto_unlock((uint8_t *)"def");
    } else {
      retval = svp_crypto_unlock(optSecuOldStr);
    }
    if (retval != 0) {
      if (retval == 2) {
        gr2_set_visible(msgWrongPwd, 1, &sda_sys_con);
        gr2_set_visible(msgPwdStored, 0, &sda_sys_con);
        gr2_set_visible(msgKeyMismatch, 0, &sda_sys_con);
      } else if(retval == 3) {
        gr2_set_grayout(optSecuOk, 1, &sda_sys_con);
        gr2_set_grayout(optSecuNew, 1, &sda_sys_con);
        gr2_set_grayout(optSecuOld, 1, &sda_sys_con);
      }
    } else {
      gr2_set_grayout(optSecuOld, 0, &sda_sys_con);
      gr2_set_grayout(optSecuLock, 0, &sda_sys_con);
      gr2_set_visible(msgPwdStored, 1, &sda_sys_con);
      gr2_set_visible(msgWrongPwd, 0, &sda_sys_con);
      if (svp_crypto_get_if_set_up()){
        svp_crypto_change_password(optSecuNewStr);
        svp_crypto_reencrypt_os_keyfile(optSecuOldStr, optSecuNewStr);
      } else {
        svp_crypto_change_password(optSecuNewStr);
      }    
      if (sda_crypto_keyfile_init_check() != 0) {
        sda_show_error_message(SCR_KEY_ERROR_MSG);
        gr2_set_visible(msgKeyMismatch, 1, &sda_sys_con);
        gr2_set_visible(resetBtn, 1, &sda_sys_con);
      }
      svp_crypto_lock();
    }
    optSecuNewStr[0] = 0;
    optSecuOldStr[0] = 0;

    gr2_set_modified(optSecuNew, &sda_sys_con);
    gr2_set_modified(optSecuOld, &sda_sys_con);
    sda_keyboard_hide();
  }
  return 0;
}
