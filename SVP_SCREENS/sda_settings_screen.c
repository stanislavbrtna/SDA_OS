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
#include "sda_settings/settings.h"

void svp_settings_set_spacing(uint16_t id) {
  gr2_set_param(id, SDA_SETTINGS_SPACER, &sda_sys_con);
}

uint16_t svp_optScreen(uint8_t init, uint8_t top) {
  static uint16_t optScreen;

  static uint16_t optTimeScr;
  static uint16_t optLcdScr;
  static uint16_t optColScr;
  static uint16_t optInfoScr;
  static uint16_t optNotifyScr;
  static uint16_t optSecuScr;
  static uint16_t optDbgScr;

  static uint16_t optTimSel;
  static uint16_t optLcdSel;
  static uint16_t optDbgSel;
  static uint16_t optInfoSel;

  static uint16_t optSecuSel;

  static uint16_t optMntSel;
  static uint16_t optSound;



  //mount
  static uint8_t sd_mounted;
  static uint16_t prac_screen;

  if (init == 1) {

    sd_mounted = 1; //probably

    optScreen = gr2_add_screen(&sda_sys_con);

    // sub-screens
    optLcdScr = sda_settings_display_screen(1);
    optInfoScr = sda_settings_info_screen(1);
    optNotifyScr = sda_settings_notif_screen(1);
    optSecuScr = sda_settings_security_screen(1);
    optDbgScr = sda_settings_debug_screen(1);
    optTimeScr = sda_settings_time_screen(1);

    // options screen init
    gr2_add_text(1, 1, 10, 2, SCR_SETTINGS, optScreen, &sda_sys_con);

    optTimSel = gr2_add_button(1, 2, 9, 3, SCR_SET_DATE_TIME, optScreen, &sda_sys_con);
    optLcdSel = gr2_add_button(1, 3, 9, 4, SCR_SET_DISPLAY, optScreen, &sda_sys_con);
    optSound = gr2_add_button(1, 4, 9, 5, SCR_SETTINGS_SND, optScreen, &sda_sys_con);
    optSecuSel = gr2_add_button(1, 5, 9, 6, SCR_SET_SECU, optScreen, &sda_sys_con);
    optMntSel = gr2_add_button(1, 8, 9, 9, SD_UMOUNT, optScreen, &sda_sys_con);
    optInfoSel = gr2_add_button(1, 10, 9, 11, SCR_ABOUT_SYSTEM, optScreen, &sda_sys_con);

    svp_settings_set_spacing(optTimSel);
    svp_settings_set_spacing(optLcdSel);
    svp_settings_set_spacing(optSound);
    svp_settings_set_spacing(optSecuSel);
    svp_settings_set_spacing(optMntSel);
    svp_settings_set_spacing(optInfoSel);

    optDbgSel = gr2_add_button(1, 6, 9, 7, (uint8_t *)"Debug", optScreen, &sda_sys_con);

    svp_settings_set_spacing(optDbgSel);


    return optScreen;
  }

  //loop top
  if (top == 1) {

    if (gr2_clicked(optTimSel, &sda_sys_con)) {
      mainScr = optTimeScr;
      setRedrawFlag();
    }

    if (gr2_clicked(optLcdSel, &sda_sys_con)) {
      mainScr = optLcdScr;
      sda_settings_display_screen(2);
      setRedrawFlag();
    }

    if (gr2_clicked(optInfoSel, &sda_sys_con)) {
      mainScr = optInfoScr;
      sda_settings_info_screen(2); // reset position of the internal screen
      setRedrawFlag();
    }

    if (gr2_clicked(optDbgSel, &sda_sys_con)) {
      mainScr = optDbgScr;
      sda_settings_debug_screen(2); // reload config of dbg serial
      setRedrawFlag();
    }

    if (gr2_clicked(optSecuSel, &sda_sys_con)) {
      mainScr = optSecuScr;
      sda_settings_security_screen(2);
      setRedrawFlag();
    }

    if (gr2_clicked(optSound, &sda_sys_con)) {
      sda_settings_notif_screen(2); // reload current settings
      mainScr = optNotifyScr;
      setRedrawFlag();
    }

    /*Card unmounting and apps reloader*/
    if (gr2_clicked(optMntSel, &sda_sys_con)) {
      if (sd_mounted == 1) {
        svmCloseAll();
        sda_slot_on_top(2);
        svp_umount();
        sd_mounted = 0;
        gr2_set_str(optMntSel, SCR_SD_MOUNT, &sda_sys_con);
        prac_screen = slotScreen[1];
        slotScreen[1] = gr2_add_screen(&sda_sys_con);
        gr2_add_text(1, 1, 10, 2, SCR_SD_NOT_PRESENT_WARNING, slotScreen[1], &sda_sys_con);
      } else {
        if(svp_mount()) {
          sda_show_error_message(SCR_CARD_ERROR_MSG);
        } else {
          gr2_set_str(optMntSel, SD_UMOUNT, &sda_sys_con);
          // little hack for reload of app screen
          gr2_destroy(slotScreen[1], &sda_sys_con);
          gr2_destroy(prac_screen, &sda_sys_con);
          slotScreen[1] = svp_appScreen(1, 0);
          sd_mounted = 1;
          sda_slot_on_top(2);
        }
      }
    }

    sda_settings_debug_screen(0);
    sda_settings_time_screen(0);
    sda_settings_display_screen(0);
    sda_settings_color_screen(0);
    sda_settings_info_screen(0);
    sda_settings_notif_screen(0);
    sda_settings_security_screen(0);
  }
  return 0;
}

