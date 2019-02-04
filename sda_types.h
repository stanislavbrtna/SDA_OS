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

typedef enum {PRESSED, HOLD,LONGHOLD, RELEASED} eventType;

typedef enum {LED_ON, LED_OFF ,LED_BLINK, LED_SHORTBLINK, LED_ALARM} ledPatternType;

typedef enum {LCD_ON, LCD_OFF} lcdStateType;

typedef enum {SDA_LOCK_LOCKED, SDA_LOCK_UNLOCKED} sdaLockState;

typedef enum {PWR_MAX, PWR_MID, PWR_LOW} pwrStateType;

typedef enum {SDA_PWR_MODE_NORMAL, SDA_PWR_MODE_SLEEP} pwrModeType;

typedef enum {POWER_USB, POWER_BATT} systemPwrType;

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

	//sound
	volatile uint8_t mute;

	//keyboard
	volatile uint8_t   kbdFlag;
	uint8_t kbdKeyStr[10];
	volatile uint8_t   kbdVisible;

	//system
	volatile uint8_t systemRedraw;
	volatile uint16_t systemOverlay;
	volatile uint16_t systemMessage;
	volatile uint8_t systemXBtnVisible;
	volatile uint8_t systemXBtnClick;
	volatile uint8_t systemOptClick;
	volatile uint8_t systemPwrLongPress;

  volatile pwrModeType  powerMode; // normal or sleep
  volatile pwrStateType powerState; // specifies for how long the sda waits for input
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
	uint16_t id;
	uint16_t parentId;
	uint8_t name[APP_NAME_LEN];
	uint8_t currentWorkDir[APP_NAME_LEN];
	uint16_t screen;
	uint8_t openFileName[64];
	uint8_t openFileUsed;
	uint8_t openConfName[64];
	uint8_t openConfUsed;
	uint8_t openCsvName[64];
	uint8_t openCsvUsed;
	uint8_t lcdOffButtons;
	uint8_t launchFromCWD;
} sdaSvmMetadata;

#endif
