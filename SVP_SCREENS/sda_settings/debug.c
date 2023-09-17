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

extern svsVM svm;
extern sdaSvmMetadata svmMeta;

uint16_t sda_settings_debug_screen(uint8_t init) {

  static uint16_t dbgUartEnable;
  static uint16_t optDbgBack;
  static uint16_t bMemInfo;
  static uint16_t bSvmInfo;
  static uint16_t bCClean;
  
  uint16_t optDbgScr;

  if (init == 1){
    // notifiacations screen
    optDbgScr = gr2_add_screen(&sda_sys_con);
    gr2_set_relative_init(1, &sda_sys_con);
    gr2_add_text(1, 1, 9, 1, (uint8_t *)"Debug", optDbgScr, &sda_sys_con);
    dbgUartEnable = gr2_add_checkbox(1, 3, 9, 1, (uint8_t *)"Use USB port for debug", optDbgScr, &sda_sys_con);

    bCClean = gr2_add_button(1, 5, 6, 1, (uint8_t *)"Clean cached Apps", optDbgScr, &sda_sys_con);

    bMemInfo = gr2_add_button(1, 7, 6, 1, (uint8_t *)"Print memory info", optDbgScr, &sda_sys_con);
    bSvmInfo = gr2_add_button(1, 8, 6, 1, (uint8_t *)"Print SVSvm info", optDbgScr, &sda_sys_con);

    

    optDbgBack = gr2_add_button(1, 10, 3, 1, SCR_BACK, optDbgScr, &sda_sys_con);

    
    gr2_text_set_align(optDbgBack, GR2_ALIGN_CENTER, &sda_sys_con);

    if (sda_usb_serial_is_enabled()) {
      gr2_set_value(dbgUartEnable, 1, &sda_sys_con);
    }

    gr2_set_relative_init(0, &sda_sys_con);
    return optDbgScr;
  }

  if (gr2_clicked(dbgUartEnable, &sda_sys_con)) {
    sda_usb_enable_for_dbg(gr2_get_value(dbgUartEnable, &sda_sys_con));
    sda_store_dbg_options();
    init = 2;
  }

  if (gr2_clicked(bMemInfo, &sda_sys_con)) {
    printf("SDA_OS memsize:\n   svm:        %u B\n   svmMeta:    %u B\n   gr2 system: %u B\n   gr2 app:    %u B\n   Total:      %u B\n",
      sizeof(svm), sizeof(svmMeta),
      sizeof(gr2Element)*SDA_SYS_ELEM_MAX + sizeof(gr2Screen)*SDA_SYS_SCREEN_MAX,
      sizeof(gr2Element)*SDA_APP_ELEM_MAX  + sizeof(gr2Screen)*SDA_APP_SCREEN_MAX,
      sizeof(svm) + sizeof(svmMeta) +
      sizeof(gr2Element)*SDA_SYS_ELEM_MAX  + sizeof(gr2Screen)*SDA_SYS_SCREEN_MAX +
      sizeof(gr2Element)*SDA_APP_ELEM_MAX  + sizeof(gr2Screen)*SDA_APP_SCREEN_MAX
    );
  }

  if (gr2_clicked(bSvmInfo, &sda_sys_con)) {
    svsPrintUsedUp(&svm);
  }

  if (gr2_clicked(bCClean, &sda_sys_con)) {
    svmRemovePrecache();
  }

  if (init == 2) {
    if (sda_usb_get_enable_for_dbg()) {
      gr2_set_value(dbgUartEnable, 1, &sda_sys_con);
    } else {
      gr2_set_value(dbgUartEnable, 0, &sda_sys_con);
    }

    return optDbgBack;
  }

  if (gr2_clicked(optDbgBack, &sda_sys_con)) {
    mainScr = slotScreen[2];
    setRedrawFlag();
  }

  return 0;
}
