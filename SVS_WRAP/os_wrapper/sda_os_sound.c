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
  //#!If 0 is given as frequency_hz, only callback will be performed afrer given duration.
  //#!If empty string is passed as callback, no callback will be performed. 
  //#!
  //#!Return: None
   if (sysFuncMatch(argS->callId, "beepC", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_STR;

    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }
    if(argS->arg[1].val_u != 0) {
      svp_beep_set_pf(argS->arg[1].val_u);
      svp_beep_set_t(argS->arg[2].val_u);
      svp_beep();
    }
    svmBeepSetCallback(s->stringField + argS->arg[3].val_str, argS->arg[2].val_u);
    return 1;
  }

  //#!##### Set beep param to default
  //#!    sys.snd.beepDef();
  //#!Sets beep to its default values.
  //#!(1000hz, 250ms)
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

  //#!##### Get if haptics is enabled
  //#!    sys.snd.getHaptics();
  //#!Returns system haptics
  //#!
  //#!Return: [num]1 if haptics is enabled.
  if (sysFuncMatch(argS->callId, "getHaptics", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
#ifdef SDA_FEATURE_NOTIF_VIBRO
    result->value.val_u = svpSGlobal.haptics;
#else
    result->value.val_u = 0;
#endif
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Perform haptic feedbach
  //#!    sys.snd.haptics([num]time (~ms));
  //#!Performs haptic feedback for a given time. (max 1s)
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "haptics", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if(argS->arg[1].val_u <= 1000) {
      svp_haptic_fb(argS->arg[1].val_u);
    }
    return 1;
  }

  //#!##### Play media
  //#!    sys.snd.play([str] string);
  //#!Plays given wav file (PCM sound must be supported by the device).
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "play", s)) {
    argType[1] = SVS_TYPE_STR; // file

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    sda_media_play(s->stringField + argS->arg[1].val_str);

    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Stop playback
  //#!    sys.snd.stop();
  //#!Stops media playback
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "stop", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }

    sda_media_stop();

    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Seek
  //#!    sys.snd.seek([num]secs);
  //#!Seeks in current media to given timestamp (in seconds).
  //#!
  //#!Return: [num] 0 - ok, 1 - error
  if (sysFuncMatch(argS->callId, "seek", s)) {
    argType[1] = SVS_TYPE_NUM;
    
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_media_seek(argS->arg[1].val_u);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get playback position
  //#!    sys.snd.getPos();
  //#!Get media playback position in seconds.
  //#!
  //#!Return: [num] position in seconds
  if (sysFuncMatch(argS->callId, "getPos", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }

    result->value.val_u = sda_media_getPos();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media duration
  //#!    sys.snd.getDuration([str] fileName);
  //#!Returns media duration in seconds.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "getDuration", s)) {
    argType[1] = SVS_TYPE_STR; // file

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_media_getDuration(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media bitrate
  //#!    sys.snd.getBitRate([str] fileName);
  //#!Returns media bitrate.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "getBitRate", s)) {
    argType[1] = SVS_TYPE_STR; // file

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_media_getBitRate(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media sample rate
  //#!    sys.snd.getSmplRate([str] fileName);
  //#!Returns media sample rate.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "getSmplRate", s)) {
    argType[1] = SVS_TYPE_STR; // file

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_media_getSampleRate(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media output type
  //#!    sys.snd.getDevice();
  //#!Returns if current playback device is speaker or headphones.
  //#!
  //#!Return: [num] 0 - speaker, 1 - headphones
  if (sysFuncMatch(argS->callId, "getDevice", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }

    result->value.val_u = svpSGlobal.outputPCM;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media playback volume
  //#!    sys.snd.getVolume();
  //#!Returns volume of the current output device in range 0 - 1000.
  //#!
  //#!Return: [num] volume
  if (sysFuncMatch(argS->callId, "getVolume", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }

    result->value.val_u = sda_get_volume();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set volume
  //#!    sys.snd.setVolume([num]value);
  //#!Sets playback volume for current output device.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "setVolume", s)) {
    argType[1] = SVS_TYPE_NUM;
    
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    sda_set_volume(argS->arg[1].val_u);

    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set pause
  //#!    sys.snd.setPause([num]pauseOn);
  //#!Pauses and resumes media playback.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "setPause", s)) {
    argType[1] = SVS_TYPE_NUM;
    
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    sda_media_pause(argS->arg[1].val_u);

    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}