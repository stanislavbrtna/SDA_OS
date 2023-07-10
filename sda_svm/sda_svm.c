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

#include "sda_svm.h"
#include "sda_svm_load_save.h"
#include "sda_svm_misc.h"

//svs VM
svsVM          svm;
sdaSvmMetadata svmMeta;
uint8_t        svmStrings[STRING_FIELD_L];

static uint16_t nextId;

static uint8_t svmValid;
static uint8_t svm_init;

static uint8_t flag_svmCall;
static uint8_t svmCallName[APP_NAME_LEN];
static uint8_t svmCallback[NAME_LENGTH];
static varType svmCallArg[3];
static uint8_t svmCallArgType[3];
static uint8_t *svmCallArgStr[3];

svmSavedProcType svmSavedProc[MAX_OF_SAVED_PROC];

extern uint16_t svsLoadCounter;
extern uint8_t soft_error_flag;

extern gr2Element *sda_app_gr2_elements;
extern gr2Screen *sda_app_gr2_screens;
extern gr2context sda_app_con; //

extern uint8_t svs_wrap_setScr_flag;
extern uint16_t svs_wrap_setScr_id;

static uint8_t slot_restore; // what appslot to restore after close

// Globals:
// main screen
uint16_t mainScr; //obrazovka top slotu / top-slot screen

// pscg
uint8_t * pscgErrorString;

// static headers
static void svmInValidate(uint16_t id);
static void svmSuspendAddId(uint16_t id, uint8_t * name);
static uint16_t GetIfSingular(uint8_t * name);


uint8_t svmGetSavedProcValid(uint16_t proc_array_index) {
  return svmSavedProc[proc_array_index].valid;
}


uint16_t svmGetSavedProcId(uint16_t proc_array_index) {
  return svmSavedProc[proc_array_index].id;
}


void svmSetNextId(uint16_t id) {
  nextId = id;
}


uint16_t svmGetMainScreen() {
  return mainScr;
}


uint8_t * svmGetName() {
  return svmMeta.name;
}

// app misc
uint8_t svmGetRunning() {
  if (svmValid){
    return 1;
  }
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].valid == 1) {
      return 1;
    }
  }
  return 0;
}

void svmBootInit() {
  svsSetStringField(svmStrings, sizeof(svmStrings), &svm);
  svsDirectSWrapInit();
  pcBasicWrapInit();
  svsGr2WrapInit();
  sda_files_wrapper_init();
  sda_svs_wrapper_init();
}

uint8_t svmGetValidId(uint16_t id) {
  if (svmMeta.id == id && svmValid){
    return 1;
  }
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].id == id) {
      return svmSavedProc[x].valid;
    }
  }
  return 0;
}

