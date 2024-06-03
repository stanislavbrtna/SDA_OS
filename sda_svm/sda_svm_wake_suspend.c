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

#include "sda_svm_wake_suspend.h"

extern svsVM             svm;
extern sdaSvmMetadata    svmMeta;
extern svmSavedProcType  svmSavedProc[MAX_OF_SAVED_PROC];
extern uint8_t           soft_error_flag;


uint16_t svmGetSuspendedPid(uint16_t id) { 
  if (svmSavedProc[id].valid == 1) {
    return svmSavedProc[id].pid;
  }
  return 0;
}


uint8_t *svmGetSuspendedName(uint16_t id) {
  if (svmSavedProc[id].valid == 1) {
    return svmSavedProc[id].name;
  }
  return (uint8_t *)"";
}

void svmSuspendInitPid(uint16_t pid, uint8_t * name) {
  uint16_t index = 0;
  while (svmSavedProc[index].valid != 0) {
    if (index == MAX_OF_SAVED_PROC - 1) {
      return;
    }
    index++;
  }

  sda_strcp(name, svmSavedProc[index].name, APP_NAME_LEN);
  svmSavedProc[index].pid = pid;
  svmSavedProc[index].valid = 1;
  svmSavedProc[index].singular = 0;
  svmSavedProc[index].cryptoUnlocked = 0;
  svmSavedProc[index].uartCallbackEnabled = 0;
}

// execs the suspend function, result: 0 - ok, 1 - app exited, 2 - svs error occured
uint8_t svmExecSuspend() {
  // marks that suspend was executed only once
  if (svmMeta.suspendExecuted == 1) {
    return 0;
  }
  svmMeta.suspendExecuted = 1;

  if(functionExists(SVM_SUSPEND_FUNCTION, &svm)) {
    commExec(SVM_SUSPEND_FUNCTION, &svm);
    if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
      svp_errSoftPrint(&svm);
      return 2;
    }
    if (svmCheckAndExit()) {
      return 1;
    }
  }

  return 0;
}


void svmHandleHomeButton() {
  svmMeta.kbdVisible = svpSGlobal.kbdVisible;
  svmExecSuspend();
}


uint8_t svmStoreRunning() {
  uint8_t r = svmExecSuspend();

  if (r == 2) return 1; // error occured
  if (r == 1) return 0; // sys.os.exit called

  if (svmGetCryptoUnlock()) {
    svp_crypto_lock();
  }
  // reset the flag
  svmMeta.suspendExecuted = 0;
  if(sda_get_top_slot() == SDA_SLOT_SVM) {
    svmMeta.kbdVisible = svpSGlobal.kbdVisible;
  }
  
  svmSaveProcData();
  return 0;
}


void svmSuspend() {
  if (svmMeta.parentPid != 0) {
    uint8_t errorOccured = 0;
    
    // zero pid before saving
    uint16_t parentPid = svmMeta.parentPid;
    svmMeta.parentPid = 0;
    // save
    svmStoreRunning();

    // reload pid, so load parent works as expected
    svmMeta.parentPid = parentPid;

    if((errCheck(&svm) == 1) && (soft_error_flag == 0)) {
      svp_errSoftPrint(&svm);
      errorOccured = 1;
    } else {
      if ((getOverlayId() != 0) && (soft_error_flag == 0)) {
        destroyOverlay();
      }
    }

    if (svmMeta.parentPid != 0) {
      svmLoadParent(errorOccured);
    }

    return;
  } else {
    svmExecSuspend();
    gr2_text_deactivate(&sda_app_con);
    svpSGlobal.systemXBtnClick = 0;
    svpSGlobal.systemXBtnVisible = 0;
    
    if (getOverlayId() != 0) {
      destroyOverlay();
    }

    if (svmMeta.prevPid != 0) {
      svmStoreRunning(); // store currently running app...
      svmLoadPrevious();
      return;
    }

    sda_set_landscape(0);
    sda_keyboard_hide();

    sda_slot_on_top(SDA_SLOT_APPLIST);
    svp_switch_main_dir();
    svp_chdir((uint8_t *)"APPS");
    sda_set_sleep_lock(0);
  }
}

// wakes up given pid, returns: 0 - ok, 1 - error
// TODO: rewrite whis mess
uint8_t svmWake(uint16_t pid) {
  if(!(pid == svmMeta.pid && svmGetValid())) {
    if (svmGetValid()) {
      svmStoreRunning();
    }

    int16_t id = svmGetId(pid);

    if(id < 0) {
      printf("svmWake: id not valid\n");
      svmSavedProc[id].valid = 0;
      return 1;
    } 

    if (svmLoadProcData(pid) == 0) {
      printf("svmWake: error while loading app (1)\n");
      svmSavedProc[id].valid = 0;
      return 1;
    }
    sda_slot_set_valid(SDA_SLOT_SVM);
    svmSetValid(1);
  }

  svmOnTop();

  // unlock
  if (svmGetCryptoUnlock()) {
    svp_crypto_unlock_nopass();
  }

  // reset suspend executed flag
  svmMeta.suspendExecuted = 0;
  svpSGlobal.kbdVisible = svmMeta.kbdVisible;

  // exec wakeup
  if(functionExists(SVM_WAKEUP_FUNCTION, &svm)) {
    commExec(SVM_WAKEUP_FUNCTION, &svm);
    if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
      svp_errSoftPrint(&svm);
      return 1;
    }
    if (svmCheckAndExit()) {
      return 0;
    }
  }
    
  return 0;
}


// wakes up given pid, fills the correct arg structure. returns: 0 - ok, 1 - error 
uint8_t svmWakeArgs(uint16_t pid, uint8_t* argType, varType *arg, uint8_t **svmArgs) {
  uint16_t parent = 0;
  parent = svmMeta.pid;
  if(!(pid == svmMeta.pid && svmGetValid())) {
    if (svmGetValid()) {
      svmStoreRunning();
    }

    int16_t id = svmGetId(pid);

    if(id < 0) {
      printf("svmWake: id not valid\n");
      return 1;
    } 

    if (svmLoadProcData(pid) == 0) {
      printf("svmWake: error while loading app (1)\n");
      svmSavedProc[id].valid = 0;
      return 1;
    }
    svmMeta.parentPid = parent;
    sda_slot_set_valid(SDA_SLOT_SVM);
    svmSetValid(1);
  }

  svmOnTop();

  if (svmGetCryptoUnlock()) {
    svp_crypto_unlock_nopass();
  }
  
  svmRestoreArguments(argType, arg, svmArgs, &svm);
  if(functionExists(SVM_WAKEUP_FUNCTION, &svm)) {
    commExec(SVM_WAKEUP_FUNCTION, &svm);
    if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
      svp_errSoftPrint(&svm);
      return 1;
    }
    if (svmCheckAndExit()) {
      return 0;
    }
    svmClearArguments(&svm);
  }

  return 0;
}

