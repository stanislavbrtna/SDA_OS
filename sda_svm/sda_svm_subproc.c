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

#include "sda_svm_subproc.h"

extern sdaSvmMetadata svmMeta;
extern svsVM          svm;
extern uint8_t        svmStrings[STRING_FIELD_L];

static uint8_t flag_svmCall;
static uint8_t svmCallName[APP_NAME_LEN];
static varType svmCallArg[3];
static uint8_t svmCallArgType[3];
static uint8_t *svmCallArgStr[3];

static uint8_t svmCallback[NAME_LENGTH];


extern gr2Element *sda_app_gr2_elements;
extern gr2Screen  *sda_app_gr2_screens;
extern gr2context sda_app_con;

uint8_t *svmGetCallback() {
    return svmCallback;
}

uint8_t svmRunPerformCall() {
  uint8_t  argBuff[APP_ARG_STR_LEN];
  uint16_t parentPid = svmMeta.pid;
  //TODO: Storing and restoring arguments crashes on emcc     

  if (flag_svmCall != 1) {
    return 0;
  }

  // call is served, drop the flag
  flag_svmCall = 0;

  svmStoreArguments(argBuff, svmCallArg, svmCallArgType, svmCallArgStr, &svm);
  // if there is an overlay, we destroy it before switching the context
  destroyOverlay();

  // get if singular and running
  uint16_t singularId = 0;
  singularId = svmGetIfSingular(svmCallName);
  if (singularId) {
    // fill the args
    //printf("svmRunPerformCall Process:%s (id:%u) calling:%u\n", svmMeta.name, svmMeta.pid, singularId);
    svmWakeArgs(singularId, svmCallArgType, svmCallArg, svmCallArgStr);
    sda_strcp(svmCallback, svmMeta.svmCallback, NAME_LENGTH);
    return 1;
  }
  
  // otherwise launch
  if(svmLaunch(svmCallName, svmMeta.pid) == 0) {
    svmWake(parentPid);
    sda_show_error_message((uint8_t *)"svmRun: subprocess launch failed!");
    return 0;
  }
  svmRestoreArguments(svmCallArgType, svmCallArg, svmCallArgStr, &svm);
  // Restore callback
  sda_strcp(svmCallback, svmMeta.svmCallback, NAME_LENGTH);
  
  return 1;
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


void svmRestoreArguments(uint8_t* argType, varType *arg, uint8_t **svmArgs, svsVM *s) {
  for(uint8_t z = 0; z < 3; z++) {
    s->commArgs.argType[z + 1] = argType[z];

    if (argType[z] == SVS_TYPE_STR) {
      s->commArgs.arg[z + 1] = (varType)strNew(svmArgs[z], s);
      //printf("%s-> restoring %s\n", __FUNCTION__, svmArgs[z]);
    } else {
      s->commArgs.arg[z + 1] = arg[z];
    }
  }
  s->commArgs.usedup = 3;
}

void svmClearArguments(svsVM *s) {
  for(uint8_t z = 0; z < 3; z++) {
    s->commArgs.argType[z + 1] = SVS_TYPE_UNDEF;
    s->commArgs.arg[z + 1].val_u = 0;
  }
  s->commArgs.usedup = 0;
}

/*
 * Arguments: 3 possible arguments indexed 0 - 2
*/

void svmSetArgumentNum(uint8_t id, int32_t val) {
  svm.commArgs.argType[id + 1] = SVS_TYPE_NUM;
  svm.commArgs.arg[id + 1].val_s = val;
  svm.commArgs.usedup = 3;
}

void svmSetArgumentStr(uint8_t id, uint8_t* val) {
  svm.commArgs.argType[id + 1] = SVS_TYPE_STR;
  svm.commArgs.arg[id + 1] = (varType)strNew(val, &svm);
  svm.commArgs.usedup = 3;
}

void svmSetArgumentFlt(uint8_t id, float val) {
  svm.commArgs.argType[id + 1] = SVS_TYPE_NUM;
  svm.commArgs.arg[id + 1].val_f = val;
  svm.commArgs.usedup = 3;
}


void svmStoreArguments(uint8_t *buff, varType *arg, uint8_t* argType, uint8_t **svmArgs, svsVM *s) {
  uint32_t x, n;
  uint8_t *prac;
  uint8_t *prac2;
  uint8_t *prac3;

  n = 0;
  prac = buff;
  for(uint8_t z = 0; z < 3; z++) {
    if (argType[z] == SVS_TYPE_STR) {
      prac3 = s->stringField + arg[z].val_str;
      //printf("%s-> storing: %s\n", __FUNCTION__, prac3);
      prac2 = prac;
      x = 0;
      while(prac3[x] != 0) {
        buff[n] = prac3[x];
        x++;
        n++;
        prac++;
      }
      buff[n] = 0;
      n++;
      if(n > APP_ARG_STR_LEN - 1) {
        buff[APP_ARG_STR_LEN - 1] = 0;
        printf("%s: Error: storeArguments owerflow!\n", __FUNCTION__);
        return;
      }
      prac++;
      svmArgs[z] = prac2;
    }
  }
}


uint8_t svmLoadParent(uint8_t errorOccured) {
  uint8_t  argBuff[2048];
  varType  svmCallRetval[3];
  uint8_t  svmCallRetvalType[3];
  uint8_t* svmCallRetvalStr[3];
  uint8_t  callback[NAME_LENGTH];

  svmStoreArguments(argBuff, svmMeta.svmCallRetval, svmMeta.svmCallRetvalType, svmMeta.svmCallRetvalStr, &svm);

  sda_strcp(svmMeta.svmCallback, callback, NAME_LENGTH);

  for (uint32_t i = 0; i < 3; i++) {
    svmCallRetval[i]     = svmMeta.svmCallRetval[i];
    svmCallRetvalType[i] = svmMeta.svmCallRetvalType[i];
    svmCallRetvalStr[i]  = svmMeta.svmCallRetvalStr[i];
  }

  // load parent
  uint16_t pid = svmMeta.parentPid;
  if (svmGetValidPid(svmMeta.parentPid)) {
    if (svmLoadProcData(svmMeta.parentPid) == 0) {
      printf("%s: Loading parent app failed! (pid: %u)\n",__FUNCTION__, pid);
      svmSetValid(0);
      sda_slot_set_invalid(SDA_SLOT_SVM);
      sda_prev_slot_on_top(SDA_SLOT_APPLIST);
      svp_switch_main_dir();
      svp_chdir((uint8_t *)"APPS");
      return 1;
    }
  } else {   
    sda_prev_slot_on_top(SDA_SLOT_APPLIST);
    printf("%s: Parent is not valid. (%u)\n",__FUNCTION__, pid);
    svp_switch_main_dir();
    svp_chdir((uint8_t *)"APPS");
    return 1;
  }

  // Launch callback
  //printf("Parent loaded (%u) (pid: %u), executing callback: %s\n", pid, svmMeta.pid, callback);
  if(functionExists(callback, &svm)) {
    svmRestoreArguments(svmCallRetvalType, svmCallRetval, svmCallRetvalStr, &svm);
    if(!errorOccured) {
      commExec(callback, &svm);
    } else {
      printf("%s: Error occured in child process, callback won't be launched!\n", __FUNCTION__);
    }
  } else {
    if (!svp_strcmp(callback, (uint8_t *)"")) {
      printf("%s: Callback \"%s\" does not exist!\n", __FUNCTION__, callback);
      return 1;
    }
  }

  svmOnTop();
  setRedrawFlag();

  return 0;
}

uint8_t svmLoadPrevious() {
  uint16_t pid = svmMeta.prevPid;
  svmMeta.prevPid = 0;

  if (svmGetValidPid(pid)) {
    if (svmLoadProcData(pid) == 0) {
      printf("%s: Loading previous app failed! (pid: %u)\n",__FUNCTION__, pid);
      svmSetValid(0);
      sda_slot_set_invalid(SDA_SLOT_SVM);
      sda_prev_slot_on_top(SDA_SLOT_APPLIST);
      svp_switch_main_dir();
      svp_chdir((uint8_t *)"APPS");
      return 1;
    }
  } else {
    svmKillRunning();
    sda_slot_set_invalid(SDA_SLOT_SVM);
    printf("%s: Previous spp is not valid. (pid: %u)\n",__FUNCTION__, pid);
    svp_switch_main_dir();
    svp_chdir((uint8_t *)"APPS");
    sda_prev_slot_on_top(SDA_SLOT_APPLIST);
    return 1;
  }

  sda_slot_on_top(SDA_SLOT_SVM);
  svmOnTop();
  setRedrawFlag();
  return 0;
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
  
  if (sda_get_db_fname() != 0)  {
    sda_strcp(sda_get_db_fname(), svmMeta.openDbName, sizeof(svmMeta.openDbName));
    svmMeta.openDbUsed = 1;
  }

  svmMeta.lcdOffButtons = wrap_get_lcdOffButtons();

  sda_files_close();

#ifdef SVM_DBG_ENABLED
  printf("svmSaveProcData: cache files\n");
#endif

  sdaSvmSaver(svmMeta.pid, (uint8_t *) ".svm", &svm, sizeof(svm));
  sdaSvmSaver(svmMeta.pid, (uint8_t *) ".str", svmStrings, svm.stringFieldLen + 1);
  sdaSvmSaver(svmMeta.pid, (uint8_t *) ".gr0", &sda_app_gr2_elements, sizeof(gr2Element) * SDA_APP_ELEM_MAX);
  sdaSvmSaver(svmMeta.pid, (uint8_t *) ".gr1", &sda_app_gr2_screens, sizeof(gr2Screen) * SDA_APP_SCREEN_MAX);
  sdaSvmSaver(svmMeta.pid, (uint8_t *) ".gr2", &sda_app_con, sizeof(gr2context));
  sdaSvmSaver(svmMeta.pid, (uint8_t *) ".met", &svmMeta, sizeof(svmMeta));

#ifdef SVM_DBG_ENABLED
  printf("svmSaveProcData: done\n");
#endif
}


uint8_t svmLoadProcData(uint16_t pid) {

#ifdef SVM_DBG_ENABLED
  printf("svmLoadProcData: loading: id: %u\n", id);
#endif

  if(!sdaSvmLoader(pid, (uint8_t *) ".svm", &svm, sizeof(svm)))
    return 0;

  if(!sdaSvmLoader(pid, (uint8_t *) ".str", svmStrings, svm.stringFieldLen + 1))
    return 0;

  if(!sdaSvmLoader(pid, (uint8_t *) ".gr0", &sda_app_gr2_elements, sizeof(gr2Element) * SDA_APP_ELEM_MAX))
    return 0;

  if(!sdaSvmLoader(pid, (uint8_t *) ".gr1", &sda_app_gr2_screens, sizeof(gr2Screen) * SDA_APP_SCREEN_MAX))
    return 0;

  if(!sdaSvmLoader(pid, (uint8_t *) ".gr2", &sda_app_con, sizeof(gr2context)))
    return 0;

  if(!sdaSvmLoader(pid, (uint8_t *) ".met", &svmMeta, sizeof(svmMeta)))
    return 0;

  SVSopenCache(&svm);

  svp_switch_main_dir();
  svp_chdir(svmMeta.currentWorkDir);
  //printf("restoring workdir: %s\n", svmMeta.currentWorkDir);

  for(uint16_t i = 0; i < SDA_FILES_OPEN_MAX; i++) {
    if (svmMeta.openFileUsed[i]) {
      if (!sda_fr_fname_reopen(i)) {
        printf("Failed to open file: %s\n", svmMeta.openFileName);
        sda_show_error_message("Filed to open file!");
        return 0;
      }
    }
  }

  if (svmMeta.openConfUsed) {
    if (!svp_fexists(svmMeta.openConfName)) {
      printf("Failed to open conf file: %s\n", svmMeta.openConfName);
      sda_show_error_message("Filed to open conf file!");
      return 0;
    }

    sda_files_conf_open(svmMeta.openConfName);
  }

  if (svmMeta.openCsvUsed) {
    if (!svp_fexists(svmMeta.openCsvName)) {
      printf("Failed to open csv file: %s\n", svmMeta.openCsvName);
      sda_show_error_message("Filed to open conf file!");
      return 0;
    }

    sda_files_csv_open(svmMeta.openCsvName);
  }

  if (svmMeta.openDbUsed) {
    if (!svp_fexists(svmMeta.openDbName)) {
      printf("Failed to open db file: %s\n", svmMeta.openDbName);
      sda_show_error_message("Filed to open db file!");
      return 0;
    }

    sda_files_db_open(svmMeta.openDbName);
  }

  if (svmMeta.lcdOffButtons) {
    wrap_set_lcdOffButtons(1);
  } else {
    wrap_set_lcdOffButtons(0);
  }

  if (svmMeta.landscape != svpSGlobal.lcdLandscape) {
    sda_set_landscape(svmMeta.landscape);
  }

  if (svmGetCryptoUnlock()) {
    svp_crypto_unlock_nopass();
  }

  svp_chdir(svmMeta.currentWorkDir);

  sda_slot_set_screen(SDA_SLOT_SVM, svmMeta.screen);

  svpSGlobal.kbdVisible = svmMeta.kbdVisible;
  return 1;
}


uint8_t sdaSvmLoader(uint16_t pid, uint8_t * tail, void *target, uint32_t size) {
  uint8_t cacheBuffer[256];
  uint8_t numbuff[25];
  svp_file svmFile;

  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");

  sda_int_to_str(numbuff, (int32_t)pid, sizeof(numbuff));
  sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
  sda_str_add(cacheBuffer, numbuff);
  sda_str_add(cacheBuffer, tail);

  if(svp_fopen_read(&svmFile, cacheBuffer) == 0) {
    printf("sdaSvmLoader: file open error (%s)\n", cacheBuffer);
    return 0;
  }

  svp_fread(&svmFile, target, size);
  svp_fclose(&svmFile);

  return 1;
}


void sdaSvmSaver(uint16_t pid, uint8_t * tail, void *target, uint32_t size) {
  uint8_t cacheBuffer[256];
  uint8_t numbuff[25];
  svp_file svmFile;

  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");

  sda_int_to_str(numbuff, (int32_t)pid, sizeof(numbuff));
  sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
  sda_str_add(cacheBuffer, numbuff);
  sda_str_add(cacheBuffer, tail);

  if(svp_fopen_rw(&svmFile, cacheBuffer) == 0) {
    printf("sdaSvmSaver: file open error\n");
    return;
  }
  svp_fwrite(&svmFile, target, size);

  svp_fclose(&svmFile);
}
