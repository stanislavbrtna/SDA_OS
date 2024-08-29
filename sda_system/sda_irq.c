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

#include "sda_irq.h"

void sda_irq_update_timestruct(
  uint16_t year,
  uint8_t mon,
  uint8_t day,
  uint8_t weekday,
  uint8_t hour,
  uint8_t min,
  uint8_t sec
) {
  static uint32_t oldtime;
  static uint8_t oldsec;
  svpSGlobal.sec = sec;
  svpSGlobal.min = min;
  svpSGlobal.hour = hour;
  svpSGlobal.day = day;
  svpSGlobal.weekday = weekday;
  svpSGlobal.month = mon;
  svpSGlobal.year = year;
  svpSGlobal.timestamp = sdaTimeGetTimestamp(
    year,
    mon,
    day,
    hour,
    min,
    sec
  );

  // uptime could be modified when wake from sleep occurs
  if (svpSGlobal.sec != oldsec) {
    svpSGlobal.uptime++;
    if (svpSGlobal.uptimeMs < (svpSGlobal.uptime - 2) * 1000 || svpSGlobal.uptimeMs > (svpSGlobal.uptime + 2) * 1000) {
      svpSGlobal.uptimeMs = svpSGlobal.uptime * 1000;
    }
    oldsec = sec;
  }

  // and because of that, we must be carefull, more seconds than one might pass
  if (svpSGlobal.uptime != oldtime) {

    if (svpSGlobal.lcdState == LCD_ON) {
      svpSGlobal.lcdOnTime += svpSGlobal.uptime - oldtime;
    }

    if (svpSGlobal.pwrType == POWER_BATT) {
      svpSGlobal.battTime += svpSGlobal.uptime - oldtime;
    } else {
      svpSGlobal.battTime = 0;
    }
  }
  oldtime = svpSGlobal.uptime;
}
