/*
Copyright (c) 2019 Stanislav Brtna

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

#include "sda_battery.h"

#define MIN_VOLTAGE 3.1
#define MAX_VOLTAGE 4.0


static uint8_t get_batt_percent(float voltage) {
	uint8_t percent;
	percent = (uint8_t)((voltage - MIN_VOLTAGE) / ((MAX_VOLTAGE - MIN_VOLTAGE) / 100 ));
	if (percent > 100) {
		percent = 100;
	}
	return percent;
}


static void reload_batt_string(float voltage) {
  uint32_t temp;
  temp = (uint32_t)(voltage * 10);
  svpSGlobal.battString[0] = ' ';
  svpSGlobal.battString[1] = temp / 10 + 48;
  svpSGlobal.battString[2] = '.';
  svpSGlobal.battString[3] = temp % 10 + 48;
  svpSGlobal.battString[4] = 'V';
  svpSGlobal.battString[5] = 0;
}


static void reload_batt_percent(uint8_t percentage) {
  if (svpSGlobal.battPercentage > percentage) {
    if (svpSGlobal.battPercentage > 5) {
      svpSGlobal.battPercentage -= 5;
    }
  }
}


void sda_handle_battery_status() {
  // read sda_is_battery_measured(); then sda_get_battery_voltage();
  float batteryVoltage;
  uint32_t temp;
  static uint8_t unplugged_flag;
  static uint8_t battPercentRealValue;
  static uint32_t oldSysPercentageTime;
  systemPwrType oldBattState;

  if (sda_is_battery_measured()) {
    batteryVoltage = sda_get_battery_voltage();
    reload_batt_string(batteryVoltage);
   
		battPercentRealValue = get_batt_percent(batteryVoltage);

    if (unplugged_flag) {
      if (svpSGlobal.uptime > oldSysPercentageTime + 30) {

      }

      unplugged_flag = 0;
    } else {
      if (svpSGlobal.uptime > oldSysPercentageTime + 30) {
        reload_batt_percent(battPercentRealValue);
      }
      oldSysPercentageTime = svpSGlobal.uptime;
    }
  }

  // reset batt state after unplugging device from charger
  if (oldBattState != svpSGlobal.pwrType && svpSGlobal.pwrType == POWER_BATT) {
    // raise some sort of flag that will override default behaviour and measure battery few times to get accurate result
    unplugged_flag = 1;
    oldSysPercentageTime = svpSGlobal.uptime;
  }

  oldBattState = svpSGlobal.pwrType;
  /*
  	// if we do not know (etc reset) set it to measured, rounded by 5
	if (battDisplayValue == 0 || battDisplayValue == 101) {
		if (svpSGlobal.battPercentage == 101) {
			battDisplayValue = 101;
		} else {
			battDisplayValue = svpSGlobal.battPercentage - svpSGlobal.battPercentage % 5;
		}
	} else {
		// check every 30s if measured is lower, if yes, then subtract 5% from value
		if ((svpSGlobal.uptime > oldSysPercentageTime + 30) && (svpSGlobal.battPercentage < battDisplayValue)) {
			if (battDisplayValue > 5) {
				battDisplayValue -= 5;
			}
			oldSysPercentageTime = svpSGlobal.uptime;
		}
	}

  // reset the measured value when unplugged from charger
  if (oldBattState != svpSGlobal.pwrType && svpSGlobal.pwrType == POWER_BATT) {
      if (svpSGlobal.battPercentage != 101) {
          battDisplayValue = svpSGlobal.battPercentage - svpSGlobal.battPercentage % 5;
      }
  }
*/
}