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

//svs VM
svsVM   svm;
sdaSvmMetadata svmMeta;

static uint16_t nextId;

static uint8_t svmValid;
static uint8_t svmInit;

static uint8_t flag_svmCall;
static uint8_t svmCallName[APP_NAME_LEN];
static uint8_t svmCallback[NAME_LENGTH];
static varType svmCallArg[3];
static uint8_t svmCallArgType[3];
static uint8_t *svmCallArgStr[3];

static varType svmCallRetval[3];
static uint8_t *svmCallRetvalStr[3];
static uint8_t svmCallRetvalType[3];

static uint8_t svmSavedProcName[MAX_OF_SAVED_PROC][APP_NAME_LEN];
static uint16_t svmSavedProcId[MAX_OF_SAVED_PROC];
static uint8_t svmSavedProcValid[MAX_OF_SAVED_PROC];
static uint8_t svmSavedProcSingular[MAX_OF_SAVED_PROC];

static int32_t notificationId;
static int32_t notificationParam;
static uint8_t notificationFlag;

static uint8_t redrawDetect;

extern uint16_t svsLoadCounter;
extern uint8_t soft_error_flag;

extern pscgElement *sda_app_gr2_elements; //[SDA_APP_ELEM_MAX];
extern pscgScreen *sda_app_gr2_screens; //[SDA_APP_SCREEN_MAX];
extern gr2context sda_app_con; //

static uint16_t pscg_last_elements_count; //pro kontrolu zda po sobě aplikace uklidila

// Globals:

// main screen
uint16_t mainScr; //obrazovka top slotu
// svs
uint8_t svs_wrap_setScr_flag;
uint16_t svs_wrap_setScr_id;
// pscg
uint8_t * pscgErrorString;

// static headers
static void storeArguments(uint8_t *buff, varType *arg, uint8_t* argType, uint8_t **svmArgs, svsVM *s);
static void restoreArguments(uint8_t* argType, varType *arg, uint8_t **svmArgs, svsVM *s);
static void sdaSvmSaver(uint16_t id, uint8_t * tail, void *target, uint32_t size);
static uint8_t sdaSvmLoader(uint16_t id, uint8_t * tail, void *target, uint32_t size);
static void svmInValidate(uint16_t id);
static void svmSuspendAddId(uint16_t id, uint8_t * name);
static void svmRemoveCachedProc(uint16_t id);
static void svmRemoveCachedFile(uint16_t id, uint8_t * tail);
static uint8_t svmCheckAndExit();
static uint16_t GetIfSingular(uint8_t * name);

// notification

void setNotificationFlag(int32_t id, int32_t param) {
	notificationId = id;
	notificationParam = param;
	notificationFlag = 1;
}

int32_t getNotificationId() {
	return notificationId;
}

int32_t getNotificationParam() {
	return notificationParam;
}

int8_t getNotificationFlag() {
	return notificationFlag;
}

void clearNotificationFlag() {
	notificationFlag = 0;
}

// screens and stuff
void sdaSvmSetMainScreen(uint16_t val) {
	svs_wrap_setScr_id = val;
  svs_wrap_setScr_flag = 1;
  pscg_set_modified(val, &sda_app_con);
}

uint16_t sdaSvmGetMainScreen() {
	return mainScr;
}

void sdaSetRedrawDetect(uint8_t val) {
	redrawDetect = val;
}

uint8_t sdaGetRedrawDetect() {
	return redrawDetect;
}

void sdaSvmOnTop() {
	svp_switch_main_dir();
	svp_chdir(svmMeta.currentWorkDir);
	sdaSlotOnTop(4);
	svpSGlobal.systemXBtnVisible = 1;
	svpSGlobal.systemXBtnClick = 0;
}

void sdaSvmGetGR2Settings() {
	// load colors from system to app context
	pscg_set_border_color(pscg_get_border_color(&sda_sys_con), &sda_app_con);
	pscg_set_text_color(pscg_get_text_color(&sda_sys_con), &sda_app_con);
	pscg_set_background_color(pscg_get_background_color(&sda_sys_con), &sda_app_con);
	pscg_set_fill_color(pscg_get_fill_color(&sda_sys_con), &sda_app_con);
	pscg_set_active_color(pscg_get_active_color(&sda_sys_con), &sda_app_con);
}

// app misc

