/*
Copyright (c) 2020 Stanislav Brtna

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

uint16_t sda_settings_notif_screen(uint8_t init) {

  // sound & notification settings
  static uint16_t optSoundMute;
  static uint16_t optSoundLoud;
  static uint16_t optSoundBack;
  uint16_t optNotifyScr;

  if (init == 1){
    // notifiacations screen
    optNotifyScr = pscg_add_screen(&sda_sys_con);
    pscg_add_text(1, 1, 10, 2, SCR_NOTIFICATIONS, optNotifyScr, &sda_sys_con);
    optSoundMute = pscg_add_checkbox(1, 2, 7, 3, SCR_SOUND_OFF, optNotifyScr, &sda_sys_con);

    //pscg_add_text(1, 4, 10, 5, SCR_LED_NOTIFICATIONS, optNotifyScr);

    optSoundBack = pscg_add_button(1, 10, 4, 11, SCR_BACK, optNotifyScr, &sda_sys_con);
    pscg_text_set_align(optSoundBack, GR2_ALIGN_CENTER, &sda_sys_con);

    pscg_set_value(optSoundMute, svpSGlobal.mute, &sda_sys_con);

    return optNotifyScr;
  }

  if (init == 2) {
    if (svpSGlobal.mute) {
      pscg_set_value(optSoundMute, 1, &sda_sys_con);
    } else {
      pscg_set_value(optSoundMute, 0, &sda_sys_con);
    }
  }

  if (gr2_clicked(optSoundMute, &sda_sys_con)) {
    svpSGlobal.mute = 1 - svpSGlobal.mute;
    pscg_set_value(optSoundMute, svpSGlobal.mute, &sda_sys_con);
    sda_store_mute_config();
  }

  if (gr2_clicked(optSoundBack, &sda_sys_con)) {
    mainScr = slotScreen[2];
    setRedrawFlag();
  }

  return 0;
}