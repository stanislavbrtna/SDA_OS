#include "sda_irq.h"

void sda_irq_update_timestruct(uint16_t year, uint8_t mon, uint8_t day, uint8_t weekday, uint8_t hour, uint8_t min, uint8_t sec) {
	static uint8_t oldsec;
	svpSGlobal.sec = sec;
	svpSGlobal.min = min;
	svpSGlobal.hour = hour;
	svpSGlobal.day = day;
	svpSGlobal.weekday = weekday;
	svpSGlobal.month = mon;
	svpSGlobal.year = year;
	svpSGlobal.timestamp
		= sdaTimeGetTimestamp(
				year,
				mon,
				day,
				hour,
				min,
				sec
		);

	if(svpSGlobal.sec != oldsec) {
		svpSGlobal.uptime++;
		if (svpSGlobal.lcdState == LCD_ON) {
			svpSGlobal.lcdOnTime++;
		}

		if (svpSGlobal.pwrType == POWER_BATT) {
			svpSGlobal.battTime++;
		} else {
			svpSGlobal.battTime = 0;
		}
	}
	oldsec = sec;
}
