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

#include "sda_svm_timers.h" 
#include "sda_svm_getset.h"
#include "sda_svm_misc.h"
#include "sda_svm_wake_suspend.h"

#define SUSPEND_FUNCTION (uint8_t *)"suspend"
#define WAKEUP_FUNCTION (uint8_t *)"wakeup"

// *****************************************************************************
//                                 standard API
// *****************************************************************************

// launches .svs file from APPS directory
uint8_t svmLaunch(uint8_t * fname, uint16_t parentPid);

// Set argument from out of SVM, should be called after svmLaunch
void svmSetArgumentNum(uint8_t id, int32_t val);
void svmSetArgumentStr(uint8_t id, uint8_t* val);
void svmSetArgumentFlt(uint8_t id, float val);

// if instance of some app is running, this switches to it
void svmOnTop();

// Gets if there is currently valid, or suspended valid svm 
uint8_t svmGetRunning();

// gently closes the currently running app, performs exit() call
void svmCloseRunning();

// kills currently running app
void svmKillRunning();

// performs update of currently running app
uint16_t svmRun(uint8_t init, uint8_t top);

// sets main screen from svs
void svmSetMainScreen(uint16_t val);
uint16_t svmGetMainScreen();

// svm redraw detect
void sdaSetRedrawDetect(uint8_t val);
uint8_t sdaGetRedrawDetect();

// set error
void svmSetError(uint8_t * str);

// cwd for launching child app
void svmSetLaunchCWDflag(uint8_t val);

// get name
uint8_t * svmGetName();

// lock/unlock crypto
void svmSetCryptoUnlock(uint8_t unlock);
uint8_t svmGetCryptoUnlock();

// get app uptime
uint64_t svmGetAppUptime();

// landscape/portrait
void svmSetLandscape(uint8_t val);

// Authorized app
void svmAuthorize();
uint8_t svmGetAuthorized();

// init svs and svm
void svmBootInit();

// multiprocess
void svmCallSubProc(
  uint8_t *name,
  uint8_t *callback,
  varType arg0, uint8_t type0,
  varType arg1, uint8_t type1,
  varType arg2, uint8_t type2
);

void svmSetSubProcRetval(
  varType arg0, uint8_t type0,
  varType arg1, uint8_t type1,
  varType arg2, uint8_t type2
);

// wake-suspend
// suspend currently running app
uint8_t svmSuspend();

// wake app with given pid
uint8_t svmWake(uint16_t pid);

// close app with given pid
void svmClose(uint16_t pid);

// Closes all apps, running or suspended
void svmCloseAll();

// Gets if there is currently valid, or suspended valid svm 
uint8_t svmGetRunning();

// Gets Pid of the currently running app
uint16_t svmGetPid();

// gets if svm contains valid running app
uint8_t svmGetValid();
// sets validity of currently running app
void svmSetValid(uint8_t val);

uint16_t svmGetSuspendedPid(uint16_t id);
uint8_t *svmGetSuspendedName(uint16_t id);

// Marks app with given Pid as singular
void svmSetSingular(uint16_t pid);

/*
Singular app has only one instance running,
if another instance is to be launched,
currently running instance is waken up instead 
*/

// get current wd relative to main dir
void svmUpdateCurrentWD();

// notification flags
void sda_alarm_set_flag(int32_t id, int32_t param);
int32_t sda_alarm_get_id();
int32_t sda_alarm_get_param();
int8_t sda_alarm_get_flag();
void sda_alarm_clear_flag();

// Pre-caching
// Precaches given svs file, returns 0 on success 
uint8_t svmPrecacheFile(uint8_t *fname);

// next svmLaunch will be forced to not use cache
void svmSetNocacheFlag();

// Enable/Disable autocache on app launch
void svmSetAutocahceEnable(uint8_t val);
// Get if auto-caching is enabled
uint8_t svmGetAutocahceEnable();
// Cleans the app cache
void svmRemovePrecache();

// *****************************************************************************
//                                 internal functions
// *****************************************************************************

uint8_t svmTokenizeFile(uint8_t *fname, uint8_t *name, uint8_t mode);
void svmGetGR2Settings();
void svmSetRestoreSlot(uint8_t slot);

// multiprocess internal functions
void svmSaveProcData();
uint8_t svmLoadProcData(uint16_t pid);

uint16_t svmGetIfSingular(uint8_t * name);

uint8_t svmCheckAndExit();

uint8_t svmGetSavedProcValid(uint16_t id);
uint16_t svmGetSavedProcPid(uint16_t id);
void svmSetNextPid(uint16_t pid);
void svmRemoveCachedProc(uint16_t pid);
void svmRemoveCachedFile(uint16_t pid, uint8_t * tail);
void svmInit();
uint8_t svmUpdatePath(uint8_t *newFname, uint8_t *oldFname);
void svmSetBeepCallback(uint8_t * cb, uint32_t time);
uint8_t svmBeepHandler();

#endif
