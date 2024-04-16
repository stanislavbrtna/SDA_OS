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

uint16_t prac_screen;
uint16_t optMntSel;

void settings_sd_umount();
void settings_sd_mount();

void svp_settings_set_spacing(uint16_t id) {
  gr2_set_param(id, SDA_SETTINGS_SPACER, &sda_sys_con);
}

static uint16_t settingsScreen;
static uint16_t settingsTitle;
static uint16_t settingsFrame;
static uint16_t settingsMenu;

static uint16_t settingsPrev;
static uint16_t settingsStack[5];
static uint8_t  *settingsStackStr[5];

static uint16_t globBack;
static uint16_t optSecuScr;


uint16_t sda_settings_menu(uint8_t init);

void sda_settings_stack_add(uint16_t scr, uint8_t * label) {
  settingsPrev++;
  settingsStack[settingsPrev] = scr;
  settingsStackStr[settingsPrev] = label;
  gr2_set_str(settingsTitle, label, &sda_sys_con);

  gr2_set_value(settingsFrame, scr, &sda_sys_con);
}


void sda_settings_open() {
  sda_slot_on_top(SDA_SLOT_SETTINGS);
  settingsPrev = 0;
  sda_settings_stack_add(settingsMenu, SCR_SETTINGS);
}


uint16_t sda_settings_gui(uint8_t init, uint8_t top) {
  static uint8_t sd_inserted_pre;
  static uint16_t btnBack;

  if (init) {
    settingsScreen = gr2_add_screen(&sda_sys_con);
    settingsTitle  = gr2_add_text(2, 0, 10, 1, SCR_SETTINGS, settingsScreen, &sda_sys_con);
    settingsMenu   = sda_settings_menu(1);
    settingsFrame  = gr2_add_frame(0, 1, 10, 14, settingsMenu, settingsScreen, &sda_sys_con);

    btnBack = gr2_add_button(0, 0, 2, 1, "<-", settingsScreen, &sda_sys_con);

    gr2_text_set_align(btnBack, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_set_ghost(btnBack, 1, &sda_sys_con);

    sd_inserted_pre = 1;
    settingsPrev = 0;

    sda_settings_stack_add(settingsMenu, SCR_SETTINGS);

    return settingsScreen;
  }

  //loop top
  if (top == 1) {
    sda_settings_menu(0);

    if (gr2_clicked(btnBack, &sda_sys_con)) {
      if (settingsPrev == 1) {
        sda_slot_on_top(SDA_SLOT_APPLIST);
        return;
      }
      settingsPrev--;
      gr2_set_value(settingsFrame, settingsStack[settingsPrev], &sda_sys_con);
      gr2_set_str(settingsTitle, settingsStackStr[settingsPrev], &sda_sys_con);

      sda_keyboard_hide();
    }

    sda_screen_button_handler(mainScr, btnBack, &sda_sys_con);
  }

  if (sda_card_inserted() != sd_inserted_pre) {
    gr2_set_grayout(optMntSel, 1 - sda_card_inserted(), &sda_sys_con);

    if (sda_card_inserted() == 0) {
      settings_sd_umount();
    }

    if (sda_card_inserted() == 1) {
      settings_sd_mount();
    }

    sd_inserted_pre = sda_card_inserted();
  }

  return 0;
}


void settings_sd_umount() {
  if (svpSGlobal.sdaDeviceLock == DEVICE_UNLOCKED) {
    sda_slot_on_top(SDA_SLOT_SETTINGS);
  }
  svp_umount();
  gr2_set_str(optMntSel, SCR_SD_MOUNT, &sda_sys_con);
  prac_screen = slotScreen[1];
  slotScreen[1] = gr2_add_screen(&sda_sys_con);
  gr2_add_text(1, 1, 10, 2, SCR_SD_NOT_PRESENT_WARNING, slotScreen[1], &sda_sys_con);
}


void settings_sd_mount() {
  if(svp_mount()) {
    sda_show_error_message(SCR_CARD_ERROR_MSG);
  } else {
    gr2_set_str(optMntSel, SD_UMOUNT, &sda_sys_con);
    // little hack for reload of app screen
    gr2_destroy(slotScreen[1], &sda_sys_con);
    gr2_destroy(prac_screen, &sda_sys_con);
    slotScreen[1] = svp_appScreen(1, 0);

    if (svpSGlobal.sdaDeviceLock == DEVICE_UNLOCKED) {
      sda_slot_on_top(SDA_SLOT_SETTINGS);
    }

    //next little hack, relod the main screen, this will be replaced someday
    sda_homescreen_configure();
  }
}


void sda_settings_open_security() {
  svpSGlobal.systemXBtnVisible = 0;
  sda_settings_open();
  sda_settings_stack_add(optSecuScr, SCR_SECURITY_SCREEN);
  sda_settings_security_screen(2);
  setRedrawFlag();
}


uint16_t sda_settings_menu(uint8_t init) {

  static uint16_t optScreen;

  static uint16_t optTimeScr;
  static uint16_t optLcdScr;
  static uint16_t optInfoScr;
  static uint16_t optNotifyScr;
  static uint16_t optDbgScr;
  static uint16_t optTimSel;
  static uint16_t optLcdSel;
  static uint16_t optDbgSel;
  static uint16_t optInfoSel;
  static uint16_t optSecuSel;
  static uint16_t optSound;


  if (init == 1) {
    optScreen = gr2_add_screen(&sda_sys_con);

    gr2_set_yscroll(optScreen, 16, &sda_sys_con);

    // sub-screens
    optLcdScr = sda_settings_display_screen(1);
    optInfoScr = sda_settings_info_screen(1);
    optNotifyScr = sda_settings_notif_screen(1);
    optSecuScr = sda_settings_security_screen(1);
    optDbgScr = sda_settings_debug_screen(1);
    optTimeScr = sda_settings_time_screen(1);

    // options screen init
    uint8_t ri = gr2_get_relative_init(&sda_sys_con);
    gr2_set_relative_init(1, &sda_sys_con);

    optTimSel  = gr2_add_button(1, 1, 7, 1, SCR_SET_DATE_TIME, optScreen, &sda_sys_con);
    optLcdSel  = gr2_add_button(1, 2, 7, 1, SCR_SET_DISPLAY, optScreen, &sda_sys_con);
    optSound   = gr2_add_button(1, 3, 7, 1, SCR_SETTINGS_SND, optScreen, &sda_sys_con);
    optSecuSel = gr2_add_button(1, 4, 7, 1, SCR_SET_SECU, optScreen, &sda_sys_con);
    optDbgSel  = gr2_add_button(1, 5, 7, 1, (uint8_t *)"Debug", optScreen, &sda_sys_con);
    optMntSel  = gr2_add_button(1, 7, 7, 1, SD_UMOUNT, optScreen, &sda_sys_con);
    optInfoSel = gr2_add_button(1, 9, 7, 1, SCR_ABOUT_SYSTEM, optScreen, &sda_sys_con);

    svp_settings_set_spacing(optTimSel);
    svp_settings_set_spacing(optLcdSel);
    svp_settings_set_spacing(optSound);
    svp_settings_set_spacing(optSecuSel);
    svp_settings_set_spacing(optMntSel);
    svp_settings_set_spacing(optInfoSel);
    svp_settings_set_spacing(optDbgSel);

    gr2_set_relative_init(ri, &sda_sys_con);

    return optScreen;
  }

  if (gr2_clicked(optTimSel, &sda_sys_con)) {
    sda_settings_stack_add(optTimeScr, SCR_SET_TIME_SCR);
  }

  if (gr2_clicked(optLcdSel, &sda_sys_con)) {
    sda_settings_display_screen(2); // reload current settings
    sda_settings_stack_add(optLcdScr, SCR_DISPLAY_SETTINGS_SCREEN);
  }

  if (gr2_clicked(optSound, &sda_sys_con)) {
    sda_settings_notif_screen(2); 
    sda_settings_stack_add(optNotifyScr, SCR_NOTIFICATIONS);
  }

  if (gr2_clicked(optSecuSel, &sda_sys_con)) {
    sda_settings_stack_add(optSecuScr, SCR_SECURITY_SCREEN);
    sda_settings_security_screen(2);
  }

  if (gr2_clicked(optDbgSel, &sda_sys_con)) {
    sda_settings_stack_add(optDbgScr, SCR_SYSTEM_OPTIONS);
    sda_settings_debug_screen(2);
  }

  // Card unmounting and apps reloader
  if (gr2_clicked(optMntSel, &sda_sys_con)) {
    if (svp_getMounted() == 1) {
      svmCloseAll();
      settings_sd_umount();
    } else {
      settings_sd_mount();
    }
  }

  if (gr2_clicked(optInfoSel, &sda_sys_con)) {
    sda_settings_stack_add(optInfoScr, SCR_ABOUT_SYSTEM);
    sda_settings_info_screen(2);
  }

  sda_settings_debug_screen(0);
  sda_settings_time_screen(0);
  sda_settings_display_screen(0);
  sda_settings_color_screen(0);
  sda_settings_info_screen(0);
  sda_settings_notif_screen(0);
  sda_settings_security_screen(0);
}