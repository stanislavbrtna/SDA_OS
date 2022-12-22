/*
Copyright (c) 2022 Stanislav Brtna

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

#include "sda_svm.h"
static int32_t alarmId;
static int32_t alarmParam;
static uint8_t alarmFlag;

static uint8_t redrawDetect;

// svs
uint8_t svs_wrap_setScr_flag;
uint16_t svs_wrap_setScr_id;


void sdaSvmGetGR2Settings() {
  // load colors from system to app context
  gr2_set_border_color(gr2_get_border_color(&sda_sys_con), &sda_app_con);
  gr2_set_text_color(gr2_get_text_color(&sda_sys_con), &sda_app_con);
  gr2_set_background_color(gr2_get_background_color(&sda_sys_con), &sda_app_con);
  gr2_set_fill_color(gr2_get_fill_color(&sda_sys_con), &sda_app_con);
  gr2_set_active_color(gr2_get_active_color(&sda_sys_con), &sda_app_con);
}


// notification
void sda_alarm_set_flag(int32_t id, int32_t param) {
  alarmId = id;
  alarmParam = param;
  alarmFlag = 1;
}


int32_t sda_alarm_get_id() {
  return alarmId;
}


int32_t sda_alarm_get_param() {
  return alarmParam;
}


int8_t sda_alarm_get_flag() {
  return alarmFlag;
}


void sda_alarm_clear_flag() {
  alarmFlag = 0;
}


// screens and stuff
void sdaSvmSetMainScreen(uint16_t val) {
  svs_wrap_setScr_id = val;
  svs_wrap_setScr_flag = 1;
  gr2_set_modified(val, &sda_app_con);
}


void sdaSetRedrawDetect(uint8_t val) {
  redrawDetect = val;
}


uint8_t sdaGetRedrawDetect() {
  return redrawDetect;
}