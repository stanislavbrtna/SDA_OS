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


uint16_t sda_settings_time_screen(uint8_t init) {
  uint16_t optTimeScr;
  //nastavení data a času
  static uint16_t optTimeBtn;
  static uint16_t optDateBtn;


  static uint16_t timeOvrId;
  static uint16_t dateOvrId;

  if (init == 1) {

    optTimeScr = gr2_add_screen(&sda_sys_con);
    
    gr2_set_yscroll(optTimeScr, 16, &sda_sys_con);
    optTimeBtn = gr2_add_button(2, 1, 7, 2, SCR_SET_TIME, optTimeScr, &sda_sys_con);
    optDateBtn = gr2_add_button(2, 2, 7, 3, SCR_SET_DATE, optTimeScr, &sda_sys_con);
    
    return optTimeScr;
  }

  if (gr2_clicked(optTimeBtn, &sda_sys_con)) {
    timeOvrId = time_overlay_init();
    time_overlay_set_time(timeOvrId, svpSGlobal.hour, svpSGlobal.min);
  }

  if (gr2_clicked(optDateBtn, &sda_sys_con)) {
    dateOvrId = date_overlay_init(svpSGlobal.year, svpSGlobal.month, svpSGlobal.day);
    printf("Old date: y:%u d:%u wkd:%u mon:%u %u:%u \n",
            svpSGlobal.year,
            svpSGlobal.day,
            svpSGlobal.weekday,
            svpSGlobal.month,
            svpSGlobal.hour,
            svpSGlobal.min
          );
  }

  date_overlay_update(dateOvrId);

  time_overlay_update(timeOvrId);

  if(time_overlay_get_ok(timeOvrId)) {
    sda_set_time(svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, (uint8_t) time_overlay_get_hours(timeOvrId), (uint8_t) time_overlay_get_minutes(timeOvrId), 0);
    time_overlay_clear_ok(timeOvrId);
    timeOvrId = 0;
  }

  if(date_overlay_get_ok(dateOvrId)) {
    sda_set_time(date_overlay_get_year(dateOvrId), date_overlay_get_day(dateOvrId), svpSGlobal.weekday, date_overlay_get_month(dateOvrId),svpSGlobal.hour, svpSGlobal.min, svpSGlobal.sec);
    date_overlay_clear_ok(dateOvrId);
    dateOvrId = 0;
  }

  return 0;
}
