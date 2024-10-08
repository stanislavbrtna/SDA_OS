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
  uint16_t optNotifyScr;

  if (init == 1){
    // notifiacations screen
    optNotifyScr = gr2_add_screen(&sda_sys_con);
    optSoundMute = gr2_add_checkbox(1, 1, 7, 2, SCR_SOUND_OFF, optNotifyScr, &sda_sys_con);

    gr2_set_yscroll(optNotifyScr, 16, &sda_sys_con);
    gr2_set_value(optSoundMute, svpSGlobal.mute, &sda_sys_con);

    return optNotifyScr;
  }

  if (init == 2) {
    if (svpSGlobal.mute) {
      gr2_set_value(optSoundMute, 1, &sda_sys_con);
    } else {
      gr2_set_value(optSoundMute, 0, &sda_sys_con);
    }
    return 0;
  }

  if (gr2_clicked(optSoundMute, &sda_sys_con)) {
    sda_set_mute((uint8_t) gr2_get_value(optSoundMute, &sda_sys_con));
  }

  return 0;
}
