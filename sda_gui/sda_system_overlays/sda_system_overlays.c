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

#include "sda_system_overlays.h"

extern uint8_t soft_error_flag;

void sda_system_overlays_handler() {
  if (sda_batt_overlay_shown()) {
    batt_overlay_handle(0);
  }

  if(sda_dc_overlay_shown()) {
    sda_dc_overlay_handle(0);
  }

  if(sda_ql_overlay_shown()) {
    sda_ql_overlay_handle(0);
  }

  if (soft_error_flag == 1) {
    sda_error_overlay_handle();
  }

  taskSwitcherUpdate();
  sda_auth_overlay_handle();
  sda_clipboard_overlay_update();
}

uint8_t sda_os_overlay_visible() {
  if(sda_batt_overlay_shown())
    return 1;
  if(sda_dc_overlay_shown())
    return 1;
  if(sda_ql_overlay_shown())
    return 1;
  if(soft_error_flag == 1)
    return 1;
  if(taskSwitcherVisible())
    return 1;
  if(sda_clipboard_overlay_visible())
    return 1;

  return 0;
}