uint8_t sdaSvmGetRunning() {
	if (svmValid){
		return 1;
	}
	for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
		if (svmSavedProcValid[x] == 1) {
			return 1;
		}
	}
	return 0;
}

uint16_t sdaSvmGetId() {
	if (svmValid) {
		return svmMeta.id;
	} else {
		return 0;
	}
}

void sdaSvmKillApp() {
	svm.handbrake = 1;
}

void sdaSvmSetError(uint8_t * str) {
	pscgErrorString = str;
}

void svmSetLaunchCWDflag(uint8_t val) {
  svmMeta.launchFromCWD = val;
}

static uint8_t updatePath(uint8_t *newFname, uint8_t *oldFname) {
  uint8_t dirbuf[258];

  svp_getcwd(dirbuf, 256);
  newFname[0] = 0;

  for (uint16_t i = 0; i < sizeof(dirbuf); i++) {
    if ((i < (sizeof(dirbuf) - 4)) && dirbuf[i] == 'A' && dirbuf[i+1] == 'P' && dirbuf[i+2] == 'P' && dirbuf[i+3] == 'S') {
      i += 3;
      sda_strcp(dirbuf + i + 2, newFname, APP_NAME_LEN);
      sda_strcp("/", newFname + sda_strlen(newFname), APP_NAME_LEN);
      sda_strcp(oldFname, newFname + sda_strlen(newFname), APP_NAME_LEN);
      return 0;
    }
  }

  return 1;
}

// app loading/closing

uint8_t sdaSvmLoadApp(uint8_t *fname, uint8_t *name, uint8_t mode) {
	uint8_t dirbuf[258];
  svp_getcwd(dirbuf, 256);

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
        errSoftPrint(&svm);
        svp_errSoftPrint(&svm);
      }
    	return 1;
    }
#ifdef PLATFORM_PC
    printf("Loading time: %ums\n", SDL_GetTicks() - sdl_time);
#else
    printf("Loading time: %ums\n", svsLoadCounter);
#endif
    svmInit = 0;
  }
  return 0;
}

uint8_t sdaSvmLaunch(uint8_t * fname, uint16_t parentId) {
	uint8_t cacheBuffer[256];
	uint8_t numbuff[25];
	uint8_t dirbuf[258];
	uint8_t fname_updated[APP_NAME_LEN];
  svp_getcwd(dirbuf, 256);

	uint16_t singularId = 0;
	singularId = GetIfSingular(fname);
	if (singularId) {
		svmWake(singularId);
		return 1;
	}

	sda_int_to_str(numbuff, (int32_t)nextId, sizeof(numbuff));
	sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
	svp_str_add(cacheBuffer, numbuff);
	svp_str_add(cacheBuffer,(uint8_t *) ".stc");

	if (sda_strlen(fname) > APP_NAME_LEN) {
	  printf("Application name %s is too long!\n", fname);
	  return 0;
	}

	if (svmValid)	{
		if(svmSuspend()) {
			return 0;
		}
	}
	svmValid = 0; // invalidate slot before loading
	pscg_last_elements_count = pscg_get_element_count(&sda_app_con);
	set_pscg_workaround_context(&sda_app_con);
  if (parentId != 0 && svmMeta.launchFromCWD == 1) {
    svp_chdir(dirbuf);
    // if we do not launch from launcher, we update the path of the executable
    if (updatePath(fname_updated, fname)) {
      sda_show_error_message("Executables are only allowed in APPS folder.\n");
      return 0;
    }
    fname = fname_updated;
  }

  // move to APPS directory
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");

	// loads app
  if (sdaSvmLoadApp(fname, cacheBuffer, 0) != 0) {
    return 0;
  }

  // set metadata
  svmMeta.id = nextId;
	nextId++;
	svmMeta.parentId = parentId;
	sda_strcp(fname, svmMeta.name, sizeof(svmMeta.name));
	sda_strcp((uint8_t *)"", svmMeta.openFileName, sizeof(svmMeta.openFileName));
	svmMeta.openFileUsed = 0;
	sda_strcp((uint8_t *)"", svmMeta.openConfName, sizeof(svmMeta.openConfName));
	svmMeta.openConfUsed = 0;
	sda_strcp((uint8_t *)"", svmMeta.openCsvName, sizeof(svmMeta.openCsvName));
	svmMeta.openCsvUsed = 0;
  sda_strcp((uint8_t *)"DATA", svmMeta.currentWorkDir, sizeof(svmMeta.currentWorkDir));
	svmMeta.lcdOffButtons = 0;
	svmMeta.launchFromCWD = 0;
	wrap_set_lcdOffButtons(0);

	// move to DATA
  svp_switch_main_dir();
	svp_chdir((uint8_t *)"DATA");
	// reset gr2 context
	gr2_ResetContext(&sda_app_con);
	sdaSvmGetGR2Settings();
	// validate the app slot
	sdaSlotSetValid(4);
	sdaSlotOnTop(4);
	// insert to table of running apps
	svmSuspendAddId(svmMeta.id, svmMeta.name);
	// show the close button
	svpSGlobal.systemXBtnVisible = 1;
	svpSGlobal.systemXBtnClick = 0;
	return 1;
}

