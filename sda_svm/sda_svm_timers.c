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

#include "sda_svm_timers.h"

extern svsVM          svm;
extern sdaSvmMetadata svmMeta;

extern uint8_t soft_error_flag;

uint8_t callback_arise_flag;
static uint32_t svmSavedProcTimer[MAX_OF_SAVED_PROC];
static uint8_t  svmSavedProcTimerCallback[MAX_OF_SAVED_PROC][NAME_LENGTH];

// timers

void sdaSvmSetTimer(uint32_t time_ms, uint8_t *callback) {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmGetSavedProcPid(x) == svmMeta.pid && svmGetSavedProcValid(x) == 1) {
      svmSavedProcTimer[x] = svpSGlobal.uptimeMs + time_ms;
      sda_strcp(callback, svmSavedProcTimerCallback[x], sizeof(svmSavedProcTimerCallback[x]));
      //printf("setting %u, %s\n", svmSavedProcTimer[x], svmSavedProcTimerCallback[x]);
    }
  }
}


void sdaSvmClearTimer() {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmGetSavedProcPid(x) == svmMeta.pid && svmGetSavedProcValid(x) == 1) {
      svmSavedProcTimer[x] = 0;
    }
  }
}


uint8_t sdaSvmHandleTimers() {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmGetSavedProcValid(x) == 1) {
      //printf("checking %u, %u\n", svpSGlobal.uptimeMs, svmSavedProcTimer[x]);
      if (svmSavedProcTimer[x] <= svpSGlobal.uptimeMs && svmSavedProcTimer[x] != 0) {
        //printf("triggered!\n");
        svmSavedProcTimer[x] = 0; // reset
        if (svmGetSavedProcPid(x) == svmMeta.pid) {
          //execute
          commExec(svmSavedProcTimerCallback[x], &svm);
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
          commExec(svmSavedProcTimerCallback[x], &svm);
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
  return 1;
}


void sdaSvmSetAriseFlag() {
  callback_arise_flag = 1;
}


uint8_t sdaSvmIsTimerSet() {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProcTimer[x] != 0 && svmGetSavedProcValid(x) == 1) {
      return 1;
    }
  }
  return 0;
}
