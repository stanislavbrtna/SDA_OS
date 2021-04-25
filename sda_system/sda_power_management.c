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

#include "sda_power_management.h"

// time of the last input
static uint32_t lastInputTime;

// flag to disable automatic sleep
uint8_t sleepLock;

void sda_set_sleep_lock(uint8_t val) {
  sleepLock = val;
}

void sda_power_sleep() {
  if (svpSGlobal.lcdState == LCD_ON) {
    svp_set_lcd_state(LCD_OFF);
  }
  svpSGlobal.powerMode = SDA_PWR_MODE_SLEEP;
  system_clock_set_low();
}


// wakes from sleep to low power mode with screen off,
// goes to sleep again after lcd shutdown time
void sda_interrupt_sleep() {
  if (svpSGlobal.powerMode == SDA_PWR_MODE_NORMAL) {
    return;
  }
  svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
  lastInputTime = svpSGlobal.uptime;
}


void sda_lcd_on_handler() {
  system_clock_set_normal();
  svp_set_irq_redraw();
  svpSGlobal.powerMode = SDA_PWR_MODE_NORMAL;
}


void sda_lcd_off_handler() {
  svpSGlobal.powerMode = SDA_PWR_MODE_SLEEP;

  if ((wrap_get_lcdOffButtons() == 1 && slotOnTop[4] && slotValid[4]) || sdaSvmIsTimerSet()) {
    svpSGlobal.powerSleepMode = SDA_PWR_MODE_SLEEP_LOW;
  } else if (sdaGetActiveAlarm() == 1) {
    svpSGlobal.powerSleepMode = SDA_PWR_MODE_SLEEP_NORMAL;
  } else {
    svpSGlobal.powerSleepMode = SDA_PWR_MODE_SLEEP_DEEP;
  }

}

void sda_power_wait_for_input_UMC() {
#ifndef PC
  uint32_t pwrDelay = 10000;

  for (uint32_t x = 0; x < pwrDelay; x++) { // waiting for next touch event
    if (svpSGlobal.touchValid == 1) {
      lastInputTime = svpSGlobal.uptime;
      break;
    }

    if (svpSGlobal.btnFlag == 1) {
      svpSGlobal.btnFlag = 0;
      lastInputTime = svpSGlobal.uptime;

      // when called from lcd off, we go to sleep after 30s
      if (svpSGlobal.lcdState == LCD_OFF) {
        //btnSleepTimer = svpSGlobal.uptimeMs + 5000;
      }
      break;
    }
  }
#endif
}


void sda_power_management_handler() {
  static lcdStateType lcdStateOld;
  static volatile uint32_t lcdOffBlinkTimer;
  static volatile uint32_t btnSleepTimer;
  static uint32_t pwrDelay;

  if (svpSGlobal.touchValid) {
    lastInputTime = svpSGlobal.uptime;
  }

  // when lcd is turned ON
  if ((svpSGlobal.lcdState == LCD_ON) && (lcdStateOld == LCD_OFF)) {
    lastInputTime = svpSGlobal.uptime;
    sda_lcd_on_handler();
    lcdOffBlinkTimer = 0;
    led_set_pattern(LED_OFF);
  }

  // when lcd is turned OFF
  if ((svpSGlobal.lcdState == LCD_OFF) && (lcdStateOld == LCD_ON)) {

    if (((wrap_get_lcdOffButtons() == 1) && slotValid[4] && slotOnTop[4]) || sdaSvmIsTimerSet()) {
      lcdOffBlinkTimer = svpSGlobal.uptimeMs + 100;
    } else if (sdaGetActiveAlarm()){
      lcdOffBlinkTimer = svpSGlobal.uptimeMs + 500;
    } else {
      lcdOffBlinkTimer = svpSGlobal.uptimeMs + 1000;
    }
    led_set_pattern(LED_ON);
    sda_lcd_off_handler();
  }

  if ((lcdOffBlinkTimer != 0) && (lcdOffBlinkTimer < svpSGlobal.uptimeMs)) {
    led_set_pattern(LED_OFF);
    lcdOffBlinkTimer = 0;
    // after we blink the led, system will underclock itself
    // to gave time for apps or system to do stuff after lcd shutdown
    system_clock_set_low();
  }

  if ((btnSleepTimer != 0) && (btnSleepTimer < svpSGlobal.uptimeMs)) {
    btnSleepTimer = 0;
    // to handle the scenario when button is pressed with screen off
    system_clock_set_low();
    sda_power_sleep();
  }

  // lcd auto shut down, this must be at the bottom, so it does not turn off before the sda wakes.
  if (((svpSGlobal.lcdShutdownTime * 60) < (svpSGlobal.uptime - lastInputTime))
        && (sleepLock == 0) && (svpSGlobal.powerMode != SDA_PWR_MODE_SLEEP)) {
    sda_power_sleep();
  }

  lcdStateOld = svpSGlobal.lcdState;
}