void sdaSvmCloseApp() {
	if (sdaSlotGetValid(4) == 0) {
		return;
	}
  if((errCheck(&svm) == 1) && (soft_error_flag == 0)) {
    svp_errSoftPrint(&svm);
  } else {
    if ((getOverlayId() != 0) && (soft_error_flag == 0)) {
      destroyOverlay();
    }
  }
  if (sdaSlotGetValid(4) == 1) {
    if(functionExists((uint8_t *)"exit", &svm)) {
      commExec((uint8_t *)"exit", &svm);
      if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
        errSoftPrint(&svm);
      }
    }
  }
  svp_crypto_lock();
  svmInValidate(svmMeta.id);
  sdaSetSleepLock(0);
  svpSGlobal.kbdVisible = 0;
  clearNotificationFlag();
  sda_files_close();

  if (svmMeta.parentId != 0) {
  	uint8_t argBuff[2048];

		storeArguments(argBuff, svmCallRetval, svmCallRetvalType, svmCallRetvalStr, &svm);

    //TODO: when we try to return to an already killed app, sda shows gr2 invalid elements errors
  	if (!sdaSvmLoad(svmMeta.parentId)){
  		svmValid = 0;
  		sdaSlotSetInValid(4);
  		sdaSlotOnTop(1);
  		return;
  	}

  	if(functionExists(svmCallback, &svm)) {

			restoreArguments(svmCallRetvalType, svmCallRetval, svmCallRetvalStr, &svm);

  		commExec(svmCallback, &svm);
  	}

  	sdaSvmOnTop();
  	setRedrawFlag();
  	return;
  }

  pscg_text_deactivate(&sda_app_con);
  svpSGlobal.systemXBtnVisible = 0;
  svmValid = 0;
  if (sdaGetSlotScreen(4) != 0) {
    pscg_destroy_screen(sdaGetSlotScreen(4), &sda_app_con);
  }
  sdaSlotSetInValid(4);
  sdaSlotOnTop(1);
  svp_switch_main_dir();
	svp_chdir((uint8_t *)"APPS");

}

void svmCloseAll() {
	for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
		if (svmSavedProcValid[x] == 1) {
			svmClose(svmSavedProcId[x]);
		}
	}

	if (sda_serial_is_enabled()) {
		sda_serial_disable();
	}
}

static void svmSuspendAddId(uint16_t id, uint8_t * name) {
	uint16_t index = 0;
	while (svmSavedProcValid[index] != 0) {
		if (index == MAX_OF_SAVED_PROC - 1) {
			return;
		}
		index++;
	}

	sda_strcp(name, svmSavedProcName[index], APP_NAME_LEN);
	svmSavedProcId[index] = id;
	svmSavedProcValid[index] = 1;
	svmSavedProcSingular[index] = 0;
}

uint8_t svmSuspend() {
	if(functionExists(SUSPEND_FUNCTION, &svm)) {
    commExec(SUSPEND_FUNCTION, &svm);
    if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
      errSoftPrint(&svm);
      return 1;
    }
    if (svmCheckAndExit()) {
    	return 0;
    }
  }
  sdaSvmSave();
  return 0;
}

