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

#include "sda_svm.h"

extern svsVM            svm;
extern sdaSvmMetadata   svmMeta;
extern svmSavedProcType svmSavedProc[MAX_OF_SAVED_PROC];

extern uint8_t  soft_error_flag;

static uint8_t slot_restore; // what appslot to restore after close

void svmSetRestoreSlot(uint8_t slot) {
  slot_restore = slot;
}

// gracefully closes running svm (app)
void svmCloseRunning() {
  uint8_t errorOccured = 0;
  // printf("SVM Close running called...\n");
  // clear the close flag
  svpSGlobal.systemXBtnClick = 0;

  if (sda_slot_get_valid(SDA_SLOT_SVM) == 0 || svmGetValid() == 0) {
    return;
  }

  if((errCheck(&svm) == 1) && (soft_error_flag == 0)) {
    svp_errSoftPrint(&svm);
    errorOccured = 1;
  } else {
    if ((getOverlayId() != 0) && (soft_error_flag == 0)) {
      destroyOverlay();
    }
  }
  
  // just suspend instead of closing
  if(svmGetSuspendOnClose() && (errCheck(&svm) == 0 && soft_error_flag == 0)) {
    svmSuspend();
    return;
  }

  if (sda_slot_get_valid(SDA_SLOT_SVM) == 1) {
    if(functionExists(SVM_EXIT_FUNCTION, &svm)) {
      commExec(SVM_EXIT_FUNCTION, &svm);
      if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
        svp_errSoftPrint(&svm);
      }
    }
  }

  if (svmGetCryptoUnlock()) {
    svp_crypto_lock();
  }

  SVScloseCache(&svm);
  svmInValidate(svmMeta.pid);
  sda_set_sleep_lock(0);
  svpSGlobal.kbdVisible = 0;
  sda_set_landscape(0);
  sdaAlarmClearFlag();
  sda_files_close();
  sda_resource_free_pid(svmMeta.pid);
  sda_custom_icon_release_pid(svmMeta.pid);

  if (svmMeta.parentPid != 0) {
    //printf("waking up parent from close!\n");
    if (svmLoadParent(errorOccured) == 0) {
      // return from exit, continue executing parent app
      return;
    }
    //printf("close: parent failed to wake\n");
    // else: parent failed to load, continue with exit...
  }

  gr2_text_deactivate(&sda_app_con);
  svpSGlobal.systemXBtnVisible = 0;
  svmSetValid(0);
  if (sda_slot_get_screen(SDA_SLOT_SVM) != 0) {
    gr2_destroy(sda_slot_get_screen(SDA_SLOT_SVM), &sda_app_con);
  }
  sda_slot_set_invalid(SDA_SLOT_SVM);

  // TODO: restore right slot.. previous app perhaps...
  if (slot_restore == 0) {
    sda_slot_on_top(SDA_SLOT_APPLIST);
  }

  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");
}


void svmCloseAll() {
  svmSetRestoreSlot(sda_get_top_slot());

  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].valid == 1) {
      svmClose(svmSavedProc[x].pid, 0);
    }
  }

  if (sda_serial_is_enabled() && sda_resource_get_lock(SERIAL_PORT, 0) == SDA_LOCK_UNLOCKED) {
    sda_serial_disable();
  }

  if (slot_restore != 0) {
    if (sda_slot_get_valid(slot_restore)) {
      sda_slot_on_top(slot_restore);
    } else {
      sda_slot_on_top(SDA_SLOT_HOMESCREEN);
    }
    slot_restore = 0;
  }
}


// closes app with given id
void svmClose(uint16_t id, uint8_t force_unclosable) {
  uint16_t prevApp = 0;

  if (svmMeta.pid != id && sda_get_top_slot() == 4) {
    prevApp = svmMeta.pid;
  }

  if (svmMeta.pid != id) {
    svmWake(id);
  }

  if (force_unclosable) {
    // here we close even the unclosable apps
    svmSetSuspendOnClose(0);
  }

  svmCloseRunning();
  
  if (prevApp) {
    svmWake(prevApp);
  }
}
