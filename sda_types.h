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

File description:
  This file stores main custom data types used in SDA-OS.

*/

#ifndef SDA_TYPES_H
#define SDA_TYPES_H

#include "SVS/svs_limits.h" // for varType in svmMeta
#include "SVS/svs_types.h"

typedef enum {PRESSED, HOLD,LONGHOLD, RELEASED} eventType;

typedef enum {LED_ON, LED_OFF ,LED_BLINK, LED_SHORTBLINK, LED_ALARM} ledPatternType;

typedef enum {LCD_ON, LCD_OFF} lcdStateType;

typedef enum {SDA_LOCK_LOCKED, SDA_LOCK_UNLOCKED} sdaLockState;

typedef enum {PWR_MAX, PWR_MID, PWR_LOW} pwrStateType;

typedef enum {SDA_PWR_MODE_NORMAL, SDA_PWR_MODE_SLEEP} pwrModeType;

typedef enum {SDA_PWR_MODE_SLEEP_LOW, SDA_PWR_MODE_SLEEP_NORMAL, SDA_PWR_MODE_SLEEP_DEEP} pwrSleepModeType;

typedef enum {POWER_USB, POWER_BATT} systemPwrType;

typedef enum {CLICKED_NONE, CLICKED_SHORT, CLICKED_LONG} clickType;

typedef enum {DEVICE_LOCKED, DEVICE_UNLOCKED} sdaDeviceLockType;

typedef enum {ON_SCREEN_KEYBOARD, OTHER} sdaInputMethodType;

typedef struct {
  //touch
  volatile uint8_t   touchValid;
  volatile uint16_t  touchX; //x pro dotyk
  volatile uint16_t  touchY; //y pro dotyk
  volatile gr2EventType touchType;

  //keys
  volatile gr2EventType keyEv[6];
  volatile uint8_t btnFlag;

  //battery
  volatile uint8_t battPercentage;
  uint8_t battString[6]; //3.654V např
  volatile systemPwrType pwrType;

  //sysled
  ledPatternType ledPattern;

  //LCDetect
  lcdOrientationType lcdOrientation;
  uint8_t lcdLandscape;
  lcdStateType lcdState;
  uint8_t lcdBacklight;
  uint32_t lcdShutdownTime;

  //RTC
  volatile uint8_t sec;
  volatile uint8_t min;
  volatile uint8_t hour;
  volatile uint8_t day;
  volatile uint8_t weekday;
  volatile uint8_t month;
  volatile uint16_t year;
  volatile uint8_t dateUpdated;
  volatile int32_t timestamp;

  //time
  volatile uint32_t uptime;
  volatile uint32_t lcdOnTime;
  volatile uint32_t battTime;
  volatile uint64_t uptimeMs;

  //sound
  volatile uint8_t mute;

  //keyboard
  volatile uint8_t   kbdFlag;
  volatile uint8_t   kbdVisible;
  uint8_t  kbdKeyStr[64];
  uint8_t  clipboard[SDA_CLIPBOARD_SIZE];
  uint16_t newStringIdFlag;
  uint16_t newString;

  //system
  volatile uint8_t   systemRedraw;
  volatile uint16_t  systemOverlay;
  volatile uint16_t  systemMessage;
  volatile uint8_t   systemXBtnVisible;
  volatile uint8_t   systemXBtnClick;
  volatile int32_t   systemXBtnTime;
  volatile clickType systemOptClick;
  volatile uint8_t   systemPwrLongPress;

  sdaInputMethodType inputMethod;

  volatile uint8_t breakP16Draw;

  volatile sdaDeviceLockType sdaDeviceLock;

  volatile pwrModeType  powerMode; // normal or sleep
  volatile pwrSleepModeType powerSleepMode;
  } svpStatusStruct;

//TODO: specify what should be part of os and what should be part of touch driver
#ifdef PC
typedef struct {
  float a;
  float b;
  float c;
  float d;
  float e;
} touchCalibDataStruct;
#endif

typedef struct {
  uint16_t pid;
  uint16_t parentPid;
  uint16_t prevPid;
  uint8_t  name[APP_NAME_LEN];
  uint8_t  currentWorkDir[APP_NAME_LEN];
  uint8_t  landscape;
  uint16_t screen;
  uint8_t  openFileName[SDA_FILES_OPEN_MAX][128];
  uint8_t  openFileUsed[SDA_FILES_OPEN_MAX];
  uint8_t  openConfName[128];
  uint8_t  openConfUsed;
  uint8_t  openCsvName[128];
  uint8_t  openCsvUsed;
  uint8_t  lcdOffButtons;
  uint8_t  launchFromCWD;
  uint64_t loadUptime;
  uint8_t  beepTimerCallback[15];
  uint32_t beepTime;
  uint8_t  authorized;
  uint8_t  useDrawRoot;
  uint8_t  drawRoot[128];

  varType  svmCallRetval[3];
  uint8_t* svmCallRetvalStr[3];
  uint8_t  svmCallRetvalType[3];
  uint8_t  svmCallback[NAME_LENGTH];
  uint8_t  suspendOnClose;
  uint8_t  uartCallback[NAME_LENGTH];
  uint8_t  initExecuted;
  uint8_t  suspendExecuted;
  uint8_t  kbdVisible;
} sdaSvmMetadata;

typedef struct {
  uint8_t  name[APP_NAME_LEN];
  uint8_t  cryptoUnlocked;
  uint16_t pid;
  uint8_t  valid;
  uint8_t  singular;
  uint8_t  uartCallbackEnabled;
} svmSavedProcType;


typedef struct {
  uint16_t overlayScr;
  gr2context * overlayCont;
  uint16_t overlayX1;
  uint16_t overlayX2;
  uint16_t overlayY1;
  uint16_t overlayY2;
  uint16_t ov_id;
  void (*overlay_destructor) ();
  uint8_t destructor_set;
} sdaOverlayType;


typedef enum {
  EXTERNAL_EXPANSION_PORT = 1,
  INTERNAL_EXPANSION_PORT = 2,
  SERIAL_PORT = 3,
  USB_PORT = 4
} sdaResource;

#endif
