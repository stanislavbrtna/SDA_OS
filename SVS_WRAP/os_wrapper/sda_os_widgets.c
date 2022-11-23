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

#include "sda_os_wrapper.h"


uint8_t sda_os_cal_widget_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];
  static dateSelectorWidgetType date;

  //#!
  //#!#### Date selector widget
  //#!

  //#!##### Init calendar widget
  //#!    sys.w.cal.init([num]year, [num]month, [num]day) #return: cal screen ID
  //#!Creates callendar widget screen.
  //#!With given year, month and day.
  //#!Return: [num]Callendar widget screen id.
  if (sysFuncMatch(argS->callId, "init", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }
    result->value.val_u = date_select_widget_init(&date, argS->arg[1].val_s, argS->arg[2].val_s, argS->arg[3].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Select date
  //#!    sys.w.cal.select([num]year, [num]month, [num]day);
  //#!Sets year, month and day to callendar widget.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "select", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }
    date_select_widget_set_date(&date, argS->arg[1].val_s, argS->arg[2].val_s, argS->arg[3].val_s);
    return 1;
  }

  //#!##### Update
  //#!    sys.w.cal.update();
  //#!Updates callendar widget.
  //#!Return: [num]1 when callendar is clicked.
  if (sysFuncMatch(argS->callId, "update", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = date_select_widget_update(&date);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Mark day
  //#!    sys.w.cal.mark([num]day);
  //#!Marks day in callendar widget.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "mark", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    date_select_highlight(&date, argS->arg[1].val_s);
    return 1;
  }

  //#!##### Set highlighting
  //#!    sys.w.cal.highlight([num]val);
  //#!Enable that all buttons except marked are rendered as ghost buttons.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "highlight", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    date_select_set_highlight(&date, (uint8_t) argS->arg[1].val_s);
    return 1;
  }

  //#!##### Get selected day
  //#!    sys.w.cal.getDay();
  //#!Returns selected day.
  //#!Return: [num]day
  if (sysFuncMatch(argS->callId, "getDay", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = date_select_widget_get_day(&date);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}

uint8_t sda_counter_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Counters

  //#!##### Set counter
  //#!    sys.cnt.set([num] ms);
  //#!Sets system timer, it counts down and stops at zero.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "set", s)) {
    argType[1] = SVS_TYPE_NUM; //ms
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    set_sda_counter(argS->arg[1].val_s);
    return 1;
  }

  //#!##### Gets counter
  //#!    sys.cnt.get();
  //#!Gets system timer value
  //#!Return: value of system timer
  if (sysFuncMatch(argS->callId, "get", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = get_sda_counter();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}
