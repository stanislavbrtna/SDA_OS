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

#include "sda_wrapper.h"

uint8_t sda_time_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!
  //#!#### Time API
  //#!

  //#!##### Get time
  //#!    sys.time.get();
  //#!Returns system time in the timestamp form.
  //#!Count of seconds from 0:0 1.1. 2007
  //#!Return: [num]Timestamp
  if (sysFuncMatch(argS->callId, "get", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = sdaTimeGetTimestamp(
      svpSGlobal.year,
      svpSGlobal.month,
      svpSGlobal.day,
      svpSGlobal.hour,
      svpSGlobal.min,
      svpSGlobal.sec
    );
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get time update flag
  //#!    sys.time.getUpd()
  //#!Returns 1 when time update flag occurs.
  //#!Return: 1 on time update (roughly 1s)
  if (sysFuncMatch(argS->callId, "getUpd", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = getTimeUpdateFlag();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get seconds
  //#!    sys.time.getSec();
  //#!Returns system time.
  //#!Return: [num]Seconds
  if (sysFuncMatch(argS->callId, "getSec", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.sec;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get minutes
  //#!    sys.time.getMin();
  //#!Returns system time.
  //#!Return: [num]Minutes
  if (sysFuncMatch(argS->callId, "getMin", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.min;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get hours
  //#!    sys.time.getHr();
  //#!Returns system time
  //#!Return: [num]Hours
  if (sysFuncMatch(argS->callId, "getHr", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.hour;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get day
  //#!    sys.time.getDay();
  //#!Returns system time.
  //#!Return: [num]Day
  if (sysFuncMatch(argS->callId, "getDay", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.day;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get month
  //#!    sys.time.getMonth();
  //#!Returns system time.
  //#!Return: [num]Month
  if (sysFuncMatch(argS->callId, "getMonth", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.month;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get year
  //#!    sys.time.getYear();
  //#!Returns system time.
  //#!Return: [num]Year
  if (sysFuncMatch(argS->callId, "getYear", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.year;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get uptime
  //#!    sys.time.getUptime();
  //#!Returns system uptime.
  //#!Return: [num]Uptime(secs)
  if (sysFuncMatch(argS->callId, "getUptime", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = sda_get_uptime();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get uptime in miliseconds
  //#!    sys.time.getAppUptime();
  //#!Returns milisecond uptime of the app.
  //#!Return: [num]Uptime(miliseconds)
  if (sysFuncMatch(argS->callId, "getAppUptime", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = (int32_t) (svpSGlobal.uptimeMs - svmGetAppUptime());
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get new timestamp
  //#!    sys.time.setTs([num]Year, [num]Month, [num]Day, [num]Hour, [num]Min, [num]Sec);
  //#!Returns timestamp of given time, works only for years 2007 and above.
  //#!Return: [num]Timestamp
  if (sysFuncMatch(argS->callId, "setTs", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;
    argType[5] = SVS_TYPE_NUM;
    argType[6] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 6, s)) {
      return 0;
    }
    result->value.val_s = sdaTimeGetTimestamp(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      argS->arg[5].val_s,
      argS->arg[6].val_s
    );
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get seconds from timestamp
  //#!    sys.time.getTsSec([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Seconds
  if (sysFuncMatch(argS->callId, "getTsSec", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetSeconds(argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get minutes from timestamp
  //#!    sys.time.getTsMin([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Minutes
  if (sysFuncMatch(argS->callId, "getTsMin", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetMinutes(argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get hours from timestamp
  //#!    sys.time.getTsHr([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Seconds
  if (sysFuncMatch(argS->callId, "getTsHr", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetHours(argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get days from timestamp
  //#!    sys.time.getTsDay([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Days
  if (sysFuncMatch(argS->callId, "getTsDay", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetDays(argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get weekday from timestamp
  //#!    sys.time.getTsWkDay([num]timestamp);
  //#!Returns weekday from given timestamp.
  //#!Return: [num]Weekday (starting with monday)
  if (sysFuncMatch(argS->callId, "getTsWkDay", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetWeekDay(argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get month from timestamp
  //#!    sys.time.getTsMonth([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Months
  if (sysFuncMatch(argS->callId, "getTsMonth", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetMonths(argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get year from timestamp
  //#!    sys.time.getTsYear([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Years
  if (sysFuncMatch(argS->callId, "getTsYear", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetYears(argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!#### Timer API
  //#!
  //#!System timer will call the callback after given time, it will wake up the app if it is in the background. You can push app to foreground by calling sys.os.arise(); from the callback.

  //#!##### Set timer
  //#!    sys.time.setTimer([num]time_ms, [str]callaback);
  //#!Sets the timer.
  //#!Return: none
  if (sysFuncMatch(argS->callId, "setTimer", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    sdaSvmSetTimer(argS->arg[1].val_s, s->stringField + argS->arg[2].val_str);

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Clear timer
  //#!    sys.time.clearTimer();
  //#!Clears the timer if it is running.
  //#!Return: none
  if (sysFuncMatch(argS->callId, "clearTimer", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sdaSvmClearTimer();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}


uint8_t sda_time_alarm_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Alarm API
  //#!Alarm API creates system handled, repeatable alarms, that will be stored during reboots and will automatically launch the app.

  //#!##### Register alarm
  //#!    sys.alarm.setFixed([num]timestamp, [num]param);
  //#!Returns id of the new alarm.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "setFixed", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    result->value.val_s = sdaRegisterAlarm(
      s->fName,
      argS->arg[1].val_s,
      0,
      0,
      0,
      0,
      0,
      argS->arg[2].val_s
    );
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Register repeating alarm
  //#!    sys.alarm.setRep([num]hour, [num]min, [num]wkday, [num]day, [num]month,[num]param);
  //#!Returns id of the new alarm.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "setRep", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;
    argType[5] = SVS_TYPE_NUM;
    argType[6] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 6, s)) {
      return 0;
    }

    result->value.val_s = sdaRegisterAlarm(
      s->fName,
      0,
      argS->arg[1].val_s, //hour
      argS->arg[2].val_s, //min
      argS->arg[3].val_s, //wkday
      argS->arg[4].val_s, //day
      argS->arg[5].val_s, //month
      argS->arg[6].val_s  //param
    );
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Remove alarm
  //#!    sys.alarm.destroy([num]id);
  //#!Returns if alarm was deleted.
  //#!Return: [num] 0 - Ok, 1 - Fail
  if (sysFuncMatch(argS->callId, "destroy", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = removeAlarm(argS->arg[1].val_s, s->fName);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get alarm flag
  //#!    sys.alarm.getFlag();
  //#!Returns id of the new alarm.
  //#!Return: [num]flag
  if (sysFuncMatch(argS->callId, "getFlag", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = getNotificationFlag();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Clear alarm flag
  //#!    sys.alarm.clrFlag();
  //#!Clears alarm flag.
  //#!Return: none
  if (sysFuncMatch(argS->callId, "clrFlag", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    clearNotificationFlag();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get alarm id
  //#!    sys.alarm.getId();
  //#!Returns id of the curent alarm.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "getId", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = getNotificationId();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get alarm parameter
  //#!    sys.alarm.getPar();
  //#!Returns parameter of the current alarm.
  //#!Return: [num]param
  if (sysFuncMatch(argS->callId, "getPar", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = getNotificationParam();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}