uint8_t svmWake(uint16_t id) {
	if(id == svmMeta.id) {
		sdaSvmOnTop();
		if(functionExists(WAKEUP_FUNCTION, &svm)) {
		  commExec(WAKEUP_FUNCTION, &svm);
		  if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
		    errSoftPrint(&svm);
		    return 1;
		  }
		  if (svmCheckAndExit()) {
		  	return 0;
		  }
		}
		return 0;
	}

	svmSuspend();

	for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
		if (svmSavedProcId[x] == id && svmSavedProcValid[x] == 1) {
			sdaSvmLoad(id);
			sdaSvmOnTop();
			if(functionExists(WAKEUP_FUNCTION, &svm)) {
				commExec(WAKEUP_FUNCTION, &svm);
				if((errCheck(&svm) != 0) && (soft_error_flag == 0)) {
				  errSoftPrint(&svm);
				  return 1;
				}
				if (svmCheckAndExit()) {
					return 0;
				}
			}
			return 0;
		}
	}
	return 1;
}

static uint16_t GetIfSingular(uint8_t * name) {
	for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
		if (svmSavedProcValid[x] == 1) {
			if (strCmp(svmSavedProcName[x], name) && svmSavedProcSingular[x]) {
				return svmSavedProcId[x];
			}
		}
	}
	return 0;
}

void svmSetSingular(uint16_t id) {
	for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
		if (svmSavedProcValid[x] == 1 && svmSavedProcId[x] == id) {
			svmSavedProcSingular[x] = 1;
		}
	}
}

uint16_t svmGetSuspendedId(uint16_t id) {
	if (svmSavedProcValid[id] == 1) {
		return svmSavedProcId[id];
	}
	return 0;
}

uint8_t *svmGetSuspendedName(uint16_t id) {
	if (svmSavedProcValid[id] == 1) {
		return svmSavedProcName[id];
	}
	return (uint8_t *)"";
}

void svmClose(uint16_t id) {
	svmWake(id);
	sdaSvmCloseApp();
}

static void svmRemoveCachedFile(uint16_t id, uint8_t * tail) {
	uint8_t cacheBuffer[256];
	uint8_t numbuff[25];

	sda_int_to_str(numbuff, (int32_t)id, sizeof(numbuff));
	sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
	svp_str_add(cacheBuffer, numbuff);
	svp_str_add(cacheBuffer, tail);

	svp_unlink(cacheBuffer);
}

static void svmRemoveCachedProc(uint16_t id) {
	svmRemoveCachedFile(id, (uint8_t *) ".gr0");
	svmRemoveCachedFile(id, (uint8_t *) ".gr1");
	svmRemoveCachedFile(id, (uint8_t *) ".gr2");
	svmRemoveCachedFile(id, (uint8_t *) ".met");
	svmRemoveCachedFile(id, (uint8_t *) ".stc");
	svmRemoveCachedFile(id, (uint8_t *) ".svm");
}

static void svmInValidate(uint16_t id) {
	for (uint16_t x = 0; x < MAX_OF_SAVED_PROC; x++) {
		if (svmSavedProcId[x] == id && svmSavedProcValid[x] == 1) {
			svmSavedProcValid[x] = 0;
			svmRemoveCachedProc(x);
		}
	}
}

static void restoreArguments(uint8_t* argType, varType *arg, uint8_t **svmArgs, svsVM *s) {
	for(uint8_t z = 0; z < 3; z++) {
		s->commArgs.argType[z + 1] = argType[z];

  	if (argType[z] == 1) {
  		s->commArgs.arg[z + 1] = (varType)strNew(svmArgs[z], s);
  	} else {
  		s->commArgs.arg[z + 1] = arg[z];
  	}
  }
		s->commArgs.usedup = 3;
}

static void storeArguments(uint8_t *buff, varType *arg, uint8_t* argType, uint8_t **svmArgs, svsVM *s) {
	uint32_t x, n;
  uint8_t *prac;
  uint8_t *prac2;
  uint8_t *prac3;

	n = 0;
	prac = buff;
	for(uint8_t z = 0; z < 3; z++) {
		if (argType[z] == 1) {
			prac3 = s->stringField + arg[z].val_str;
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
	  	prac++;
	  	svmArgs[z] = prac2;
  	}
	}

}

void sdaSvmKillApp_handle() {
	pscg_text_deactivate(&sda_app_con);
	svpSGlobal.systemXBtnClick = 0;
	svpSGlobal.systemXBtnVisible = 0;
	svmValid = 0;
	svpSGlobal.kbdVisible = 0;
	if (sdaGetSlotScreen(4) != 0) {
		pscg_destroy_screen(sdaGetSlotScreen(4), &sda_app_con);
	}
	sdaSlotSetInValid(4);

	svp_crypto_lock();
	pscg_cleanup(&sda_app_con); // performs cleanup of pscg elements
	sdaSlotOnTop(1);
	svp_switch_main_dir();
	svp_chdir((uint8_t *)"APPS");
	sdaSetSleepLock(0);
}