// gets if current SVM is valid
uint8_t svmGetValid() {
  return svmValid;
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

void svmLaunchSetDefMetadata(uint16_t id, uint16_t parentId, uint8_t *fname) {
  svmMeta.id = id;
  svmMeta.parentId = parentId;
  
  sda_strcp(fname, svmMeta.name, sizeof(svmMeta.name));
  
  for(uint16_t i = 0; i < SDA_FILES_OPEN_MAX; i++) {
    svmMeta.openFileUsed[i] = 0;
    sda_strcp((uint8_t *)"", svmMeta.openFileName[i], sizeof(svmMeta.openFileName[i]));
  }
  
  sda_strcp((uint8_t *)"", svmMeta.openConfName, sizeof(svmMeta.openConfName));
  sda_strcp((uint8_t *)"", svmMeta.openCsvName, sizeof(svmMeta.openCsvName));
  sda_strcp((uint8_t *)"DATA", svmMeta.currentWorkDir, sizeof(svmMeta.currentWorkDir));
  svmMeta.openConfUsed = 0;
  svmMeta.openCsvUsed  = 0;
  
  svmMeta.loadUptime     = svpSGlobal.uptimeMs;
  svmMeta.landscape      = 0;
  svmMeta.lcdOffButtons  = 0;
  svmMeta.launchFromCWD  = 0;
  svmMeta.cryptoUnlocked = 0;
  svmMeta.beepTime       = 0;
  svmMeta.authorized     = 0;

  for (uint16_t i = 0; i < 3; i++) {
    svmMeta.svmCallRetval[i].val_u = 0;
    svmMeta.svmCallRetvalType[i]   = SVS_TYPE_UNDEF;
  }
}



// launch app
uint8_t svmLaunch(uint8_t * fname, uint16_t parentId) {
  uint8_t cacheBuffer[256];
  uint8_t numbuff[25];
  uint8_t dirbuf[258];
  uint8_t fname_updated[APP_NAME_LEN + 1];
  svp_getcwd(dirbuf, 256);

#ifdef SVM_DBG_ENABLED
      printf("%s: launching: %s \n",__FUNCTION__ , fname);
#endif

  uint16_t singularId = 0;
  singularId = GetIfSingular(fname);
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

  sda_int_to_str(numbuff, (int32_t)nextId, sizeof(numbuff));
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
    if(svmSuspend()) {
      return 0;
    }
  }
  svmValid = 0; // invalidate slot before loading

  set_gr2_workaround_context(&sda_app_con);

  if (parentId != 0 && svmMeta.launchFromCWD == 1) {
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

  if(svmPreCachedExists(crc)) {
    if(svmLoadPrecached(crc)) {
      return 0;
    }
  } else {
    // loads app
    if (svmTokenizeFile(fname, cacheBuffer, 0) != 0) {
      return 0;
    }
  }
  svm_init = 0;

  // set metadata
  svmLaunchSetDefMetadata(nextId, parentId, fname);
  nextId++;
  wrap_set_lcdOffButtons(0);

  // move to DATA
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"DATA");

  // reset gr2 context
  gr2_reset_context(&sda_app_con);
  svmGetGR2Settings();
  
  // validate the app slot
  sda_slot_set_valid(4);
  sda_slot_on_top(4);
  
  // insert to table of running apps
  svmSuspendAddId(svmMeta.id, svmMeta.name);
  
  // show the close button
  svpSGlobal.systemXBtnVisible = 1;
  svpSGlobal.systemXBtnClick = 0;
  svmValid = 1;

  // reset timers
  sdaSvmClearTimer();

  return 1;
}

// gracefully closes running svm (app)
void svmCloseRunning() {

  if (sda_slot_get_valid(4) == 0 || svmValid == 0) {
    return;
  }

  if((errCheck(&svm) == 1) && (soft_error_flag == 0)) {
    svp_errSoftPrint(&svm);
  } else {
    if ((getOverlayId() != 0) && (soft_error_flag == 0)) {
      destroyOverlay();
    }
  }

  if (sda_slot_get_valid(4) == 1) {
    if(functionExists((uint8_t *)"exit", &svm)) {
      commExec((uint8_t *)"exit", &svm);
      if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
        svp_errSoftPrint(&svm);
      }
    }
  }

  if (svmMeta.cryptoUnlocked) {
    svp_crypto_lock();
  }

  SVScloseCache(&svm);
  svmInValidate(svmMeta.id);
  sda_set_sleep_lock(0);
  svpSGlobal.kbdVisible = 0;
  sda_set_landscape(0);
  sda_alarm_clear_flag();
  sda_files_close();

  if (svmMeta.parentId != 0) {
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
    if (svmGetValidId(svmMeta.parentId)) {
      if (svmLoadProcData(svmMeta.parentId) == 0) {
        printf("svmCloseRunning: Loading parent app failed!\n");
        svmValid = 0;
        sda_slot_set_invalid(4);
        sda_slot_on_top(1);
        svp_switch_main_dir();
        svp_chdir((uint8_t *)"APPS");
        return;
      }
    } else {
      svmValid = 0;
      sda_slot_set_invalid(4);
      sda_slot_on_top(1);
      printf("svmCloseRunning: Parent is not valid.\n");
      svp_switch_main_dir();
      svp_chdir((uint8_t *)"APPS");
      return;
    }

    // Launch callback
    if(functionExists(svmCallback, &svm)) {
      svmRestoreArguments(svmCallRetvalType, svmCallRetval, svmCallRetvalStr, &svm);
      commExec(svmCallback, &svm);
    }

    svmOnTop();
    setRedrawFlag();
    return;
  }

  gr2_text_deactivate(&sda_app_con);
  svpSGlobal.systemXBtnVisible = 0;
  svmValid = 0;
  if (sda_slot_get_screen(4) != 0) {
    gr2_destroy(sda_slot_get_screen(4), &sda_app_con);
  }
  sda_slot_set_invalid(4);

  // TODO: restore right slot.. previous app perhaps...
  if (slot_restore == 0) {
    sda_slot_on_top(1);
  }

  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");
}


