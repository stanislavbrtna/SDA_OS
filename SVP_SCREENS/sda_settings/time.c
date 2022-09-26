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
  static uint16_t optTimBack;

  static uint16_t timeOvrId;
  static uint16_t dateOvrId;

  if (init == 1) {

    optTimeScr = gr2_add_screen(&sda_sys_con);
    gr2_add_text(1, 1, 10, 2, SCR_SET_TIME_SCR, optTimeScr, &sda_sys_con);
    optTimeBtn = gr2_add_button(2, 2, 7, 3, SCR_SET_TIME, optTimeScr, &sda_sys_con);
    optDateBtn = gr2_add_button(2, 3, 7, 4, SCR_SET_DATE, optTimeScr, &sda_sys_con);
    optTimBack = gr2_add_button(1, 6, 4, 7, SCR_BACK, optTimeScr, &sda_sys_con);
    gr2_text_set_align(optTimBack, GR2_ALIGN_CENTER, &sda_sys_con);

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
    #ifdef PLATFORM_PC
    printf("setting time: %u:%u (Does not work in simulation)\n", time_overlay_get_hours(timeOvrId), time_overlay_get_minutes(timeOvrId));
    #else
    printf("Old time: y:%u d:%u wkd:%u mon:%u %u:%u \n",svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, svpSGlobal.hour, svpSGlobal.min);
    rtc_set_time(svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, (uint8_t) time_overlay_get_hours(timeOvrId), (uint8_t) time_overlay_get_minutes(timeOvrId), 0);
    printf("New time: y:%u d:%u wkd:%u mon:%u %u:%u \n",svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, svpSGlobal.hour, svpSGlobal.min);
    #endif
    time_overlay_clear_ok(timeOvrId);
    timeOvrId = 0;
  }

  if(date_overlay_get_ok(dateOvrId)) {
#ifndef PC
    rtc_set_time(date_overlay_get_year(dateOvrId), date_overlay_get_day(dateOvrId), svpSGlobal.weekday, date_overlay_get_month(dateOvrId),svpSGlobal.hour, svpSGlobal.min, svpSGlobal.sec);
    printf("New date: y:%u d:%u wkd:%u mon:%u %u:%u \n",svpSGlobal.year, svpSGlobal.day, svpSGlobal.weekday, svpSGlobal.month, svpSGlobal.hour, svpSGlobal.min);
#else
    printf("setting date locally: %u. %u. %u \n", date_overlay_get_day(dateOvrId), date_overlay_get_month(dateOvrId), date_overlay_get_year(dateOvrId));
    svpSGlobal.day=date_overlay_get_day(dateOvrId);
    svpSGlobal.month=date_overlay_get_month(dateOvrId);
    if (date_overlay_get_year(dateOvrId) > 2005) {
      svpSGlobal.year=date_overlay_get_year(dateOvrId);
    }
#endif
    svpSGlobal.dateUpdated = 1;
    date_overlay_clear_ok(dateOvrId);
    dateOvrId = 0;
  }

  if (gr2_clicked(optTimBack, &sda_sys_con)) {
    mainScr = slotScreen[2];
    setRedrawFlag();
  }
}