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
#include "sda_types_hw.h"


typedef struct {
  // Touch
  volatile uint8_t       touchValid;
  volatile uint16_t      touchX;
  volatile uint16_t      touchY;
  volatile gr2EventType  touchType;

  // Keypad
  volatile gr2EventType  keyEv[6];
  volatile uint8_t       btnFlag;

  // Battery
  volatile uint8_t       battPercentage;
  uint8_t                battString[6]; // 3.654V for example 
  volatile systemPwrType pwrType;

  // Power mode
  volatile pwrModeType      powerMode;
  volatile pwrSleepModeType powerSleepMode;
  volatile sdaPwrModeType   pwrClockScalingMode;
  volatile uint16_t         systemClock; // clock speed in mhz

  // Status LED
  ledPatternType ledPattern;

  // Screen
  lcdOrientationType lcdOrientation;
  lcdStateType       lcdState;
  uint8_t  lcdLandscape;
  volatile uint8_t  lcdBacklight;
  uint32_t lcdShutdownTime;
  volatile uint16_t ambLightValue; // voltage on two decimal places
  volatile uint8_t  autoBacklight;
  volatile uint8_t  autoBacklightOverride;

  // RTC
  volatile uint8_t  sec;
  volatile uint8_t  min;
  volatile uint8_t  hour;
  volatile uint8_t  day;
  volatile uint8_t  weekday;
  volatile uint8_t  month;
  volatile uint16_t year;
  volatile uint8_t  dateUpdated;
  volatile int32_t  timestamp;

  // Time
  volatile uint32_t uptime;
  volatile uint32_t lcdOnTime;
  volatile uint32_t battTime;
  volatile uint64_t uptimeMs;

  // Notifications
  volatile uint8_t          mute;
  volatile uint8_t          haptics;
  volatile uint16_t         volumeSpeaker;
  volatile uint16_t         volumeHeadphones;
  volatile sdaPCMOutputType outputPCM;

  // Keyboard, clipboard, text input method
  volatile uint8_t   kbdFlag;
  volatile uint8_t   kbdVisible;
  sdaInputMethodType inputMethod;
  uint8_t  kbdKeyStr[64];
  uint8_t  clipboard[SDA_CLIPBOARD_SIZE];
  uint16_t newStringIdFlag;
  uint16_t newString;

  // System
  volatile uint8_t   systemRedraw;
  volatile uint16_t  systemOverlay;
  volatile uint16_t  systemMessage;
  volatile uint8_t   systemXBtnVisible;
  volatile uint8_t   systemXBtnClick;
  volatile int32_t   systemXBtnTime;
  volatile clickType systemOptClick;
  volatile uint8_t   systemPwrLongPress;

  // Misc
  volatile uint8_t breakP16Draw;
  volatile sdaDeviceLockType sdaDeviceLock;

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
  uint8_t  openDbName[128];
  uint8_t  openDbTable[16];
  uint8_t  openDbUsed;
  uint8_t  lcdOffButtons;
  uint8_t  launchFromCWD;
  uint64_t loadUptime;
  uint8_t  beepTimerCallback[15];
  uint32_t beepTime;
  uint8_t  authorized;
  uint8_t  useDrawRoot;
  uint8_t  drawRoot[128];
  uint8_t  calWidgetUsed;

  varType  svmCallRetval[3];
  uint8_t* svmCallRetvalStr[3];
  uint8_t  svmCallRetvalType[3];
  uint8_t  svmCallback[NAME_LENGTH];
  uint8_t  suspendOnClose;
  uint8_t  uartCallback[NAME_LENGTH];
  uint8_t  initExecuted;
  uint8_t  suspendExecuted;
  uint8_t  kbdVisible;
  uint8_t  cryptoKey;
} sdaSvmMetadata;

typedef struct {
  uint8_t  name[APP_NAME_LEN];
  uint8_t  niceName[SDA_NICE_NAME];
  uint8_t  cryptoUnlocked;
  uint16_t pid;
  uint8_t  valid;
  uint8_t  singular;
  uint8_t  uartCallbackEnabled;
  uint8_t  screenShdnLock;
  uint8_t  sleepLock;
  uint8_t  standbyLock;
} svmSavedProcType;


typedef struct {
  uint16_t overlayScr;
  gr2context *overlayCont;
  uint16_t overlayX1;
  uint16_t overlayX2;
  uint16_t overlayY1;
  uint16_t overlayY2;
  uint16_t ov_id;
  void (*overlay_destructor) ();
  uint8_t destructor_set;
} sdaOverlayType;

#endif