void svmSetRestoreSlot(uint8_t slot) {
  slot_restore = slot;
}


void svmCloseAll() {
  svmSetRestoreSlot(sda_get_top_slot());

  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].valid == 1) {
      svmClose(svmSavedProc[x].id);
    }
  }

  if (sda_serial_is_enabled()) {
    sda_serial_disable();
  }

  if (slot_restore != 0) {
    if (sda_slot_get_valid(slot_restore)) {
      sda_slot_on_top(slot_restore);
    } else {
      sda_slot_on_top(0);
    }
    slot_restore = 0;
  }
}


static void svmSuspendAddId(uint16_t id, uint8_t * name) {
  uint16_t index = 0;
  while (svmSavedProc[index].valid != 0) {
    if (index == MAX_OF_SAVED_PROC - 1) {
      return;
    }
    index++;
  }

  sda_strcp(name, svmSavedProc[index].name, APP_NAME_LEN);
  svmSavedProc[index].id = id;
  svmSavedProc[index].valid = 1;
  svmSavedProc[index].singular = 0;
}


uint8_t svmSuspend() {
  if(functionExists(SUSPEND_FUNCTION, &svm)) {
    commExec(SUSPEND_FUNCTION, &svm);
    if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
      svp_errSoftPrint(&svm);
      return 1;
    }
    if (svmCheckAndExit()) {
      return 0;
    }
  }
  svmSaveProcData();
  return 0;
}


uint8_t svmWake(uint16_t id) {
  if(id == svmMeta.id && svmValid) {
    svmOnTop();
    if(functionExists(WAKEUP_FUNCTION, &svm)) { // execute the wakeup
      commExec(WAKEUP_FUNCTION, &svm);
      if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
        svp_errSoftPrint(&svm);
        return 1;
      }
      if (svmCheckAndExit()) { // handle potential exit call
        return 0;
      }
    }
    return 0;
  }

  if (svmValid) {
    svmSuspend();
  }

  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].id == id && svmSavedProc[x].valid == 1) {
      if (svmLoadProcData(id) == 0) {
        printf("svmWake: error while loading app (1)\n");
        svmSavedProc[x].valid = 0;
        return 1;
      }
      sda_slot_set_valid(4);
      svmValid = 1;
      svmOnTop();
      if(functionExists(WAKEUP_FUNCTION, &svm)) {
        commExec(WAKEUP_FUNCTION, &svm);
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
  }
  printf("svmWake: error while loading app (2)\n");
  return 1;
}


static uint16_t GetIfSingular(uint8_t * name) {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].valid == 1) {
      if (strCmp(svmSavedProc[x].name, name) && svmSavedProc[x].singular) {
        return svmSavedProc[x].id;
      }
    }
  }
  return 0;
}


void svmSetSingular(uint16_t id) {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].valid == 1 && svmSavedProc[x].id == id) {
      svmSavedProc[x].singular = 1;
    }
  }
}


uint16_t svmGetSuspendedId(uint16_t id) { 
  if (svmSavedProc[id].valid == 1) {
    return svmSavedProc[id].id;
  }
  return 0;
}


