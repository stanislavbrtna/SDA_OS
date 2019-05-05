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

#ifndef SDA_SVM_H
#define SDA_SVM_H
#include "../SDA_OS.h"
#define SUSPEND_FUNCTION (uint8_t *)"suspend"
#define WAKEUP_FUNCTION (uint8_t *)"wakeup"

// *****************************************************************************
//                                 standard API
// *****************************************************************************

// launches .svs file from APPS directory
uint8_t sdaSvmLaunch(uint8_t * fname, uint16_t parentId);

// if instance of some app is running, this switches to it
void sdaSvmOnTop();

// returns non-zero if some app is currently loaded
uint8_t sdaSvmGetRunning();

// gently closes the currently running app, performs exit() call
void sdaSvmCloseApp();

// kills app
void sdaSvmKillApp();

// performs update of currently running app
uint16_t sdaSvmRun(uint8_t init, uint8_t top);

// sets main screen from svs
void sdaSvmSetMainScreen(uint16_t val);
uint16_t sdaSvmGetMainScreen();

// misc
void sdaSetRedrawDetect(uint8_t val);
uint8_t sdaGetRedrawDetect();
void sdaSvmSetError(uint8_t * str);
void svmSetLaunchCWDflag(uint8_t val);

// multiprocess
void sdaSvmSave();
uint8_t sdaSvmLoad(uint16_t id);
void sdaSvmCall(
		uint8_t *name,
		uint8_t *callback,
		varType arg0, uint8_t type0,
		varType arg1, uint8_t type1,
		varType arg2, uint8_t type2
	);
void sdaSvmRetval(
		varType arg0, uint8_t type0,
		varType arg1, uint8_t type1,
		varType arg2, uint8_t type2
	);

// wake-suspend
uint8_t svmSuspend();
uint8_t svmWake(uint16_t id);
void svmClose(uint16_t id);
void svmCloseAll();
void svmSetSingular(uint16_t id);
uint8_t sdaSvmGetRunning();
uint16_t sdaSvmGetId();
uint16_t svmGetSuspendedId(uint16_t id);
uint8_t *svmGetSuspendedName(uint16_t id);
void sdaUpdateCurrentWD();

// notification flags
void setNotificationFlag(int32_t id, int32_t param);
int32_t getNotificationId();
int32_t getNotificationParam();
int8_t getNotificationFlag();
void clearNotificationFlag();

// *****************************************************************************
//                                 internal functions
// *****************************************************************************

uint8_t sdaSvmLoadApp(uint8_t *fname, uint8_t *name, uint8_t mode);
void sdaSvmGetGR2Settings();

#endif
