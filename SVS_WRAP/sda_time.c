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
  //#!    sys.getTime();
  //#!Returns system time in the timestamp form.
  //#!Count of seconds from 0:0 1.1. 2007
  //#!Return: [num]Timestamp
  if (sysFuncMatch(argS->callId, "getTime", s)) {
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
    result->type = 0;
    return 1;
  }

  //#!##### Get time update flag
  //#!    sys.getTimeUpd()
  //#!Returns 1 when time update flag occurs.
  //#!Return: 1 on time update (roughly 1s)
  if (sysFuncMatch(argS->callId, "getTimeUpd", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = getTimeUpdateFlag();
    result->type = 0;
    return 1;
  }

  //#!##### Get seconds
  //#!    sys.getTimeSec();
  //#!Returns system time.
  //#!Return: [num]Seconds
  if (sysFuncMatch(argS->callId, "getTimeSec", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.sec;
    result->type = 0;
    return 1;
  }

  //#!##### Get minutes
  //#!    sys.getTimeMin();
  //#!Returns system time.
  //#!Return: [num]Minutes
  if (sysFuncMatch(argS->callId, "getTimeMin", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.min;
    result->type = 0;
    return 1;
  }

  //#!##### Get hours
  //#!    sys.getTimeHr();
  //#!Returns system time
  //#!Return: [num]Hours
  if (sysFuncMatch(argS->callId, "getTimeHr", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.hour;
    result->type = 0;
    return 1;
  }

  //#!##### Get day
  //#!    sys.getTimeDay();
  //#!Returns system time.
  //#!Return: [num]Day
  if (sysFuncMatch(argS->callId, "getTimeDay", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.day;
    result->type = 0;
    return 1;
  }

  //#!##### Get month
  //#!    sys.getTimeMonth();
  //#!Returns system time.
  //#!Return: [num]Month
  if (sysFuncMatch(argS->callId, "getTimeMonth", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.month;
    result->type = 0;
    return 1;
  }

  //#!##### Get year
  //#!    sys.getTimeYear();
  //#!Returns system time.
  //#!Return: [num]Year
  if (sysFuncMatch(argS->callId, "getTimeYear", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svpSGlobal.year;
    result->type = 0;
    return 1;
  }

  //#!##### Get uptime
  //#!    sys.getUptime();
  //#!Returns system uptime.
  //#!Return: [num]Uptime(secs)
  if (sysFuncMatch(argS->callId, "getUptime", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svp_get_uptime();
    result->type = 0;
    return 1;
  }

  //#!##### Get new timestamp
  //#!    sys.setTs([num]Year, [num]Month, [num]Day, [num]Hour, [num]Min, [num]Sec);
  //#!Returns timestamp of given time, works only for years 2007 and above.
  //#!Return: [num]Timestamp
  if (sysFuncMatch(argS->callId, "setTs", s)) {
    argType[1] = 0;
    argType[2] = 0;
    argType[3] = 0;
    argType[4] = 0;
    argType[5] = 0;
    argType[6] = 0;

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
    result->type = 0;
    return 1;
  }

  //#!##### Get seconds from timestamp
  //#!    sys.getTsSec([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Seconds
  if (sysFuncMatch(argS->callId, "getTsSec", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetSeconds(argS->arg[1].val_s);
    result->type = 0;
    return 1;
  }

  //#!##### Get minutes from timestamp
  //#!    sys.getTsMin([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Minutes
  if (sysFuncMatch(argS->callId, "getTsMin", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetMinutes(argS->arg[1].val_s);
    result->type = 0;
    return 1;
  }

  //#!##### Get hours from timestamp
  //#!    sys.getTsHr([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Seconds
  if (sysFuncMatch(argS->callId, "getTsHr", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetHours(argS->arg[1].val_s);
    result->type = 0;
    return 1;
  }

  //#!##### Get days from timestamp
  //#!    sys.getTsDay([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Days
  if (sysFuncMatch(argS->callId, "getTsDay", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetDays(argS->arg[1].val_s);
    result->type = 0;
    return 1;
  }

  //#!##### Get weekday from timestamp
  //#!    sys.getTsWkDay([num]timestamp);
  //#!Returns weekday from given timestamp.
  //#!Return: [num]Weekday (starting with monday)
  if (sysFuncMatch(argS->callId, "getTsWkDay", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetWeekDay(argS->arg[1].val_s);
    result->type = 0;
    return 1;
  }

  //#!##### Get month from timestamp
  //#!    sys.getTsMonth([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Months
  if (sysFuncMatch(argS->callId, "getTsMonth", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetMonths(argS->arg[1].val_s);
    result->type = 0;
    return 1;
  }

  //#!##### Get year from timestamp
  //#!    sys.getTsYear([num]timestamp);
  //#!Returns seconds from given timestamp.
  //#!Return: [num]Years
  if (sysFuncMatch(argS->callId, "getTsYear", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = (int32_t)sdaTimeGetYears(argS->arg[1].val_s);
    result->type = 0;
    return 1;
  }


  //#!##### Register alarm
  //#!    sys.alarmSetFixed([num]timestamp, [num]param);
  //#!Returns id of the new alarm.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "alarmSetFixed", s)) {
    argType[1] = 0;
    argType[2] = 0;
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
    result->type = 0;
    return 1;
  }

  //#!##### Register repeating alarm
  //#!    sys.alarmSetRep([num]hour, [num]min, [num]wkday, [num]day, [num]month,[num]param);
  //#!Returns id of the new alarm.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "alarmSetRep", s)) {
    argType[1] = 0;
    argType[2] = 0;
    argType[3] = 0;
    argType[4] = 0;
    argType[5] = 0;
    argType[6] = 0;
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
    result->type = 0;
    return 1;
  }

  //#!##### Remove alarm
  //#!    sys.alarmDestroy([num]id);
  //#!Returns if alarm was deleted.
  //#!Return: [num] 0 - Ok, 1 - Fail
  if (sysFuncMatch(argS->callId, "alarmDestroy", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = removeAlarm(argS->arg[1].val_s, s->fName);
    result->type = 0;
    return 1;
  }

  //#!##### Get alarm flag
  //#!    sys.alarmGetFlag();
  //#!Returns id of the new alarm.
  //#!Return: [num]flag
  if (sysFuncMatch(argS->callId, "alarmGetFlag", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = getNotificationFlag();
    result->type = 0;
    return 1;
  }

  //#!##### Clear alarm flag
  //#!    sys.alarmClrFlag();
  //#!Clears alarm flag.
  //#!Return: none
  if (sysFuncMatch(argS->callId, "alarmClrFlag", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    clearNotificationFlag();
    result->type = 0;
    return 1;
  }

  //#!##### Get alarm id
  //#!    sys.alarmGetId();
  //#!Returns id of the curent alarm.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "alarmGetId", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = getNotificationId();
    result->type = 0;
    return 1;
  }

  //#!##### Get alarm parameter
  //#!    sys.alarmGetPar();
  //#!Returns parameter of the current alarm.
  //#!Return: [num]param
  if (sysFuncMatch(argS->callId, "alarmGetPar", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = getNotificationParam();
    result->type = 0;
    return 1;
  }

  return 0;
}
