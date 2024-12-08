/*
Copyright (c) 2023 Stanislav Brtna

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

#include "sda_svm_getset.h"

static uint8_t redrawDetect;

// svs
uint8_t  svs_wrap_setScr_flag;

static uint8_t suspend_flag;

extern svmSavedProcType svmSavedProc[MAX_OF_SAVED_PROC];
extern sdaSvmMetadata   svmMeta;
extern svsVM            svm;

extern uint8_t *pscgErrorString;
extern uint8_t  autocahceEnable;
extern uint8_t  nocacheLaunchFlag;

// sets default values for new svmMeta instance
void svmLaunchSetDefMetadata(uint16_t pid, uint16_t parentPid, uint8_t *fname) {
  svmMeta.pid = pid;
  svmMeta.parentPid = parentPid;
  
  sda_strcp(fname, svmMeta.name, sizeof(svmMeta.name));
  
  for(uint16_t i = 0; i < SDA_FILES_OPEN_MAX; i++) {
    svmMeta.openFileUsed[i] = 0;
    sda_strcp((uint8_t *)"", svmMeta.openFileName[i], sizeof(svmMeta.openFileName[i]));
  }
  
  sda_strcp((uint8_t *)"", svmMeta.openConfName, sizeof(svmMeta.openConfName));
  sda_strcp((uint8_t *)"", svmMeta.openCsvName, sizeof(svmMeta.openCsvName));
  sda_strcp((uint8_t *)"", svmMeta.openDbName, sizeof(svmMeta.openDbName));
  sda_strcp((uint8_t *)"DATA", svmMeta.currentWorkDir, sizeof(svmMeta.currentWorkDir));
  sda_strcp((uint8_t *)"DATA", svmMeta.drawRoot, sizeof(svmMeta.drawRoot));
  svmMeta.openConfUsed    = 0;
  svmMeta.openCsvUsed     = 0;
  svmMeta.openDbUsed      = 0;
  svmMeta.loadUptime      = svpSGlobal.uptimeMs;
  svmMeta.landscape       = svpSGlobal.lcdLandscape;
  svmMeta.lcdOffButtons   = 0;
  svmMeta.launchFromCWD   = 0;
  svmMeta.beepTime        = 0;
  svmMeta.authorized      = 0;
  svmMeta.useDrawRoot     = 0;
  svmMeta.suspendOnClose  = 0;
  svmMeta.suspendExecuted = 0;
  svmMeta.initExecuted    = 0;
  svmMeta.screen          = 0;
  svmMeta.kbdVisible      = 0;

  for (uint16_t i = 0; i < 3; i++) {
    svmMeta.svmCallRetval[i].val_u = 0;
    svmMeta.svmCallRetvalType[i]   = SVS_TYPE_UNDEF;
  }
}

void svmSetAutocahceEnable(uint8_t val) {
  autocahceEnable = val;
}

uint8_t svmGetAutocahceEnable() {
  return autocahceEnable;
}

void svmSetNocacheFlag() {
  nocacheLaunchFlag = 1;
}

uint8_t svmGetSavedProcValid(uint16_t proc_array_index) {
  return svmSavedProc[proc_array_index].valid;
}

uint16_t svmGetSavedProcPid(uint16_t id) {
  return svmSavedProc[id].pid;
}

uint8_t * svmGetName() {
  return svmMeta.name;
}

// Crypto lock/unlock 
void svmSetCryptoUnlock(uint8_t unlock) {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].pid == svmMeta.pid) {
      svmSavedProc[x].cryptoUnlocked = unlock;
    }
  }
}

uint8_t svmGetCryptoUnlock() {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].pid == svmMeta.pid) {
      return svmSavedProc[x].cryptoUnlocked;
    }
  }
  return 0;
}

// return: process id, -1 if not valid
int16_t svmGetId(uint16_t pid) {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].pid == pid && svmSavedProc[x].valid) {
      return x;
    }
  }

  return -1;
}

// Gets if there is currently valid, or suspended valid svm 
uint8_t svmGetRunning() {
  if (svmGetValid()) {
    return 1;
  }

  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].valid == 1) {
      return 1;
    }
  }
  return 0;
}

uint8_t svmGetValidPid(uint16_t pid) {
  if (svmMeta.pid == pid && svmGetValid()){
    return 1;
  }
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].pid == pid) {
      return svmSavedProc[x].valid;
    }
  }
  return 0;
}

uint16_t svmGetIfSingular(uint8_t * name) {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].valid == 1) {
      if (strCmp(svmSavedProc[x].name, name) && svmSavedProc[x].singular) {
        return svmSavedProc[x].pid;
      }
    }
  }
  return 0;
}

void svmSetSingular(uint16_t pid) {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].valid == 1 && svmSavedProc[x].pid == pid) {
      svmSavedProc[x].singular = 1;
    }
  }
}

void svmInValidate(uint16_t pid) {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].pid == pid && svmSavedProc[x].valid == 1) {
      svmSavedProc[x].valid = 0;
      svmRemoveCachedProc(pid);
#ifdef SVM_DBG_ENABLED
      printf("svmInValidate: removing cached proc x:%u id:%u\n", x, id);
#endif
    }
  }
}

void svmKillRunning() {
  svm.handbrake = 1;
}

void svmSetError(uint8_t * str) {
  pscgErrorString = str;
}

void svmSetLaunchCWDflag(uint8_t val) {
  svmMeta.launchFromCWD = val;
}

uint64_t svmGetAppUptime() {
  return svmMeta.loadUptime;
}

void svmAuthorize() {
  svmMeta.authorized = 1;
}

uint8_t svmGetAuthorized() {
  return svmMeta.authorized;
}

void svmSetSuspendOnClose(uint8_t val) {
  svmMeta.suspendOnClose = val;
}

uint8_t svmGetSuspendOnClose() {
  return svmMeta.suspendOnClose;
}


void sdaSetRedrawDetect(uint8_t val) {
  redrawDetect = val;
}

uint8_t sdaGetRedrawDetect() {
  return redrawDetect;
}

void svmGetGR2Settings() {
  // load colors from system to app context
  gr2_set_border_color(gr2_get_border_color(&sda_sys_con), &sda_app_con);
  gr2_set_text_color(gr2_get_text_color(&sda_sys_con), &sda_app_con);
  gr2_set_background_color(gr2_get_background_color(&sda_sys_con), &sda_app_con);
  gr2_set_fill_color(gr2_get_fill_color(&sda_sys_con), &sda_app_con);
  gr2_set_active_color(gr2_get_active_color(&sda_sys_con), &sda_app_con);
}

uint16_t svmGetPid() {
  if (svmGetValid()) {
    return svmMeta.pid;
  } else {
    return 0;
  }
}

void svmSetLandscape(uint8_t val) {
  svmMeta.landscape = val;
}

void svmSetMainScreen(uint16_t val) {
  svmMeta.screen = val;
  svs_wrap_setScr_flag = 1;
  gr2_set_modified(val, &sda_app_con);
}

void svmSetDrawRoot(uint8_t * str) {
  sda_strcp(str, svmMeta.drawRoot, sizeof(svmMeta.drawRoot));
  svmMeta.useDrawRoot = 1;
}

void svmSetSuspendFlag(uint8_t val) {
  suspend_flag = val;
}

uint8_t svmGetSuspendFlag() {
  return suspend_flag;
}

