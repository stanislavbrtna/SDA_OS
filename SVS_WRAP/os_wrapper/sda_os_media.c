/*
Copyright (c) 2026 Stanislav Brtna

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


uint8_t sda_os_media_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

#ifndef SDA_FEATURE_PCM_SOUND
  errSoft((uint8_t *)"SDA_FEATURE_PCM_SOUND is not supported on this device.", s);
  return 0;
#else

  //#!#### PCM audio playback
  //#!This is an optional feature based on feature flag *SDA_FEATURE_PCM_SOUND*.
  //#!Only one file can be played at a time, there is no sound mixer available.

  //#!##### Play media
  //#!    sys.pcm.play([str] string);
  //#!Plays given wav file (PCM sound must be supported by the device).
  //#!Supported file formats: mp3, wav
  //#!
  //#!Return: playback id
  if (sysFuncMatch(argS->callId, "play", s)) {
    argType[1] = SVS_TYPE_STR; // file

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_media_play(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Stop playback
  //#!    sys.pcm.stop([num] id);
  //#!Stops media playback
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "stop", s)) {
    argType[1] = SVS_TYPE_NUM; // file

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_media_stop(argS->arg[1].val_u);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Seek
  //#!    sys.pcm.seek([num]secs, [num] id);
  //#!Seeks in current media to given timestamp (in seconds).
  //#!
  //#!Return: [num] 0 - ok, 1 - error
  if (sysFuncMatch(argS->callId, "seek", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    
    if(sysExecTypeCheck(argS, argType, 2, s)){
      return 0;
    }

    result->value.val_u = sda_media_seek(argS->arg[1].val_u, argS->arg[2].val_u);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media Id
  //#!    sys.pcm.getId();
  //#!Get id of the currently playing media.
  //#!
  //#!Return: [num] id
  if (sysFuncMatch(argS->callId, "getId", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }

    result->value.val_u = sda_media_get();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media state
  //#!    sys.pcm.getState([num] id);
  //#!Get media playback state
  //#! 0 - Unavailable
  //#! 1 - Playback
  //#! 2 - Paused
  //#! 3 - Stopped
  //#! 4 - Finished
  //#!
  //#!Return: [num] status
  if (sysFuncMatch(argS->callId, "getState", s)) {
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_media_getStatus(argS->arg[1].val_u);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get playback position
  //#!    sys.pcm.getPos([num] id);
  //#!Get media playback position in seconds.
  //#!
  //#!Return: [num] position in seconds
  if (sysFuncMatch(argS->callId, "getPos", s)) {
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_media_getPos(argS->arg[1].val_u);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media duration
  //#!    sys.pcm.getDuration([str] fileName);
  //#!Returns media duration in seconds.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "getDuration", s)) {
    argType[1] = SVS_TYPE_STR; // file

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_base_media_getDuration(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media bitrate
  //#!    sys.pcm.getBitRate([str] fileName);
  //#!Returns media bitrate.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "getBitRate", s)) {
    argType[1] = SVS_TYPE_STR; // file

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_base_media_getBitRate(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media sample rate
  //#!    sys.pcm.getSmplRate([str] fileName);
  //#!Returns media sample rate.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "getSmplRate", s)) {
    argType[1] = SVS_TYPE_STR; // file

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = sda_base_media_getSampleRate(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media output type
  //#!    sys.pcm.getDevice();
  //#!Returns if current playback device is speaker or headphones.
  //#!
  //#!Return: [num] 1 - speaker, 2 - headphones
  if (sysFuncMatch(argS->callId, "getDevice", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }

    if(svpSGlobal.outputPCM == SPEAKER) {
      result->value.val_u = 1;
    } else if(svpSGlobal.outputPCM == HEADPHONES) {
      result->value.val_u = 2;
    } else {
      result->value.val_u = 0;
    }

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get media playback volume
  //#!    sys.pcm.getVolume();
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
  //#!    sys.pcm.setVolume([num]value);
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
  //#!    sys.pcm.setPause([num]pauseOn, [num]id);
  //#!Pauses and resumes media playback.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "setPause", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    
    if(sysExecTypeCheck(argS, argType, 2, s)){
      return 0;
    }

    result->value.val_u = sda_media_pause(argS->arg[1].val_u, argS->arg[2].val_u);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
#endif
}