uint8_t *svmGetSuspendedName(uint16_t id) {
  if (svmSavedProc[id].valid == 1) {
    return svmSavedProc[id].name;
  }
  return (uint8_t *)"";
}


// closes app with given id
void svmClose(uint16_t id) {
  uint16_t prevApp = 0;

  if (svmMeta.id != id && sda_get_top_slot() == 4) {
    prevApp = svmMeta.id;
  }

  if (svmMeta.id != id) {
    svmWake(id);
  }

  svmCloseRunning();
  
  if (prevApp) {
    svmWake(prevApp);
  }
}


static void svmInValidate(uint16_t id) {
  for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
    if (svmSavedProc[x].id == id && svmSavedProc[x].valid == 1) {
      svmSavedProc[x].valid = 0;
      svmRemoveCachedProc(id);
#ifdef SVM_DBG_ENABLED
      printf("svmInValidate: removing cached proc x:%u id:%u\n", x, id);
#endif
    }
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
  if (sda_slot_get_screen(4) != 0) {
    gr2_destroy(sda_slot_get_screen(4), &sda_app_con);
  }
  sda_slot_set_invalid(4);

  svmInValidate(svmMeta.id);
  sda_set_sleep_lock(0);

  sda_alarm_clear_flag();
  sda_files_close();

  svp_crypto_lock();
  gr2_cleanup(&sda_app_con); // performs cleanup of pscg elements
  sda_slot_on_top(1);
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");
  sda_set_sleep_lock(0);
}


