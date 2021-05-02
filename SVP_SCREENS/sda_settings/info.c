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

uint8_t uptime[20];
uint8_t uptimeL[20];
uint8_t uptimePWR[20];

uint8_t update_uptime() {
  static uint32_t uptime_prev;
  if (svpSGlobal.uptime != uptime_prev) {
    sda_time_to_str(uptime, svpSGlobal.uptime);
    uptime_prev = svpSGlobal.uptime;
    return 1;
  }
  return 0;
}

uint8_t update_uptimeL() {
  static uint32_t  uptime_prev;
  if (svpSGlobal.lcdOnTime != uptime_prev) {
    sda_time_to_str(uptimeL, svpSGlobal.lcdOnTime);
    uptime_prev = svpSGlobal.lcdOnTime;
    return 1;
  }
  return 0;
}

uint8_t update_uptimePWR() {
  static uint32_t  uptime_prev;
  if (svpSGlobal.battTime != uptime_prev) {
    sda_time_to_str(uptimePWR, svpSGlobal.battTime);
    uptime_prev = svpSGlobal.battTime;
    return 1;
  }
  return 0;
}


uint16_t sda_settings_info_screen(uint8_t init) {
  // system info screen
  static uint16_t infoBattStr;
  static uint16_t infoUptime;
  static uint16_t infoUptimeL;
  static uint16_t infoUptimePWR;
  static uint8_t infoBattPercentPrev;
  static uint16_t optInfoBack;
  static uint16_t internalScr;
  static uint16_t slider;
  uint16_t optInfoScr;

  if (init == 1) {
    optInfoScr = pscg_add_screen(&sda_sys_con);
    internalScr = pscg_add_screen(&sda_sys_con);

    pscg_add_image(1, 1, 8, 4, (uint8_t *)"Icons/logo.p16", optInfoScr, &sda_sys_con);
    pscg_add_text(1, 4, 10, 5, SCR_OS_VERSION" "SDA_OS_VERSION, optInfoScr, &sda_sys_con);

    slider = pscg_add_slider_v(8, 5, 9, 11, 170, 0, optInfoScr, &sda_sys_con);

    pscg_set_screen(internalScr, optInfoScr, &sda_sys_con);
    pscg_set_x1y1x2y2(internalScr, 1, 5, 8, 11, &sda_sys_con);

    pscg_add_text(0, 0, 7, 1, SCR_BATT_VOLTAGE, internalScr, &sda_sys_con);
    infoBattStr = pscg_add_text(0, 1, 10, 2, svpSGlobal.battString, internalScr, &sda_sys_con);

    pscg_add_text(0, 2, 5, 3, SCR_UPTIME, internalScr, &sda_sys_con);
    update_uptime();
    infoUptime = pscg_add_text(0, 3, 10, 4, uptime, internalScr, &sda_sys_con);

    pscg_add_text(0, 4, 10, 5, SCR_SCREEN_ON_TIME, internalScr, &sda_sys_con);
    update_uptimeL();
    infoUptimeL = pscg_add_text(0, 5, 10, 6, uptimeL, internalScr, &sda_sys_con);

    pscg_add_text(0, 6, 10, 7, SCR_BATT_PWR_TIME, internalScr, &sda_sys_con);
    update_uptimePWR();
    infoUptimePWR = pscg_add_text(0, 7, 10, 8, uptimePWR, internalScr, &sda_sys_con);

    pscg_add_text(0, 8, 10, 9,(uint8_t *)"SVS version: "SVS_VERSION, internalScr, &sda_sys_con);
    pscg_add_text(0, 9, 10, 12,(uint8_t *)"Compiled:\n"__DATE__" "__TIME__"\n(c) Standa", internalScr, &sda_sys_con);

    optInfoBack = pscg_add_button(1, 11, 5, 12, SCR_BACK, optInfoScr, &sda_sys_con);
    return optInfoScr;
  }

  if (init == 2) {
    pscg_set_value(slider,0, &sda_sys_con);
    return 0;
  }

  if (gr2_clicked(optInfoBack, &sda_sys_con)) {
    mainScr = slotScreen[2];
    setRedrawFlag();
  }

  if (infoBattPercentPrev != svpSGlobal.battPercentage) {
    pscg_set_modified(infoBattStr, &sda_sys_con);
    infoBattPercentPrev = svpSGlobal.battPercentage;
  }

  if(update_uptime()){
    pscg_set_modified(infoUptime, &sda_sys_con);
  }

  if(update_uptimeL()) {
    pscg_set_modified(infoUptimeL, &sda_sys_con);
  }

  if(update_uptimePWR()) {
    pscg_set_modified(infoUptimePWR, &sda_sys_con);
  }

  pscg_set_yscroll(internalScr, pscg_get_value(slider, &sda_sys_con), &sda_sys_con);

  return 0;
}