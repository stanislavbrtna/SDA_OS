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

//main homescreenu, init provede inicializaci, top pokud je on top
uint16_t svp_homeScreen(uint8_t init, uint8_t top) {
  static uint16_t screen;
	static uint16_t appsBtn;
  static uint16_t optBtn;
  static uint16_t text;
  static uint16_t time;
  static uint16_t date;
  static uint16_t oldtime;
  static uint8_t olddate;
  static uint8_t time_string[6];
  static uint8_t date_string[42];

  if (init == 1) {
    screen = pscg_add_screen(&sda_sys_con);
    text = pscg_add_text(0 , 1, 12, 3,(uint8_t *)"S!   PDA", screen, &sda_sys_con);
    time = pscg_add_text(3, 4, 12, 6,(uint8_t *)"??:??", screen, &sda_sys_con);
    date = pscg_add_text(3, 6, 12, 7,(uint8_t *)"?. ?. 20??", screen, &sda_sys_con);
    appsBtn = pscg_add_icon(2, 10, 5, 13,(uint8_t *)"", (uint8_t *)"apps.p16", screen, &sda_sys_con);
    optBtn = pscg_add_icon(6, 10, 9, 13,(uint8_t *)"", (uint8_t *)"options.p16", screen, &sda_sys_con);
    pscg_text_set_size(text, 70, &sda_sys_con);
    pscg_text_set_size(time, 70, &sda_sys_con);
    oldtime = 5566; // absurd value, so current minute would not equal oldmin and time would update after init
    return screen;
  }

  //loop top
  if (top == 1) {
    svpSGlobal.systemXBtnVisible = 0;

    //time refresh
    if (((uint16_t)svpSGlobal.hour * 10) + (uint16_t)svpSGlobal.min != oldtime) {
      time_string[0] = svpSGlobal.hour / 10 + 48;
		  time_string[1] = svpSGlobal.hour % 10 + 48;
		  time_string[2] = ':';
		  time_string[3] = svpSGlobal.min / 10 + 48;
		  time_string[4] = svpSGlobal.min % 10 + 48;
		  time_string[5] = 0;
		  pscg_set_str(time, time_string, &sda_sys_con);
		  oldtime = ((uint16_t)svpSGlobal.hour * 10) + (uint16_t)svpSGlobal.min;
    }

    //date refresh
    if ((svpSGlobal.day != olddate) || (svpSGlobal.dateUpdated)) {
      svp_write_date_string(date_string, 0, 1);
      pscg_set_str(date, date_string, &sda_sys_con);
      pscg_set_modified(date, &sda_sys_con);
      olddate = svpSGlobal.day;
      svpSGlobal.dateUpdated = 0;
    }

		if (pscg_get_event(appsBtn, &sda_sys_con) == EV_RELEASED) {
		  sdaSlotOnTop(1);
		}
		pscg_set_event(appsBtn, EV_NONE, &sda_sys_con);

		if (pscg_get_event(optBtn, &sda_sys_con) == EV_RELEASED) {
		  sdaSlotOnTop(2);
		}
		pscg_set_event(optBtn, EV_NONE, &sda_sys_con);
  }
  //else: work in the background, noting for now
  return 0;
}
