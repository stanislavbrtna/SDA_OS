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

  // uptime coul be modified when wake from sleep occurs
  if (svpSGlobal.sec != oldsec) {
    svpSGlobal.uptime++;
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
