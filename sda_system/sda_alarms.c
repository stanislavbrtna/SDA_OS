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

#include "sda_alarms.h"

//#define SDA_ALARMS_DEBUG

static uint8_t currentAlarmAppName[APP_NAME_LEN];
static int32_t currentAlarmTime;
static int32_t notifyAlarmTime;
static int32_t currentAlarmId;
static int32_t currentAlarmParam;

static int32_t resolveReapeating(uint8_t hour, uint8_t min, uint8_t wkday, uint8_t day, uint8_t month, int32_t last);


void sdaPrintActiveAlarm() {
  if (currentAlarmTime != 0) {
    printf("Current alarm: id: %d app: %s time: ", (int) currentAlarmId, currentAlarmAppName);
    sdaTimePrint(currentAlarmTime);
    printf("\n");
  } else {
    printf("Current notification: None\n");
  }
}


uint8_t sdaGetActiveAlarm() {
  if (currentAlarmTime) {
    return 1;
  } else {
    return 0;
  }
}


static void sdaReloadAlarmIcon() {
  if (notifyAlarmTime != 0) {
    if (notifyAlarmTime - svpSGlobal.timestamp <= 24*60*60) {
      sda_tray_alarm_enable();
      return;
    }
  }

  sda_tray_alarm_disable();
}


int32_t getValNum(uint8_t *name, int32_t id, sda_conf * pConffile) {
  uint8_t keybuff[25];
  uint8_t numbuff[25];

  sda_int_to_str(numbuff, id, sizeof(numbuff));
  
  sda_strcp(name, keybuff, sizeof(keybuff));
  sda_str_add(keybuff, (uint8_t *) "_");
  sda_str_add(keybuff, numbuff);
  return sda_conf_key_read_i32(pConffile, keybuff, 0);
}


uint8_t getValStr(
  uint8_t *name,
  int32_t id, 
  uint8_t * buffer, 
  uint32_t len, 
  sda_conf * pConffile
) {
  uint8_t keybuff[25];
  uint8_t numbuff[25];

  sda_int_to_str(numbuff, id, sizeof(numbuff));
  
  sda_strcp(name, keybuff, sizeof(keybuff));
  sda_str_add(keybuff, (uint8_t *) "_");
  sda_str_add(keybuff, numbuff);

  return sda_conf_key_read(pConffile, keybuff, buffer, len);
}


void setValNum(
  uint8_t *name, 
  int32_t id, 
  int32_t value, 
  sda_conf * pConffile
) {
  uint8_t keybuff[25];
  uint8_t numbuff[25];

  sda_int_to_str(numbuff, id, sizeof(numbuff));
  
  sda_strcp(name, keybuff, sizeof(keybuff));
  sda_str_add(keybuff, (uint8_t *) "_");
  sda_str_add(keybuff, numbuff);

  sda_conf_key_write_i32(pConffile, keybuff, value);
}


