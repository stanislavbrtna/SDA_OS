/*
Copyright (c) 2024 Stanislav Brtna

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

#include "sda_svm_callbacks.h"

extern uint8_t callback_arise_flag;

extern svsVM            svm;
extern sdaSvmMetadata   svmMeta;
extern svmSavedProcType svmSavedProc[MAX_OF_SAVED_PROC];
extern uint8_t          soft_error_flag;

// registers serial receive callback for current app, returns 0 when ok
// callback == 0 - no callback
// callback == 1 - callback on every character
// callback == 2 - callback on end of line character
uint8_t svmRegisterUartCallback(uint8_t* callback, uint8_t val) {
  // register callback
  for(uint16_t i = 0; i < MAX_OF_SAVED_PROC; i++) {
    if (svmSavedProc[i].pid == svmGetPid() && svmSavedProc[i].valid) {
      svmSavedProc[i].uartCallbackEnabled = val;
      // set callback name
      sda_strcp(callback, svmMeta.uartCallback, NAME_LENGTH);
      return 0;
    }
  }

  return 1;
}


uint8_t svmGetUartCallbackActive() {
  for(uint16_t i = 0; i < MAX_OF_SAVED_PROC; i++) {
    if (svmSavedProc[i].uartCallbackEnabled && svmSavedProc[i].valid) {
      return 1;
    }
  }
  return 0;
}


uint8_t svmHandleUartCallbacks() {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmGetSavedProcValid(x) == 1) {
      if (svmSavedProc[x].uartCallbackEnabled
          && sda_serial_get_rdy() > 0
          && svmSavedProc[x].uartCallbackEnabled <= sda_serial_get_rdy()
      ) {
        return svmGenericCallback(svmGetSavedProcPid(x), svmMeta.uartCallback);
      }
    }
  }
  return 0;
}


uint8_t svmGenericCallback(uint16_t pid, uint8_t *callback) {
  // App is already on top
  if (pid == svmMeta.pid) {
    //execute
    commExec(callback, &svm);
    
    if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
      svp_errSoftPrint(&svm);
      return 1;
    }

    // handle potential exit call
    if (svmCheckAndExit()) { 
      return 0;
    }

    if (callback_arise_flag == 1) {
      svmMeta.suspendExecuted = 0;
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

    uint16_t top_prev_pid = 0;
    if(sda_get_top_slot() == SDA_SLOT_SVM) {
      top_prev_pid = prev_pid;
    }
    
    //wakeup
    if(svmWake(pid)) {
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

    svmMeta.prevPid = top_prev_pid;

    //execute
    commExec(callback, &svm);
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

    svmMeta.suspendExecuted = 0;
    callback_arise_flag = 0;
    svmOnTop();
    setRedrawFlag();
  }
}
