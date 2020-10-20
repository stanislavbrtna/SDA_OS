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

uint16_t svp_opt_debug(uint8_t init) {

  static uint16_t dbgUartEnable;
  static uint16_t dbgUartDisable;
  static uint16_t optDbgBack;
  uint16_t optDbgScr;

  if (init == 1){
    // notifiacations screen
    optDbgScr = pscg_add_screen(&sda_sys_con);
    pscg_add_text(1, 1, 10, 2, (uint8_t *)"Debug serial port", optDbgScr, &sda_sys_con);
    dbgUartEnable = pscg_add_checkbox(1, 2, 7, 3, (uint8_t *)"Enabled", optDbgScr, &sda_sys_con);
    dbgUartDisable = pscg_add_checkbox(1, 3, 7, 4, (uint8_t *)"Disabled", optDbgScr, &sda_sys_con);

    optDbgBack = pscg_add_button(1, 10, 4, 11, SCR_BACK, optDbgScr, &sda_sys_con);

    if (sda_dbg_serial_is_enabled()) {
      pscg_set_value(dbgUartEnable, 1, &sda_sys_con);
    } else {
      pscg_set_value(dbgUartDisable, 1, &sda_sys_con);
    }

    return optDbgScr;
  }

  if (gr2_clicked(dbgUartEnable, &sda_sys_con)) {
    sda_dbg_serial_enable();
    init = 2;
  }

  if (gr2_clicked(dbgUartDisable, &sda_sys_con)) {
    sda_dbg_serial_disable();
    init = 2;
  }

  if (init == 2) {
    if (sda_dbg_serial_is_enabled()) {
      pscg_set_value(dbgUartEnable, 1, &sda_sys_con);
      pscg_set_value(dbgUartDisable, 0, &sda_sys_con);
    } else {
      pscg_set_value(dbgUartEnable, 0, &sda_sys_con);
      pscg_set_value(dbgUartDisable, 1, &sda_sys_con);
    }
  }

  if (gr2_clicked(optDbgBack, &sda_sys_con)) {
    mainScr = slotScreen[2];
    setRedrawFlag();
  }

  return 0;
}