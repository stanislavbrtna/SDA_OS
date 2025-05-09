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

extern gr2context sda_app_con;

extern uint8_t  svs_wrap_setScr_flag;
extern uint16_t svs_wrap_setScr_id;

uint8_t autocahceEnable;
uint8_t nocacheLaunchFlag;
 
// pscg
uint8_t * pscgErrorString;


// few getters/setters
void svmSetNextPid(uint16_t pid) {
  nextPid = pid;
}

uint16_t svmGetMainScreen() {
  return svmMeta.screen;
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

#define SVM_DBG_ENABLED
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

  if(strCmp(fname, "")) {
    printf("%s: Error: Empty fname given!\n", __FUNCTION__);
    return 0;
  }

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
  int32_t  tim = svp_get_mtime(fname);
  svp_fclose(&fil);

  if(svmPreCachedExists(crc, tim, siz) && nocacheLaunchFlag == 0) {
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

  // reset main screen
  svmMeta.screen = 0;
  sda_slot_set_screen(SDA_SLOT_SVM, svmMeta.screen);
  
  // show the close button
  svpSGlobal.systemXBtnVisible = 1;
  svpSGlobal.systemXBtnClick = 0;
  svmValid = 1;

  // reset timers
  sdaSvmClearTimer();

  return 1;
}


void sdaSvmKillApp_handle();
void svmHandleScreenChange();


uint16_t svmRun(uint8_t init, uint8_t top) {

  if (init) {
    svmInit();
    return 0;
  }

  if(!svmValid) {
    return 0;
  }

  // run the init function
  if (svmMeta.initExecuted == 0) {
    commExec((uint8_t *)"init", &svm);
    svmClearArguments(&svm);
    svmMeta.initExecuted = 1;
    svmHandleScreenChange();
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
#ifdef PLATFORM_PC
      svsGetCacheReloads(1);
#endif
      commExec((uint8_t *)"update", &svm);
#ifdef PLATFORM_PC
      if(svsGetCacheReloads(0)){
        //printf("DBG: update cache reloads: %u\n", svsGetCacheReloads(0));
      }
#endif
    } else {
      printf("%s: No update function found. (%s pid: %u)\n", __FUNCTION__, svmMeta.name, svmMeta.pid);
      sda_show_error_message((uint8_t *)"No update function found.\n");
      svmCloseRunning();
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
    
    svmHandleScreenChange();
  }
  return 0;
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

  sdaAlarmClearFlag();
  sda_files_close();
  sda_resource_free_pid(svmMeta.pid);
  sda_custom_icon_release_pid(svmMeta.pid);
  
  sda_set_landscape(0);

  svp_crypto_lock();
  gr2_cleanup(&sda_app_con); // performs cleanup of pscg elements
  sda_prev_slot_on_top(SDA_SLOT_HOMESCREEN);
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");
}


void svmHandleScreenChange() {
  if (svs_wrap_setScr_flag == 1) {
    svs_wrap_setScr_flag = 0;
    sda_slot_set_screen(SDA_SLOT_SVM, svmMeta.screen);
  }
}