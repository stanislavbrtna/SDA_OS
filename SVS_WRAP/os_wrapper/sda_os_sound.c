/*
Copyright (c) 2017 Stanislav Brtna

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


uint8_t sda_os_sound_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Sound

  //#!##### Beep the speaker
  //#!    sys.snd.beep();
  //#!Initiates system beep.
  //#!
  //#!Return: None
   if (sysFuncMatch(argS->callId, "beep", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_beep();
    return 1;
  }

  //#!##### Beep the speaker with callback
  //#!    sys.snd.beepC([num] frequency_hz, [num] duration_ms, [str] callback);
  //#!Makes sound of given frequency and duration, calls given callback afterwards.
  //#!Internally calls sys.snd.beepTime and sys.snd.beepFreq, so calling sys.snd.beep();
  //#!will produce tone with frequency nad duration of last sys.snd.beepC call.
  //#!
  //#!Return: None
   if (sysFuncMatch(argS->callId, "beepC", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_STR;

    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }
    svp_beep_set_pf(argS->arg[1].val_u);
    svp_beep_set_t(argS->arg[2].val_u);
    svp_beep();
    svmBeepSetCallback(s->stringField + argS->arg[3].val_str, argS->arg[2].val_u);
    return 1;
  }

  //#!##### Set beep param to default
  //#!    sys.snd.beepDef();
  //#!Sets beep to its default values.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "beepDef", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_beep_set_def();
    return 1;
  }

  //#!##### Set the duration
  //#!    sys.snd.beepTime([num]time (~ms));
  //#!Sets lenght of beep.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "beepTime", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_beep_set_t(argS->arg[1].val_u);
    return 1;
  }

  //#!##### Set the frequency
  //#!    sys.snd.beepFreq([num]frequency (Hz));
  //#!Sets frequency of the beep in Hz in range from 27 to 20000.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "beepFreq", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_beep_set_pf(argS->arg[1].val_u);
    return 1;
  }

  //#!##### Get if system sound is disabled
  //#!    sys.snd.getMute();
  //#!Returns system mute
  //#!
  //#!Return: [num]1 if system is on mute.
  if (sysFuncMatch(argS->callId, "getMute", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = svpSGlobal.mute;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}