/*
Copyright (c) 2024 Stanislav Brtna

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

#include "sda_quick_launch.h"

uint16_t ql_overlay;
uint8_t  ql_overlay_flag;

void sda_ql_overlay_destructor();

typedef struct {
  uint16_t id;
  uint8_t  fPath[APP_NAME_LEN+1];
  uint8_t  iPath[APP_NAME_LEN+1];
  uint8_t  appName[32];
} qlIconType;

qlIconType qlIcons[SDA_QUICK_LAUNCH_POSITIONS];


void sda_ql_overlay_init() {
  if (ql_overlay_flag == 0) {
    destroyOverlay();
    sda_ql_overlay_handle(1);
    setOverlayScreen(ql_overlay, &sda_sys_con);
    setOverlayDestructor(sda_ql_overlay_destructor);
    ql_overlay_flag = 1;
    setOverlayY1(191 - 16);
    setOverlayY2(479);
    setOverlayX1(32);
    setOverlayX2(320 - 3*32);
  }
}


void sda_ql_overlay_destructor() {
  gr2_destroy(ql_overlay, &sda_sys_con);
  setRedrawFlag();
  ql_overlay_flag = 0;
  ql_overlay = 0xFFFF;
}


uint8_t sda_ql_overlay_shown() {
  return ql_overlay_flag;
}


void sda_ql_load_icons() {
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");
  svp_csvf qlCSV;

  for(int32_t i = 0; i < SDA_QUICK_LAUNCH_POSITIONS; i++) {
    qlIcons[i].id = 0;
  }

  if (svp_csv_open(&qlCSV, "quick.mnu") == 0) {
    gr2_add_text(1, 1, 20, 4, ASCR_CARD_ERROR, ql_overlay, &sda_sys_con);
    return;
  }

  uint32_t i = 0;

  do {
    svp_csv_get_cell(&qlCSV, 0, (uint8_t *)"", qlIcons[i].fPath, APP_NAME_LEN);
    svp_csv_get_cell(&qlCSV, 1, (uint8_t *)"none.p16", qlIcons[i].iPath, APP_NAME_LEN);
    svp_csv_get_cell(&qlCSV, 2, ASCR_NONAME, qlIcons[i].appName, 32);
    
    //printf("ql got: %s, %s, %s\n", qlIcons[i].fPath, qlIcons[i].iPath, qlIcons[i].appName);

    qlIcons[i].id = gr2_add_icon(
      0 + 6*(i%2),
      0 + 6*(i/2),
      6 + 6*(i%2),
      6 + 6*(i/2),
      qlIcons[i].appName,
      qlIcons[i].iPath,
      ql_overlay,
      &sda_sys_con
    );
    gr2_text_set_align(qlIcons[i].id, GR2_ALIGN_CENTER, &sda_sys_con);
    gr2_set_param(qlIcons[i].id, 1, &sda_sys_con);

    i++;

    if (i == SDA_QUICK_LAUNCH_POSITIONS) {break;}
  } while(svp_csv_next_line(&qlCSV));
}


void sda_ql_overlay_handle(uint8_t init) {

  if (init == 1) {
    ql_overlay = gr2_add_screen(&sda_sys_con);
    gr2_set_x_cell(ql_overlay, 16, &sda_sys_con);
    gr2_set_y_cell(ql_overlay, 16, &sda_sys_con);

    gr2_set_yscroll(ql_overlay, -16, &sda_sys_con);

    sda_ql_load_icons();
    return;
  }


  for(int32_t i = 0; i < SDA_QUICK_LAUNCH_POSITIONS; i++) {
    if (gr2_get_event(qlIcons[i].id, &sda_sys_con) == EV_RELEASED) {
      uint8_t type = sda_menu_detect_type(qlIcons[i].fPath);

      if(type == OBJ_TYPE_MENU) {
        sda_app_screen_load(qlIcons[i].fPath, qlIcons[i].appName);
        destroyOverlay();
        sda_slot_on_top(SDA_SLOT_APPLIST);
        return;
      }

      if(type == OBJ_TYPE_APP && svmLaunch(qlIcons[i].fPath, 0) == 0) {
        sda_show_error_message((uint8_t *)"Error occured while launching file!");
      }

      gr2_set_event(qlIcons[i].id, EV_NONE, &sda_sys_con);
      ql_overlay_flag = 0;
      setRedrawFlag();
      destroyOverlay();
      return;
    }
    gr2_set_event(qlIcons[i].id, EV_NONE, &sda_sys_con);
  }

  // destroy overlay on screen off or back button released events
  if(svpSGlobal.lcdState == LCD_OFF || sda_wrap_get_button(BUTTON_A) == EV_RELEASED) {
    ql_overlay_flag = 0;
    setRedrawFlag();
    destroyOverlay();
    sda_wrap_clear_button(BUTTON_A);
    return;
  }

  sda_screen_button_handler(ql_overlay, 0, &sda_sys_con);
}