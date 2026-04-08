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

#include "sda_media.h"

static uint32_t currentFid;

uint32_t sda_media_play(uint8_t* fname) {
  
#ifndef SDA_FEATURE_PCM_SOUND
  return 0;
#else
  sda_base_media_play(fname);
  currentFid++;
  return currentFid;
#endif
}

uint8_t sda_media_stop(uint32_t fid) {
#ifndef SDA_FEATURE_PCM_SOUND
  return 0;
#else
  if(currentFid != fid) {
    return 0;
  }
  sda_base_media_stop();
  return 1;
#endif
}

uint8_t sda_media_seek(uint32_t seek_s, uint32_t fid) {
#ifndef SDA_FEATURE_PCM_SOUND
  return 0;
#else

  if(currentFid != fid) {
    return 0;
  }

  sda_base_media_seek(seek_s);

  return 1;
#endif
}

uint8_t  sda_media_getPos(uint32_t fid) {
#ifndef SDA_FEATURE_PCM_SOUND
  return 0;
#else

  if(currentFid != fid) {
    return 0;
  }

  return sda_base_media_getPos();
#endif
}

uint8_t sda_media_pause(uint8_t pause_on, uint32_t fid) {
#ifndef SDA_FEATURE_PCM_SOUND
  return 0;
#else
  if(currentFid != fid) {
    return 0;
  }

  sda_base_media_pause(pause_on);
  return 1;
#endif
}

uint32_t sda_media_get() {
  return currentFid;
}