// svm init
static void svmInitRemoveCache(uint8_t *ext){
	uint8_t buffer[255];
	uint8_t retval;

	retval = svp_extFind(buffer, 30, ext, (uint8_t *) ".");

	while (retval){
		svp_unlink(buffer);
		retval = svp_extFindNext(buffer, 30);
	}
}

void sdaSvmInit() {
	svp_switch_main_dir();
	svp_chdir((uint8_t *)"APPS/cache");
	printf("Cleaning up cached apps.\n");
	svmInitRemoveCache((uint8_t *) "gr0");
	svmInitRemoveCache((uint8_t *) "gr1");
	svmInitRemoveCache((uint8_t *) "gr2");
	svmInitRemoveCache((uint8_t *) "met");
	svmInitRemoveCache((uint8_t *) "stc");
	svmInitRemoveCache((uint8_t *) "svm");

	svp_switch_main_dir();
	svp_chdir((uint8_t *)"APPS");
	nextId = 1;
}

// run function

static uint8_t svmCheckAndExit() {
	if((svpSGlobal.systemXBtnClick) || (errCheck(&svm) != 0)) {
    sdaSvmCloseApp();
    return 1;
  }
  return 0;
}

uint16_t sdaSvmRun(uint8_t init, uint8_t top) {
	if (init){
		sdaSvmInit();
		return 0;
	}

  if (svmInit == 0) {
    //svs init call
    svmValid = 1;
    commExec((uint8_t *)"init", &svm);
    svmInit = 1;
    return 0;
  }

  sda_files_copyer();

  if (top == 1) {
    if (svs_wrap_setScr_flag == 1) {
      svs_wrap_setScr_flag = 0;
      mainScr = svs_wrap_setScr_id;
      slotScreen[4] = svs_wrap_setScr_id;
    }

    if (svmCheckAndExit()) {
    	return 0;
    }
    commExec((uint8_t *)"update", &svm);
    redrawDetect = 0;

    if (svm.handbrake == 1) {
    	sdaSvmKillApp_handle();
    	sda_show_error_message(pscgErrorString);
    	return 0;
    }

    if (flag_svmCall == 1) {
    	uint8_t argBuff[2048];

    	storeArguments(argBuff, svmCallArg, svmCallArgType, svmCallArgStr, &svm);

    	if(sdaSvmLaunch(svmCallName, svmMeta.id) == 0) {
    	  flag_svmCall = 0;
    	  return 0;
    	}

    	restoreArguments(svmCallArgType, svmCallArg, svmCallArgStr, &svm);

    	// init call here with args
    	commExec((uint8_t *)"init", &svm);
    	svmValid = 1;
    	svmInit = 1;
    	flag_svmCall = 0;
    }
  }
  return 0;
}

// call and retval

void sdaSvmCall(
		uint8_t *name,
		uint8_t *callback,
		varType arg0, uint8_t type0,
		varType arg1, uint8_t type1,
		varType arg2, uint8_t type2
	) {
	flag_svmCall = 1;
	svmCallArg[0] = arg0;
	svmCallArgType[0] = type0;
	svmCallArg[1] = arg1;
	svmCallArgType[1] = type1;
	svmCallArg[2] = arg2;
	svmCallArgType[2] = type2;

	sda_strcp(callback, svmCallback, sizeof(svmCallback));
	sda_strcp(name, svmCallName, APP_NAME_LEN);
}

void sdaSvmRetval(varType arg0, uint8_t type0, varType arg1, uint8_t type1, varType arg2, uint8_t type2) {
	svmCallRetval[0] = arg0;
	svmCallRetvalType[0] = type0;
	svmCallRetval[1] = arg1;
	svmCallRetvalType[1] = type1;
	svmCallRetval[2] = arg2;
	svmCallRetvalType[2] = type2;
}

