/*
Copyright (c) 2018-2023 Stanislav Brtna

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
#include "sda_svm_cache.h"
#include "sda_svm_misc.h"
#include "sda_svm_subproc.h"
#include "sda_svm_wake_suspend.h"

//svs VM
svsVM          svm;
sdaSvmMetadata svmMeta;
uint8_t        svmStrings[STRING_FIELD_L];

static uint16_t nextPid;
static uint8_t  svmValid;

svmSavedProcType svmSavedProc[MAX_OF_SAVED_PROC];

extern uint16_t svsLoadCounter;
extern uint8_t  soft_error_flag;

extern gr2context sda_app_con;

extern uint8_t  svs_wrap_setScr_flag;
extern uint16_t svs_wrap_setScr_id;

static uint8_t slot_restore; // what appslot to restore after close

uint8_t autocahceEnable;
uint8_t nocacheLaunchFlag;

// Globals:
// main screen
uint16_t mainScr; //obrazovka top slotu / top-slot screen

// pscg
uint8_t * pscgErrorString;


// few getters/setters
void svmSetRestoreSlot(uint8_t slot) {
  slot_restore = slot;
}

void svmSetNextPid(uint16_t pid) {
  nextPid = pid;
}

uint16_t svmGetMainScreen() {
  return mainScr;
}

// gets if current SVM is valid
uint8_t svmGetValid() {
  return svmValid;
}

void svmSetValid(uint8_t val) {
  svmValid = val;
}

// SVS wrapper headers
void pcBasicWrapInit();
void svsGr2WrapInit();

void svmBootInit() {
  svsSetStringField(svmStrings, sizeof(svmStrings), &svm);
  svsDirectSWrapInit();
  pcBasicWrapInit();
  svsGr2WrapInit();
  sda_files_wrapper_init();
  sda_svs_wrapper_init();
}

// app loading/closing
// wrapper for svs loadApp, loads fname in svm
uint8_t svmTokenizeFile(uint8_t *fname, uint8_t *name, uint8_t mode) {
  uint8_t dirbuf[258];
  svp_getcwd(dirbuf, 256);

#ifdef SVM_DBG_ENABLED
      printf("%s: tokenizing: %s in %s \n",__FUNCTION__ , fname, name);
      uint8_t dbg_dir[258];
      svp_getcwd(dbg_dir, 256);
      printf("cwd: %s\n", dbg_dir);
      setTokenizerDebug(1);
#endif

#ifdef PC
  uint32_t sdl_time;
  sdl_time = SDL_GetTicks();
#endif

  if (svmValid == 0) {
    svsLoadCounter = 0;

    if (svp_fexists(fname) == 0) {
      printf("File %s does not exist!\n", fname);
      sda_show_error_message((uint8_t *)"File does not exist!\n");
      return 1;
    }

    if (loadApp(fname, name, &svm, mode) != 0) {
      if(errCheck(&svm) != 0) {
        svp_errSoftPrint(&svm);
      }
      return 1;
    }
#ifdef PLATFORM_PC
    printf("Loading time: %ums\n", SDL_GetTicks() - sdl_time);
#else
    printf("Loading time: %ums\n", svsLoadCounter);
#endif
  }

#ifdef SVM_DBG_ENABLED
  printf("%s: success\n",__FUNCTION__);
#endif

  return 0;
}


// launch app
uint8_t svmLaunch(uint8_t * fname, uint16_t parentPid) {
  uint8_t cacheBuffer[256];
  uint8_t numbuff[25];
  uint8_t dirbuf[258];
  uint8_t fname_updated[APP_NAME_LEN + 1];
  svp_getcwd(dirbuf, 256);

#ifdef SVM_DBG_ENABLED
      printf("%s: launching: %s \n",__FUNCTION__ , fname);
#endif

  uint16_t singularId = 0;
  singularId = svmGetIfSingular(fname);
  if (singularId) {
#ifdef SVM_DBG_ENABLED
      printf("%s: waking singular...\n",__FUNCTION__);
#endif
    svmWake(singularId);
    return 1;
  }

  uint16_t x = 0;
  while (svmSavedProc[x].valid == 1) {
    x++;
    if (x == MAX_OF_SAVED_PROC) {
      sda_show_error_message((uint8_t *)"Maximum number of active apps reached.\n");
      return 0;
    }
  }

  sda_int_to_str(numbuff, (int32_t)nextPid, sizeof(numbuff));
  sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
  sda_str_add(cacheBuffer, numbuff);
  sda_str_add(cacheBuffer,(uint8_t *) ".stc");

  if (sda_strlen(fname) > APP_NAME_LEN) {
    printf("Application name %s is too long!\n", fname);
    return 0;
  }

  if (svmValid)  {
#ifdef SVM_DBG_ENABLED
      printf("%s: suspending running app: %s \n",__FUNCTION__ , svmMeta.name);
#endif
    if(svmStoreRunning()) {
      return 0;
    }
  }
  svmValid = 0; // invalidate slot before loading

  set_gr2_workaround_context(&sda_app_con);

  if (parentPid != 0 && svmMeta.launchFromCWD == 1) {
    svp_chdir(dirbuf);

    // if we do not launch from launcher, we update the path of the executable
    if (svmUpdatePath(fname_updated, fname)) {
      sda_show_error_message((uint8_t *)"Executables are only allowed in APPS folder.\n");
      svp_switch_main_dir();
      svp_chdir((uint8_t *)"APPS");
      return 0;
    }
    fname = fname_updated;
  }

  // move to APPS directory
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");

  // check for cache
  if (sda_fs_check_and_create_dir((uint8_t *)"cache")) {
    sda_show_error_message((uint8_t *)"svmLaunch:\nCache folder is borked.\n/APPS/cache");
    return 0;
  }

  // check for pre-tokenized app
  int32_t crc = (int32_t) crc32b(fname);
  if (crc < 0) {
    crc *= -1;
  }

  svp_file fil;
  svp_fopen_rw(&fil, fname);
  uint32_t siz = svp_get_size(&fil);
  svp_fclose(&fil);

  if(svmPreCachedExists(crc, siz) && nocacheLaunchFlag == 0) {
    printf("%s: loading cached: %s\n",__FUNCTION__, fname);
    if(svmLoadPrecached(crc)) {
      return 0;
    }
  } else {
    if(autocahceEnable && nocacheLaunchFlag == 0) {
      // precahce the app, then launch it from cahce
      if(svmPrecacheFile(fname)) {
        printf("%s: caching app failed: %s\n",__FUNCTION__, fname);
        sda_show_error_message("svmPrecacheFile: Tokenizer error!");
        return 0;
      }
      if(svmLoadPrecached(crc)) {
        printf("%s: caching, then loading app failed: %s\n",__FUNCTION__, fname);
        return 0;
      }
    } else {
      nocacheLaunchFlag = 0;
      // loads app
      if (svmTokenizeFile(fname, cacheBuffer, 0) != 0) {
        return 0;
      }
    }
  }

  // set metadata
  svmLaunchSetDefMetadata(nextPid, parentPid, fname);

  // insert to table of running apps
  svmSuspendInitPid(svmMeta.pid, svmMeta.name);

  nextPid++;
  wrap_set_lcdOffButtons(0);

  // move to DATA
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"DATA");

  // reset gr2 context
  gr2_reset_context(&sda_app_con);
  svmGetGR2Settings();
  
  // validate the app slot
  sda_slot_set_valid(SDA_SLOT_SVM);
  sda_slot_on_top(SDA_SLOT_SVM);
  
  // show the close button
  svpSGlobal.systemXBtnVisible = 1;
  svpSGlobal.systemXBtnClick = 0;
  svmValid = 1;

  // reset timers
  sdaSvmClearTimer();

  return 1;
}

void svmLoadParent(uint8_t errorOccured) {
  uint8_t  argBuff[2048];
  varType  svmCallRetval[3];
  uint8_t  svmCallRetvalType[3];
  uint8_t* svmCallRetvalStr[3];

  svmStoreArguments(argBuff, svmMeta.svmCallRetval, svmMeta.svmCallRetvalType, svmMeta.svmCallRetvalStr, &svm);

  for (uint32_t i = 0; i < 3; i++) {
    svmCallRetval[i]     = svmMeta.svmCallRetval[i];
    svmCallRetvalType[i] = svmMeta.svmCallRetvalType[i];
    svmCallRetvalStr[i]  = svmMeta.svmCallRetvalStr[i];
  }

  // load parent
  if (svmGetValidPid(svmMeta.parentPid)) {
    if (svmLoadProcData(svmMeta.parentPid) == 0) {
      printf("svmCloseRunning: Loading parent app failed!\n");
      svmValid = 0;
      sda_slot_set_invalid(SDA_SLOT_SVM);
      sda_slot_on_top(SDA_SLOT_APPLIST);
      svp_switch_main_dir();
      svp_chdir((uint8_t *)"APPS");
      return;
    }
  } else {
    svmValid = 0;
    sda_slot_set_invalid(SDA_SLOT_SVM);
    sda_slot_on_top(SDA_SLOT_APPLIST);
    printf("svmCloseRunning: Parent is not valid.\n");
    svp_switch_main_dir();
    svp_chdir((uint8_t *)"APPS");
    return;
  }

  // Launch callback
  if(functionExists(svmGetCallback(), &svm)) {
    svmRestoreArguments(svmCallRetvalType, svmCallRetval, svmCallRetvalStr, &svm);
    if(!errorOccured) {
      commExec(svmGetCallback(), &svm);
    } else {
      printf("%s: Error occured in child process, callback won't be launched!\n", __FUNCTION__);
    }
  } else {
    if (!svp_strcmp(svmGetCallback(), "")) {
      printf("%s: Callback \"%s\" does not exist!\n", __FUNCTION__, svmGetCallback());
    }
  }

  svmOnTop();
  setRedrawFlag();
}


// gracefully closes running svm (app)
void svmCloseRunning() {
  uint8_t errorOccured = 0;

  if (sda_slot_get_valid(4) == 0 || svmValid == 0) {
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
  if(svmGetSuspendOnClose() && errCheck(&svm) == 0) {
    svmSuspend();
    return;
  }  

  if (sda_slot_get_valid(4) == 1) {
    if(functionExists((uint8_t *)"exit", &svm)) {
      commExec((uint8_t *)"exit", &svm);
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

  if (svmMeta.parentPid != 0) {
    svmLoadParent(errorOccured);
    return;
  }

  gr2_text_deactivate(&sda_app_con);
  svpSGlobal.systemXBtnVisible = 0;
  svmValid = 0;
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
      svmClose(svmSavedProc[x].pid);
    }
  }

  if (sda_serial_is_enabled()) {
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
void svmClose(uint16_t id) {
  uint16_t prevApp = 0;

  if (svmMeta.pid != id && sda_get_top_slot() == 4) {
    prevApp = svmMeta.pid;
  }

  if (svmMeta.pid != id) {
    svmWake(id);
  }

  // here we close even the unclosable apps
  svmSetSuspendOnClose(0);

  svmCloseRunning();
  
  if (prevApp) {
    svmWake(prevApp);
  }
}


void sdaSvmKillApp_handle() {
  gr2_text_deactivate(&sda_app_con);
  svpSGlobal.systemXBtnClick = 0;
  svpSGlobal.systemXBtnVisible = 0;
  svmValid = 0;
  svpSGlobal.kbdVisible = 0;
  svpSGlobal.touchType = EV_NONE;
  svpSGlobal.touchValid = 0;
  if (sda_slot_get_screen(SDA_SLOT_SVM) != 0) {
    gr2_destroy(sda_slot_get_screen(SDA_SLOT_SVM), &sda_app_con);
  }
  sda_slot_set_invalid(SDA_SLOT_SVM);

  svmInValidate(svmMeta.pid);
  sda_set_sleep_lock(0);

  sdaAlarmClearFlag();
  sda_files_close();

  svp_crypto_lock();
  gr2_cleanup(&sda_app_con); // performs cleanup of pscg elements
  sda_slot_on_top(SDA_SLOT_APPLIST);
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");
  sda_set_sleep_lock(0);
}


uint16_t svmRun(uint8_t init, uint8_t top) {

  if (init) {
    svmInit();
    return 0;
  }

  if(!svmValid) {
    return 0;
  }

  //svs init call
  if (svmMeta.initExecuted == 0) {
    commExec((uint8_t *)"init", &svm);
    svmClearArguments(&svm);
    svmMeta.initExecuted = 1;
    return 0;
  }

  sda_files_copyer();

  // SVM container is on top
  if (top == 1) {
    
    // check for exit
    if (svmCheckAndExit()) {
      return 0;
    }

    // Beep callback
    if (svmBeepHandler()) {
      if (!svp_strcmp(svmMeta.beepTimerCallback, (uint8_t *)"")) {
        if (functionExists(svmMeta.beepTimerCallback, &svm)) {
          commExec(svmMeta.beepTimerCallback, &svm);
        } else {
          svmCloseRunning();
          sda_show_error_message((uint8_t *)"Beep callback function not found.\n");
          return 0;
        }
      }
    }

    // Run "update"
    if(functionExists((uint8_t *)"update", &svm)) {
      commExec((uint8_t *)"update", &svm);
    } else {
      svmCloseRunning();
      printf("No update function found.\n");
      sda_show_error_message((uint8_t *)"No update function found.\n");
      return 0;
    }
    sdaSetRedrawDetect(0);

    // Perform app-kill
    if (svm.handbrake == 1) {
      sdaSvmKillApp_handle();
      sda_show_error_message(pscgErrorString);
      return 0;
    }

    // Check and perform call
    if(svmRunPerformCall()) {
      // init call here with args
      if(svmMeta.initExecuted == 0) {
        commExec((uint8_t *)"init", &svm);
        svmClearArguments(&svm);
        svmMeta.initExecuted = 1;
      }
      svmOnTop();
      setRedrawFlag();
    }

    if(svmGetSuspendFlag()) {
      svmSuspend();
      svmSetSuspendFlag(0);
    }
    
    // Set new screen
    // This needs to be after all init calls
    if (svs_wrap_setScr_flag == 1) {
      svs_wrap_setScr_flag = 0;
      mainScr = svs_wrap_setScr_id;
      slotScreen[4] = svs_wrap_setScr_id;
    }

  }
  return 0;
}
