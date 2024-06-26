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

uint8_t sda_overlay_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Overlay API

  //#!##### Set overlay screen
  //#!    sys.o.setScr([num]screen_id);
  //#!Sets overlay screen, returns overlay id.
  //#!
  //#!Return: [num] Overlay id
  if (sysFuncMatch(argS->callId, "setScr", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = setOverlayScreen((uint16_t) argS->arg[1].val_s, &sda_app_con);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get overlay id
  //#!    sys.o.getId();
  //#!Gets id of current overlay.
  //#!
  //#!Return: [num] OverlayId
  if (sysFuncMatch(argS->callId, "getId", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s =  getOverlayId();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Destroy overlay
  //#!    sys.o.destroy();
  //#!Destroys current overlay, also destroys its screen.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "destroy", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    destroyOverlay();
    setRedrawFlag();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set position and size of overlay
  //#!    sys.o.setXYXY([num]x1, [num]y1, [num]x2, [num]y2);
  //#!Sets position and size of current overlay (in screen pixels).
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setXYXY", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 4, s)) {
      return 0;
    }
    setOverlayX1(argS->arg[1].val_s);
    setOverlayY1(argS->arg[2].val_s);
    setOverlayX2(argS->arg[3].val_s);
    setOverlayY2(argS->arg[4].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }


  //#!##### Set overlay height
  //#!    sys.o.setY([num]val);
  //#!Sets lower coordinates of current overlay (in screen pixels).
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setY", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    setOverlayY2((uint16_t) argS->arg[1].val_s);
    return 1;
  }

  return 0;
}


uint8_t sda_overlay_date_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!
  //#!#### Date overlay
  //#!

  //#!##### Create date overlay
  //#!    sys.o.date.add([num]year, [num]month, [num]day);
  //#!Creates date overlay id, returns id
  //#!
  //#!Return: [num]Date overlay id
  if (sysFuncMatch(argS->callId, "add", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }

    result->value.val_s = date_overlay_init(
      (uint16_t)argS->arg[1].val_s,
      (uint16_t)argS->arg[2].val_s,
      (uint16_t)argS->arg[3].val_s
    );
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Update date overlay
  //#!    sys.o.date.update([num]id);
  //#!Updates date overlay
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "update", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    date_overlay_update((uint16_t)argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get ok from overlay
  //#!    sys.o.date.getOk([num]id);
  //#!Gets if overlay ok button was pressed.
  //#!
  //#!Return: 1 if overlay ok button was pressed
  if (sysFuncMatch(argS->callId, "getOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = date_overlay_get_ok((uint16_t) argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Clear ok from overlay
  //#!    sys.o.date.clrOk([num]id);
  //#!Clears ok flag from overlay
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "clrOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    date_overlay_clear_ok((uint16_t) argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get year form overlay
  //#!    sys.o.date.getYr([num]id);
  //#!Gets year from overlay with given id.
  //#!
  //#!Return: [num]Year
  if (sysFuncMatch(argS->callId, "getYr", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = date_overlay_get_year((uint16_t)argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get day from overlay
  //#!    sys.o.date.getDay([num]id);
  //#!Gets day from overlay with given id.
  //#!
  //#!Return: [num]Day
  if (sysFuncMatch(argS->callId, "getDay", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = date_overlay_get_day((uint16_t)argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get month from overlay
  //#!    sys.o.date.getMon([num]id);
  //#!Gets month from overlay with given id.
  //#!
  //#!Return: [num]Month
  if (sysFuncMatch(argS->callId, "getMon", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = date_overlay_get_month((uint16_t)argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}

uint8_t sda_overlay_time_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!
  //#!#### Time overlay
  //#!

  //#!##### Create time overlay
  //#!    sys.o.time.add();
  //#!    sys.o.time.add([num]hr, [num]min);
  //#!Adds a time overlay, returns its id
  //#!
  //#!Return: [num]overlay id
  if (sysFuncMatch(argS->callId, "add", s)) {

    if (argS->usedup == 2) {
      argType[1] = SVS_TYPE_NUM;
      argType[2] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 2, s)) {
        return 0;
      }

      result->value.val_s = time_overlay_init();
      result->type = SVS_TYPE_NUM;
      time_overlay_set_time(
        (uint16_t)result->value.val_s,
        (uint16_t)argS->arg[1].val_s,
        (uint16_t)argS->arg[2].val_s
      );
      return 1;
    } else {
      if(sysExecTypeCheck(argS, argType, 0, s)) {
        return 0;
      }
      result->value.val_s = time_overlay_init();
      result->type = SVS_TYPE_NUM;
      return 1;
    }
  }

  //#!##### Set time overlay time
  //#!    sys.o.time.set([num]overlay_id, [num]hr, [num]min);
  //#!Sets time in overlay with given id.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "set", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }
    time_overlay_set_time(
      (uint16_t)argS->arg[1].val_s,
      (uint16_t)argS->arg[2].val_s,
      (uint16_t)argS->arg[3].val_s
    );
    return 1;
  }

  //#!##### Update time overlay
  //#!    sys.o.time.update([num]overlay_id);
  //#!Updates time overlay.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId,"update", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    time_overlay_update((uint16_t) argS->arg[1].val_s);
    return 1;
  }

  //#!##### Get time overlay ok
  //#!    sys.o.time.getOk([num]overlay_id);
  //#!Gets ok from time overlay.
  //#!
  //#!Return: 1 if ok was pressed
  if (sysFuncMatch(argS->callId, "getOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = time_overlay_get_ok((uint16_t)argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get time overlay minutes
  //#!    sys.o.time.getMin([num]overlay_id);
  //#!Returns minutes from overlay.
  //#!
  //#!Return: [num]Minutes
  if (sysFuncMatch(argS->callId, "getMin", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = time_overlay_get_minutes((uint16_t)argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get time overlay hours
  //#!    sys.o.time.getHr([num]overlay_id);
  //#!Returns hours from overlay.
  //#!
  //#!Return: [num]Hours
  if (sysFuncMatch(argS->callId, "getHr", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = time_overlay_get_hours((uint16_t)argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Clear ok
  //#!    sys.o.time.clrOk([num]overlay_id);
  //#!Clears ok from time overlay
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "clrOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    time_overlay_clear_ok((uint16_t)argS->arg[1].val_s);
    return 1;
  }

  return 0;
}


uint8_t sda_overlay_color_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!
  //#!#### Color overlay
  //#!

  //#!##### Create color overlay
  //#!    sys.o.color.add([num]color);
  //#!Adds a color overlay, returns its id
  //#!
  //#!Return: [num]overlay id
  if (sysFuncMatch(argS->callId, "add", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = color_overlay_init();
    result->type = SVS_TYPE_NUM;
    color_overlay_set_color(
      (uint16_t)result->value.val_s,
      (uint16_t)argS->arg[1].val_s
    );
    return 1;
  }

  //#!##### Set color overlay color
  //#!    sys.o.color.set([num]overlay_id, [num]color);
  //#!Sets color in overlay with given id.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "set", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    color_overlay_set_color(
      (uint16_t)argS->arg[1].val_s,
      (uint16_t)argS->arg[2].val_s
    );
    return 1;
  }

  //#!##### Update color overlay
  //#!    sys.o.color.update([num]overlay_id);
  //#!Updates color overlay.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId,"update", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    color_overlay_update((uint16_t) argS->arg[1].val_s);
    return 1;
  }

  //#!##### Get color overlay ok
  //#!    sys.o.color.getOk([num]overlay_id);
  //#!Gets ok from color overlay.
  //#!
  //#!Return: 1 if ok was pressed
  if (sysFuncMatch(argS->callId, "getOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = color_overlay_get_ok((uint16_t)argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get color overlay value
  //#!    sys.o.color.getCol([num]overlay_id);
  //#!Returns color from overlay.
  //#!
  //#!Return: [num]Color
  if (sysFuncMatch(argS->callId, "getCol", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = color_overlay_get_color((uint16_t)argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Clear ok
  //#!    sys.o.color.clrOk([num]overlay_id);
  //#!Clears ok from the overlay
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "clrOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    color_overlay_clear_ok((uint16_t)argS->arg[1].val_s);
    return 1;
  }

  return 0;
}