static void sdaSvmSaver(uint16_t id, uint8_t * tail, void *target, uint32_t size) {
	uint8_t cacheBuffer[256];
	uint8_t numbuff[25];
	svp_file svmFile;

	svp_switch_main_dir();
	svp_chdir((uint8_t *)"APPS");

	sda_int_to_str(numbuff, (int32_t)id, sizeof(numbuff));
	sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
	svp_str_add(cacheBuffer, numbuff);
	svp_str_add(cacheBuffer, tail);

	if(svp_fopen_rw(&svmFile, cacheBuffer) == 0) {
		printf("sdaSvmSaver: file open error\n");
		return;
	}
	svp_fwrite(&svmFile, target, size);

	svp_fclose(&svmFile);
}

static uint8_t sdaSvmLoader(uint16_t id, uint8_t * tail, void *target, uint32_t size) {
	uint8_t cacheBuffer[256];
	uint8_t numbuff[25];
	svp_file svmFile;

	svp_switch_main_dir();
	svp_chdir((uint8_t *)"APPS");

	sda_int_to_str(numbuff, (int32_t)id, sizeof(numbuff));
	sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
	svp_str_add(cacheBuffer, numbuff);
	svp_str_add(cacheBuffer, tail);

	if(svp_fopen_read(&svmFile, cacheBuffer) == 0) {
		printf("sdaSvmLoader: file open error\n");
		return 0;
	}

	svp_fread(&svmFile, target, size);
	svp_fclose(&svmFile);

	return 1;
}

void sdaSvmSave() {
	SVScloseCache(&svm);
	if (sda_get_fr_fname() != 0)	{
		sda_strcp(sda_get_fr_fname(), svmMeta.openFileName, sizeof(svmMeta.openFileName));
		svmMeta.openFileUsed = 1;
	}
	if (sda_get_conf_fname() != 0)	{
		sda_strcp(sda_get_conf_fname(), svmMeta.openConfName, sizeof(svmMeta.openConfName));
		svmMeta.openConfUsed = 1;
	}
	if (sda_get_csv_fname() != 0)	{
		sda_strcp(sda_get_csv_fname(), svmMeta.openCsvName, sizeof(svmMeta.openCsvName));
		svmMeta.openCsvUsed = 1;
	}

  // get current wd relative to main dir
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
	//printf("restoring workdir: %s\n", svmMeta.currentWorkDir);

	svmMeta.lcdOffButtons = wrap_get_lcdOffButtons();

	sda_files_close();
	svmMeta.screen = slotScreen[4];

	sdaSvmSaver(svmMeta.id, (uint8_t *) ".svm", &svm, sizeof(svm));
	sdaSvmSaver(svmMeta.id, (uint8_t *) ".gr0", &sda_app_gr2_elements, sizeof(pscgElement) * SDA_APP_ELEM_MAX);
	sdaSvmSaver(svmMeta.id, (uint8_t *) ".gr1", &sda_app_gr2_screens, sizeof(pscgScreen) * SDA_APP_SCREEN_MAX);
	sdaSvmSaver(svmMeta.id, (uint8_t *) ".gr2", &sda_app_con, sizeof(gr2context));
	sdaSvmSaver(svmMeta.id, (uint8_t *) ".met", &svmMeta, sizeof(svmMeta));
}

uint8_t sdaSvmLoad(uint16_t id) {

	if(!sdaSvmLoader(id, (uint8_t *) ".svm", &svm, sizeof(svm)))
		return 0;

	if(!sdaSvmLoader(id, (uint8_t *) ".gr0", &sda_app_gr2_elements, sizeof(pscgElement) * SDA_APP_ELEM_MAX))
		return 0;

	if(!sdaSvmLoader(id, (uint8_t *) ".gr1", &sda_app_gr2_screens, sizeof(pscgScreen) * SDA_APP_SCREEN_MAX))
		return 0;

	if(!sdaSvmLoader(id, (uint8_t *) ".gr2", &sda_app_con, sizeof(gr2context)))
		return 0;

	if(!sdaSvmLoader(id, (uint8_t *) ".met", &svmMeta, sizeof(svmMeta)))
		return 0;

	SVSopenCache(&svm);

	svp_switch_main_dir();
	svp_chdir(svmMeta.currentWorkDir);

	if (svmMeta.openFileUsed) {
		sda_fr_fname_open(svmMeta.openFileName);
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

  svp_chdir(svmMeta.currentWorkDir);

  slotScreen[4] = svmMeta.screen;
	mainScr = slotScreen[4];
	return 1;
}
