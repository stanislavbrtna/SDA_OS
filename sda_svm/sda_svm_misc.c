/*
Copyright (c) 2022 Stanislav Brtna

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

#include "sda_svm.h"
#include "sda_svm_misc.h"
#include "sda_svm_cache.h"

extern svsVM            svm;
extern sdaSvmMetadata   svmMeta;
extern svmSavedProcType svmSavedProc[MAX_OF_SAVED_PROC];
extern uint8_t          soft_error_flag;

extern uint8_t callback_arise_flag;


// beep callback init & handler
void svmBeepSetCallback(uint8_t * cb, uint32_t time) {
 sda_strcp(cb, svmMeta.beepTimerCallback, sizeof(svmMeta.beepTimerCallback));
 svmMeta.beepTime = (svpSGlobal.uptimeMs - svmGetAppUptime()) + time;
}


uint8_t svmBeepHandler() {
  if ((svpSGlobal.uptimeMs - svmGetAppUptime()) > svmMeta.beepTime && svmMeta.beepTime != 0) {
    svmMeta.beepTime = 0;
    return 1;
  } else {
    return 0;
  }
}

void svmInit() {
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS/cache");
  printf("Cleaning up cached apps.\n");
  svmInitRemoveCache((uint8_t *) "gr0");
  svmInitRemoveCache((uint8_t *) "gr1");
  svmInitRemoveCache((uint8_t *) "gr2");
  svmInitRemoveCache((uint8_t *) "met");
  svmInitRemoveCache((uint8_t *) "stc");
  svmInitRemoveCache((uint8_t *) "svm");
  svmInitRemoveCache((uint8_t *) "str");

  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");
  svmSetNextPid(1);
}

void svmThrowError(uint8_t* str) {
  if (svmGetValid()) {
    errSoft(str, &svm);
  }
}

void svmOnTop() {
  sda_slot_on_top(SDA_SLOT_SVM);
  svp_switch_main_dir();
  svp_chdir(svmMeta.currentWorkDir);

  if (svmMeta.landscape != svpSGlobal.lcdLandscape) {
    sda_set_landscape(svmMeta.landscape);
  }

  svpSGlobal.systemXBtnVisible = 1;
  svpSGlobal.systemXBtnClick = 0;
}


uint8_t svmCheckAndExit() {
  if((svpSGlobal.systemXBtnClick) || (errCheck(&svm) != 0)) {
    svmCloseRunning();
    return 1;
  }
  return 0;
}

// registers serial receive callback for current app, returns 0 when ok
uint8_t svmRegisterUartCallback(uint8_t* callback, uint8_t val) {
  // check stored proc for empty callback
  for(uint16_t i = 0; i < MAX_OF_SAVED_PROC; i++) {
    if (svmSavedProc[i].uartCallbackEnabled == 1 && svmSavedProc[i].valid) {
      return 1;
    }
  }

  // register callback
  for(uint16_t i = 0; i < MAX_OF_SAVED_PROC; i++) {
    if (svmSavedProc[i].pid == svmGetPid()) {
      svmSavedProc[i].uartCallbackEnabled = val;
      // set callback name
      sda_strcp(callback, svmMeta.uartCallback, NAME_LENGTH);
    }
  }

  return 0;
}


uint8_t svmHandleUartCallbacks() {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmGetSavedProcValid(x) == 1) {
      if (svmSavedProc[x].uartCallbackEnabled && svmSavedProc[x].uartCallbackEnabled == sda_serial_get_rdy()) {
        // App is already on top
        if (svmGetSavedProcPid(x) == svmMeta.pid) {
          //execute
          commExec(svmMeta.uartCallback, &svm);
          if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
            svp_errSoftPrint(&svm);
            return 1;
          }
          if (svmCheckAndExit()) { // handle potential exit call
            return 0;
          }

          if (callback_arise_flag == 1) {
            svmOnTop();
            callback_arise_flag = 0;
          }

        } else {
          uint16_t prev_pid;
          if (svmGetValid()) {
            prev_pid = svmMeta.pid;
          } else {
            prev_pid = 0;
          }
          
          //wakeup
          if(svmWake(svmGetSavedProcPid(x))) {
            // error occured during wakeup
            // wake the previous and exit
            if (prev_pid) {
              svmWake(prev_pid);
              svmOnTop();
              setRedrawFlag();
            } else {
              // TODO: fix all of this slot mess
              svp_switch_main_dir();
              svp_chdir((uint8_t *)"APPS");
              sda_slot_on_top(SDA_SLOT_APPLIST);
            }
            
            return 0;
          }

          //execute
          commExec(svmMeta.uartCallback, &svm);
          if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
            svp_errSoftPrint(&svm);
            return 1;
          }

          // handle potential exit call
          if (svmCheckAndExit()) {
            return 0;
          }

          //go back
          if (callback_arise_flag == 0) {
            svmWake(prev_pid);
            return 0;
          }
          callback_arise_flag = 0;
          svmOnTop();
          setRedrawFlag();
        }
      }
    }
  }
  return 0;
}


// get current wd relative to main dir
void svmUpdateCurrentWD() {
  uint8_t dirbuf[258];
  uint8_t path[258];
  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();
  svp_getcwd(path, 256);

  if (dirbuf[sda_strlen(path)] == '/') {
    sda_strcp(dirbuf + sda_strlen(path) + 1, svmMeta.currentWorkDir, sizeof(svmMeta.currentWorkDir));
  } else {
    sda_strcp(dirbuf + sda_strlen(path), svmMeta.currentWorkDir, sizeof(svmMeta.currentWorkDir));
  }

  svp_chdir(svmMeta.currentWorkDir);
}


uint8_t svmUpdatePath(uint8_t *newFname, uint8_t *oldFname) {
  uint8_t dirbuf[258];

  svp_getcwd(dirbuf, 256);
  newFname[0] = 0;

  for (uint16_t i = 0; i < sizeof(dirbuf); i++) {
    if ((i < (sizeof(dirbuf) - 4)) && dirbuf[i] == 'A' && dirbuf[i+1] == 'P' && dirbuf[i+2] == 'P' && dirbuf[i+3] == 'S') {
      i += 3;
      if (dirbuf[i+1] != 0) { // if we are in apps root folder, we don't need the slash
        sda_strcp(dirbuf + i + 2, newFname, APP_NAME_LEN);
        sda_strcp((uint8_t *)"/", newFname + sda_strlen(newFname), APP_NAME_LEN);
      }
      sda_strcp(oldFname, newFname + sda_strlen(newFname), APP_NAME_LEN);
#ifdef SVM_DBG_ENABLED
      printf("%s: 0: updating path: %s -> %s \n",__FUNCTION__ , oldFname, newFname);
#endif
      return 0;
    }
  }
#ifdef SVM_DBG_ENABLED
      printf("%s: 1: APPS not in path: %s\n",__FUNCTION__ , oldFname, newFname);
#endif
  // APPS string not in path...
  return 1;
}