int32_t sdaRegisterAlarm(
    uint8_t * appname,
    int32_t timestamp,
    uint8_t hour,
    uint8_t min,
    uint8_t wkday,
    uint8_t day,
    uint8_t month,
    int32_t param
){

  uint8_t dirbuf[258];
  uint8_t keybuff[25];
  uint8_t numbuff[25];
  int32_t maxId;
  sda_conf conffile;

  svp_getcwd(dirbuf, sizeof(dirbuf));
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"sda_alarms.cfg") == 0) {
    printf("Failed to open notification config file\n");
  }

  maxId = 1;

  sda_int_to_str(numbuff, maxId, sizeof(numbuff));
  sda_strcp((uint8_t *) "appname_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);

  while (sda_conf_key_exists(&conffile, keybuff)) {
    maxId++;
    sda_int_to_str(numbuff, maxId, sizeof(numbuff));
    sda_strcp((uint8_t *) "appname_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
  }

  // now we have empty new id
  sda_conf_key_write(&conffile, keybuff, appname);

  if (timestamp != 0) {
    sda_strcp((uint8_t *) "tfix_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_write_i32(&conffile, keybuff, timestamp);
  } else {
    sda_strcp((uint8_t *) "ttime_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_write_i32(&conffile, keybuff, hour*100 + min);

    sda_strcp((uint8_t *) "twkday_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_write_i32(&conffile, keybuff, wkday);

    sda_strcp((uint8_t *) "tday_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_write_i32(&conffile, keybuff, day);

    sda_strcp((uint8_t *) "tmonth_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_write_i32(&conffile, keybuff, month);

    sda_strcp((uint8_t *) "last_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_write_i32(&conffile, keybuff, svpSGlobal.timestamp);
  }

  sda_strcp((uint8_t *) "param_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_write_i32(&conffile, keybuff, param);

  sda_strcp((uint8_t *) "notify_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_write_i32(&conffile, keybuff, 1);

  if (sda_conf_key_read_i32(&conffile, (uint8_t *)"maxId", 0) < maxId) {
    sda_conf_key_write_i32(&conffile, (uint8_t *)"maxId", maxId);
  }

  sda_conf_close(&conffile);

  svp_chdir(dirbuf);

  sdaReloadAlarms();

  return maxId;
}


void sdaReloadAlarms() {
  uint8_t dirbuf[258];
  uint8_t keybuff[25];
  uint8_t numbuff[25];
  int32_t currentId;
  int32_t maxId = 1;

  int32_t  alarmTime  = 0;
  uint32_t alarmId    = 0;
  int32_t  alarmParam = 0;

  notifyAlarmTime = 0;
  
  sda_conf conffile;

  svp_getcwd(dirbuf, sizeof(dirbuf));
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"sda_alarms.cfg") == 0) {
    printf("Failed to open notification config file\n");
  }

  currentId = 1;
  maxId = sda_conf_key_read_i32(&conffile, (uint8_t *)"maxId", 16);

  sda_int_to_str(numbuff, currentId, sizeof(numbuff));
  sda_strcp((uint8_t *) "appname_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);

  while (currentId < maxId) {
    int32_t time = 0;
    uint8_t hour = 0;
    uint8_t min = 0;
    uint8_t wkday = 0;
    uint8_t day = 0;
    uint8_t month = 0;
    int32_t last = 0;
    int32_t param = 0;
    int32_t notify = 0;

    if(!sda_conf_key_exists(&conffile, keybuff)) {
      currentId++;
      sda_int_to_str(numbuff, currentId, sizeof(numbuff));
      sda_strcp((uint8_t *) "appname_", keybuff, sizeof(keybuff));
      sda_str_add(keybuff, numbuff);
      continue;
    }

    sda_strcp((uint8_t *) "tfix_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    time = sda_conf_key_read_i32(&conffile, keybuff, 0);

    sda_strcp((uint8_t *) "param_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    param = sda_conf_key_read_i32(&conffile, keybuff, 0);

    sda_strcp((uint8_t *) "notify_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    notify = sda_conf_key_read_i32(&conffile, keybuff, 1);

    if (time == 0) {
      int32_t timeTmp = 0;

      sda_strcp((uint8_t *) "ttime_", keybuff, sizeof(keybuff));
      sda_str_add(keybuff, numbuff);
      timeTmp = sda_conf_key_read_i32(&conffile, keybuff, 0);

      hour = timeTmp / 100;

      min = timeTmp % 100;

      sda_strcp((uint8_t *) "twkday_", keybuff, sizeof(keybuff));
      sda_str_add(keybuff, numbuff);
      wkday = sda_conf_key_read_i32(&conffile, keybuff, 0);

      sda_strcp((uint8_t *) "tday_", keybuff, sizeof(keybuff));
      sda_str_add(keybuff, numbuff);
      day = sda_conf_key_read_i32(&conffile, keybuff, 0);

      sda_strcp((uint8_t *) "tmonth_", keybuff, sizeof(keybuff));
      sda_str_add(keybuff, numbuff);
      month = sda_conf_key_read_i32(&conffile, keybuff, 0);

      sda_strcp((uint8_t *) "last_", keybuff, sizeof(keybuff));
      sda_str_add(keybuff, numbuff);
      last = sda_conf_key_read_i32(&conffile, keybuff, svpSGlobal.timestamp);

      time = resolveReapeating(hour, min, wkday, day, month, last);

      if (time == 0) {
        printf("notif test: repeat false");
      }
    }

#ifdef SDA_ALARMS_DEBUG
    printf("comp: id: %u, tim: ", maxId);
    sdaTimePrint(time);
    printf("\n");
#endif
    if (time != 0) {
      if (alarmTime == 0) {
        alarmTime = time;
        alarmParam = param;
        alarmId = currentId;
      } else {
        if (time < alarmTime) {
          alarmTime = time;
          alarmParam = param;
          alarmId = currentId;
        }
      }

      if(notify) {
        if(notifyAlarmTime == 0) {
          notifyAlarmTime = time;
        } else if(time < notifyAlarmTime) {
          notifyAlarmTime = time;
        }
      }
    }
#ifdef SDA_ALARMS_DEBUG
    printf("time: %d\n", alarmId);
#endif

    currentId++;
    sda_int_to_str(numbuff, currentId, sizeof(numbuff));
    sda_strcp((uint8_t *) "appname_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
  }

  sda_int_to_str(numbuff, alarmId, sizeof(numbuff));
  sda_strcp((uint8_t *) "appname_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_read(&conffile, keybuff, currentAlarmAppName, APP_NAME_LEN);

  currentAlarmTime  = alarmTime;
  currentAlarmId    = alarmId;
  currentAlarmParam = alarmParam;

  sda_conf_close(&conffile);
  svp_chdir(dirbuf);

  sdaReloadAlarmIcon();
}


uint8_t wkdayInMask(uint8_t mask, int32_t time) {
  uint8_t day = sdaTimeGetWeekDay(time);

  if(((mask & 1) && day == 1)
    || ((mask & 2) && day == 2)
    || ((mask & 4) && day == 3)
    || ((mask & 8) && day == 4)
    || ((mask & 16) && day == 5)
    || ((mask & 32) && day == 6)
    || ((mask & 64) && day == 7)
  ) {
    return 1;
  }
  return 0;
}


static int32_t resolveReapeating(uint8_t hour, uint8_t min, uint8_t wkday, uint8_t day, uint8_t month, int32_t last) {
  int32_t alarmTime = 0;
  uint8_t tmpMonth = 0;
  uint8_t tmpDay = 0;
  // todays timstamp

  if (month == 0) {
    tmpMonth = sdaTimeGetMonths(svpSGlobal.timestamp);
  } else {
    tmpMonth = month;
  }

  if (day == 0) {
    tmpDay = sdaTimeGetDays(svpSGlobal.timestamp);
  } else {
    tmpDay = day;
  }

  alarmTime = sdaTimeGetTimestamp(sdaTimeGetYears(svpSGlobal.timestamp), tmpMonth, tmpDay, hour, min, 0);

  if (alarmTime > last && (wkday == 0 || wkdayInMask(wkday, svpSGlobal.timestamp))) {
    // event is today
    return alarmTime;
  } else {
    // event occured, get time of next event
    if ( month == 0 && day == 0 && wkday == 0) {
      return alarmTime + 24 * 3600;
    } else {
      uint8_t timeFound = 0;
      while (!timeFound) {
        alarmTime += 24 * 3600;
        if (month != 0 && sdaTimeGetMonths(alarmTime) != month) {
          continue;
        }

        if (day != 0 && sdaTimeGetDays(alarmTime) != day) {
          continue;
        }

        if (wkday != 0 && !wkdayInMask(wkday, alarmTime)) {
          continue;
        }

        timeFound = 1;

        if (alarmTime < 0) {
          return 0;
        }
      }
      return alarmTime;
    }
  }
  return 0;
}


void sdaResolveAlarm() {
  uint8_t dirbuf[258];
  uint8_t keybuff[25];
  uint8_t numbuff[25];
  sda_conf conffile;

  svp_getcwd(dirbuf, sizeof(dirbuf));
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"sda_alarms.cfg") == 0) {
    printf("Failed to open alarm config file\n");
  }

  // delete static or update last time of repeating

  sda_int_to_str(numbuff, currentAlarmId, sizeof(numbuff));

  sda_strcp((uint8_t *) "tfix_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);

  if (sda_conf_key_exists(&conffile, keybuff)) {
    sda_strcp((uint8_t *) "appname_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_remove(&conffile, keybuff);
    sda_strcp((uint8_t *) "tfix_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_remove(&conffile, keybuff);

    sda_strcp((uint8_t *) "param_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_remove(&conffile, keybuff);

  } else {
    sda_strcp((uint8_t *) "last_", keybuff, sizeof(keybuff));
    sda_str_add(keybuff, numbuff);
    sda_conf_key_write_i32(&conffile, keybuff, svpSGlobal.timestamp);
  }

  sda_conf_close(&conffile);
  svp_chdir(dirbuf);
}


uint8_t sdaGetCurentAlarm(int32_t * id, int32_t * param, uint8_t * appNameBuffer, uint16_t size) {
  if (currentAlarmTime != 0 && svpSGlobal.timestamp >= currentAlarmTime) {
    sda_strcp(currentAlarmAppName, appNameBuffer, size);
    *id = currentAlarmId;
    *param = currentAlarmParam;
    sdaResolveAlarm();
    sdaReloadAlarms();
    return 1;
  }

  if (currentAlarmTime != 0) {
    sdaReloadAlarmIcon();
  }
  
  return 0;
}


uint8_t removeAlarm(int32_t id, uint8_t * appName) {
  uint8_t dirbuf[258];
  uint8_t keybuff[25];
  uint8_t numbuff[25];
  uint8_t confAppName[APP_NAME_LEN];
  sda_conf conffile;

  if (id == 0) {
    printf("Warning: removeAlarm: zero id!\n");
    return 0;
  }

  svp_getcwd(dirbuf, sizeof(dirbuf));
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"sda_alarms.cfg") == 0) {
    printf("Failed to open notification config file\n");
  }

  // check appname
  sda_int_to_str(numbuff, id, sizeof(numbuff));
  sda_strcp((uint8_t *) "appname_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);

  sda_conf_key_read(&conffile, keybuff, confAppName, APP_NAME_LEN);

  if (! strCmp(appName, confAppName)) {
    printf("Warning: Notification delete: appnames does not match!");
    sda_conf_close(&conffile);
    svp_chdir(dirbuf);
    return 1;
  }

  sda_strcp((uint8_t *) "notify_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_remove(&conffile, keybuff);

  sda_strcp((uint8_t *) "appname_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_remove(&conffile, keybuff);

  sda_strcp((uint8_t *) "tfix_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_remove(&conffile, keybuff);

  sda_strcp((uint8_t *) "param_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_remove(&conffile, keybuff);

  sda_strcp((uint8_t *) "last_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_remove(&conffile, keybuff);

  sda_strcp((uint8_t *) "ttime_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_remove(&conffile, keybuff);

  sda_strcp((uint8_t *) "twkday_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_remove(&conffile, keybuff);

  sda_strcp((uint8_t *) "tday_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_remove(&conffile, keybuff);

  sda_strcp((uint8_t *) "tmonth_", keybuff, sizeof(keybuff));
  sda_str_add(keybuff, numbuff);
  sda_conf_key_remove(&conffile, keybuff);

  sda_conf_close(&conffile);
  svp_chdir(dirbuf);

  sdaReloadAlarms();

  return 0;
}


int32_t getAlarmParam(int32_t id, uint8_t * appName) {
  uint8_t dirbuf[258];
  uint8_t confAppName[APP_NAME_LEN];
  sda_conf conffile;

  if (id == 0) {
    printf("%s: Warning: removeAlarm: zero id!\n", __FUNCTION__);
    return 0;
  }

  svp_getcwd(dirbuf, sizeof(dirbuf));
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"sda_alarms.cfg") == 0) {
    printf("Failed to open notification config file\n");
  }

  getValStr((uint8_t *) "appname", id, confAppName, APP_NAME_LEN, &conffile);

  if (!strCmp(appName, confAppName)) {
    printf("%s:Warning: appnames does not match!", __FUNCTION__);
    sda_conf_close(&conffile);
    svp_chdir(dirbuf);
    return 1;
  }

  int32_t param = getValNum("param", id, &conffile);

  sda_conf_close(&conffile);
  svp_chdir(dirbuf);

  return param;
}


uint8_t getAlarmValid(int32_t id, uint8_t * appName) {
  uint8_t dirbuf[258];
  uint8_t confAppName[APP_NAME_LEN];
  sda_conf conffile;

  if (id == 0) {
    printf("%s: Warning: removeAlarm: zero id!\n", __FUNCTION__);
    return 0;
  }

  svp_getcwd(dirbuf, sizeof(dirbuf));
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"sda_alarms.cfg") == 0) {
    printf("Failed to open notification config file\n");
  }

  // alarm does not exist
  if (!getValStr((uint8_t *) "appname", id, confAppName, APP_NAME_LEN, &conffile)) {
    sda_conf_close(&conffile);
    svp_chdir(dirbuf);
    return 0;
  }

  // names does not match
  if (!strCmp(appName, confAppName)) {
    printf("%s:Warning: appnames does not match!", __FUNCTION__);
    sda_conf_close(&conffile);
    svp_chdir(dirbuf);
    return 0;
  }
  sda_conf_close(&conffile);
  svp_chdir(dirbuf);

  // ok
  return 1;
}


uint8_t setAlarmNotify(int32_t id, uint8_t * appName, int32_t value) {
  uint8_t dirbuf[258];
  uint8_t confAppName[APP_NAME_LEN];
  sda_conf conffile;

  if (id == 0) {
    printf("%s: Warning: removeAlarm: zero id!\n", __FUNCTION__);
    return 0;
  }

  svp_getcwd(dirbuf, sizeof(dirbuf));
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"sda_alarms.cfg") == 0) {
    printf("Failed to open notification config file\n");
  }

  // alarm does not exist
  if (!getValStr((uint8_t *) "appname", id, confAppName, APP_NAME_LEN, &conffile)) {
    sda_conf_close(&conffile);
    svp_chdir(dirbuf);
    return 0;
  }

  // names does not match
  if (!strCmp(appName, confAppName)) {
    printf("%s:Warning: appnames does not match!", __FUNCTION__);
    sda_conf_close(&conffile);
    svp_chdir(dirbuf);
    return 0;
  }

  setValNum((uint8_t *)"notify", id, value, &conffile);

  sda_conf_close(&conffile);
  svp_chdir(dirbuf);

  sdaReloadAlarms();
  
  // ok
  return 1;
}


