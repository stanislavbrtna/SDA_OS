/*
Copyright (c) 2026 Stanislav Brtna

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

#ifndef SDA_TYPES_HW_H
#define SDA_TYPES_HW_H

typedef enum {PRESSED, HOLD,LONGHOLD, RELEASED} eventType;

typedef enum {LED_ON, LED_OFF ,LED_BLINK, LED_SHORTBLINK, LED_ALARM} ledPatternType;

typedef enum {LCD_ON, LCD_OFF} lcdStateType;

typedef enum {SDA_LOCK_LOCKED, SDA_LOCK_UNLOCKED} sdaLockState;

typedef enum {SDA_PWR_MODE_NORMAL, SDA_PWR_MODE_SLEEP} pwrModeType;

typedef enum {SDA_PWR_MODE_SLEEP_LOW, SDA_PWR_MODE_SLEEP_NORMAL, SDA_PWR_MODE_SLEEP_DEEP, SDA_PWR_MODE_SLEEP_STANDBY} pwrSleepModeType;

// Power management enum
typedef enum {
  PERFORMANCE,
  POWERSAVE,
  AUTO,
} sdaPwrModeType;

typedef enum {POWER_USB, POWER_BATT} systemPwrType;

typedef enum {CLICKED_NONE, CLICKED_SHORT, CLICKED_LONG} clickType;

typedef enum {DEVICE_LOCKED, DEVICE_UNLOCKED} sdaDeviceLockType;

typedef enum {ON_SCREEN_KEYBOARD, OTHER} sdaInputMethodType;

typedef enum {SPEAKER, HEADPHONES} sdaPCMOutputType;

typedef enum {
  EXTERNAL_EXPANSION_PORT = 1,
  INTERNAL_EXPANSION_PORT = 2,
  SERIAL_PORT = 3,
  USB_PORT = 4
} sdaResource;

#endif
