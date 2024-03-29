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
static int32_t alarmId;
static int32_t alarmParam;
static uint8_t alarmFlag;

static uint8_t redrawDetect;

// svs
uint8_t svs_wrap_setScr_flag;
uint16_t svs_wrap_setScr_id;

extern svsVM          svm;
extern sdaSvmMetadata svmMeta;
extern uint8_t * pscgErrorString;


void svmGetGR2Settings() {
  // load colors from system to app context
  gr2_set_border_color(gr2_get_border_color(&sda_sys_con), &sda_app_con);
  gr2_set_text_color(gr2_get_text_color(&sda_sys_con), &sda_app_con);
  gr2_set_background_color(gr2_get_background_color(&sda_sys_con), &sda_app_con);
  gr2_set_fill_color(gr2_get_fill_color(&sda_sys_con), &sda_app_con);
  gr2_set_active_color(gr2_get_active_color(&sda_sys_con), &sda_app_con);
}


void sda_alarm_set_flag(int32_t id, int32_t param) {
  alarmId = id;
  alarmParam = param;
  alarmFlag = 1;
}


int32_t sda_alarm_get_id() {
  return alarmId;
}


int32_t sda_alarm_get_param() {
  return alarmParam;
}


int8_t sda_alarm_get_flag() {
  return alarmFlag;
}


void sda_alarm_clear_flag() {
  alarmFlag = 0;
}


void svmSetMainScreen(uint16_t val) {
  svs_wrap_setScr_id = val;
  svs_wrap_setScr_flag = 1;
  gr2_set_modified(val, &sda_app_con);
}


void sdaSetRedrawDetect(uint8_t val) {
  redrawDetect = val;
}


uint8_t sdaGetRedrawDetect() {
  return redrawDetect;
}

void svmAuthorize() {
  svmMeta.authorized = 1;
}

uint8_t svmGetAuthorized() {
  return svmMeta.authorized;
}


uint16_t svmGetPid() {
  if (svmGetValid()) {
    return svmMeta.pid;
  } else {
    return 0;
  }
}


void svmSetLandscape(uint8_t val) {
  svmMeta.landscape = val;
}


// beep callback init & handler
void svmSetBeepCallback(uint8_t * cb, uint32_t time) {
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


void svmOnTop() {
  sda_slot_on_top(4);
  svp_switch_main_dir();
  svp_chdir(svmMeta.currentWorkDir);

  if (svmMeta.landscape != svpSGlobal.lcdLandscape) {
    sda_set_landscape(svmMeta.landscape);
  }

  svpSGlobal.systemXBtnVisible = 1;
  svpSGlobal.systemXBtnClick = 0;
}


void svmKillRunning() {
  svm.handbrake = 1;
}


void svmSetError(uint8_t * str) {
  pscgErrorString = str;
}


void svmSetLaunchCWDflag(uint8_t val) {
  svmMeta.launchFromCWD = val;
}


uint64_t svmGetAppUptime() {
  return svmMeta.loadUptime;
}


// Crypto
void svmSetCryptoUnlock(uint8_t unlock) {
  svmMeta.cryptoUnlocked = unlock;
}


uint8_t svmGetCryptoUnlock() {
  return svmMeta.cryptoUnlocked;
}


uint8_t svmCheckAndExit() {
  if((svpSGlobal.systemXBtnClick) || (errCheck(&svm) != 0)) {
    svmCloseRunning();
    return 1;
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