uint8_t svmRunPerformCall() {
  uint8_t argBuff[APP_ARG_STR_LEN];
  uint16_t parentId = svmMeta.id;
  //TODO: Storing and restoring arguments crashes on emcc     
  
  svmStoreArguments(argBuff, svmCallArg, svmCallArgType, svmCallArgStr, &svm);

  if(svmLaunch(svmCallName, svmMeta.id) == 0) {
    flag_svmCall = 0;
    svmValid = 0;
    svmWake(parentId);
    sda_show_error_message((uint8_t *)"svmRun: subprocess launch failed!");
    return 0;
  }
  
  svmRestoreArguments(svmCallArgType, svmCallArg, svmCallArgStr, &svm);    

  // init call here with args
  commExec((uint8_t *)"init", &svm);
  svmValid = 1;
  svm_init = 1;
  flag_svmCall = 0;

  return 1;
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
  if (svm_init == 0) {
    commExec((uint8_t *)"init", &svm);
    svm_init = 1;
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

    // Perform call
    if (flag_svmCall == 1) {
      svmRunPerformCall();
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


// call and retval
void svmCallSubProc(
    uint8_t *name,
    uint8_t *callback,
    varType arg0, uint8_t type0,
    varType arg1, uint8_t type1,
    varType arg2, uint8_t type2
  ) {
  flag_svmCall      = 1;
  svmCallArg[0]     = arg0;
  svmCallArgType[0] = type0;
  svmCallArg[1]     = arg1;
  svmCallArgType[1] = type1;
  svmCallArg[2]     = arg2;
  svmCallArgType[2] = type2;

  sda_strcp(callback, svmCallback, sizeof(svmCallback));
  sda_strcp(name, svmCallName, APP_NAME_LEN);
}


void svmSetSubProcRetval(varType arg0, uint8_t type0, varType arg1, uint8_t type1, varType arg2, uint8_t type2) {
  svmMeta.svmCallRetval[0]     = arg0;
  svmMeta.svmCallRetvalType[0] = type0;
  svmMeta.svmCallRetval[1]     = arg1;
  svmMeta.svmCallRetvalType[1] = type1;
  svmMeta.svmCallRetval[2]     = arg2;
  svmMeta.svmCallRetvalType[2] = type2;
}


void svmSaveProcData() {
#ifdef SVM_DBG_ENABLED
  printf("svmSaveProcData: saving: id: %u\n", svmMeta.id);
#endif
  SVScloseCache(&svm);

  // general purpose files are already stored in the metadata structure

  if (sda_get_conf_fname() != 0)  {
    sda_strcp(sda_get_conf_fname(), svmMeta.openConfName, sizeof(svmMeta.openConfName));
    svmMeta.openConfUsed = 1;
  }
  if (sda_get_csv_fname() != 0)  {
    sda_strcp(sda_get_csv_fname(), svmMeta.openCsvName, sizeof(svmMeta.openCsvName));
    svmMeta.openCsvUsed = 1;
  }

  //printf("storing workdir: %s\n", svmMeta.currentWorkDir);
  svmMeta.lcdOffButtons = wrap_get_lcdOffButtons();

  sda_files_close();
  svmMeta.screen = slotScreen[4];

  sdaSvmSaver(svmMeta.id, (uint8_t *) ".svm", &svm, sizeof(svm));
  sdaSvmSaver(svmMeta.id, (uint8_t *) ".str", svmStrings, svm.stringFieldLen + 1);
  sdaSvmSaver(svmMeta.id, (uint8_t *) ".gr0", &sda_app_gr2_elements, sizeof(gr2Element) * SDA_APP_ELEM_MAX);
  sdaSvmSaver(svmMeta.id, (uint8_t *) ".gr1", &sda_app_gr2_screens, sizeof(gr2Screen) * SDA_APP_SCREEN_MAX);
  sdaSvmSaver(svmMeta.id, (uint8_t *) ".gr2", &sda_app_con, sizeof(gr2context));
  sdaSvmSaver(svmMeta.id, (uint8_t *) ".met", &svmMeta, sizeof(svmMeta));
}


uint8_t svmLoadProcData(uint16_t id) {

#ifdef SVM_DBG_ENABLED
  printf("svmLoadProcData: loading: id: %u\n", id);
#endif

  if(!sdaSvmLoader(id, (uint8_t *) ".svm", &svm, sizeof(svm)))
    return 0;

  if(!sdaSvmLoader(id, (uint8_t *) ".str", svmStrings, svm.stringFieldLen + 1))
    return 0;

  if(!sdaSvmLoader(id, (uint8_t *) ".gr0", &sda_app_gr2_elements, sizeof(gr2Element) * SDA_APP_ELEM_MAX))
    return 0;

  if(!sdaSvmLoader(id, (uint8_t *) ".gr1", &sda_app_gr2_screens, sizeof(gr2Screen) * SDA_APP_SCREEN_MAX))
    return 0;

  if(!sdaSvmLoader(id, (uint8_t *) ".gr2", &sda_app_con, sizeof(gr2context)))
    return 0;

  if(!sdaSvmLoader(id, (uint8_t *) ".met", &svmMeta, sizeof(svmMeta)))
    return 0;

  SVSopenCache(&svm);

  svp_switch_main_dir();
  svp_chdir(svmMeta.currentWorkDir);
  //printf("restoring workdir: %s\n", svmMeta.currentWorkDir);

  for(uint16_t i = 0; i < SDA_FILES_OPEN_MAX; i++) {
    if (svmMeta.openFileUsed[i]) {
      sda_fr_fname_reopen(i);
    }
  }

  if (svmMeta.openConfUsed) {
    sda_files_conf_open(svmMeta.openConfName);
  }

  if (svmMeta.openCsvUsed) {
    sda_files_csv_open(svmMeta.openCsvName);
  }

  if (svmMeta.lcdOffButtons) {
    wrap_set_lcdOffButtons(1);
  } else {
    wrap_set_lcdOffButtons(0);
  }

  if (svmMeta.landscape != svpSGlobal.lcdLandscape) {
    sda_set_landscape(svmMeta.landscape);
  }

  svp_chdir(svmMeta.currentWorkDir);

  slotScreen[4] = svmMeta.screen;
  mainScr = slotScreen[4];
  return 1;
}
