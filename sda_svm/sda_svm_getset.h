/*
Copyright (c) 2023 Stanislav Brtna

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

#ifndef SDA_SVM_GETSET_H
#define SDA_SVM_GETSET_H

#include "sda_svm.h"

void svmSetNocacheFlag();
uint8_t   svmGetRunning();
uint8_t * svmGetName();

void    svmSetAutocahceEnable(uint8_t val);
uint8_t svmGetAutocahceEnable();

void    svmSetCryptoUnlock(uint8_t unlock);
uint8_t svmGetCryptoUnlock();

uint16_t svmGetIfSingular(uint8_t * name);
void     svmSetSingular(uint16_t pid);

void svmKillRunning();
void svmSetError(uint8_t * str);
void svmSetLaunchCWDflag(uint8_t val);
uint64_t svmGetAppUptime();

void svmAuthorize();
uint8_t svmGetAuthorized();

void svmSetSuspendOnClose(uint8_t val);
uint8_t svmGetSuspendOnClose();

void sdaSetRedrawDetect(uint8_t val);
uint8_t sdaGetRedrawDetect();

void svmSetSuspendFlag(uint8_t val);
uint8_t svmGetSuspendFlag();

int16_t svmGetId(uint16_t pid);

// internal
void svmLaunchSetDefMetadata(uint16_t pid, uint16_t parentPid, uint8_t *fname);
void svmInValidate(uint16_t pid);
uint8_t svmGetValidPid(uint16_t pid);

uint8_t  svmGetSavedProcValid(uint16_t proc_array_index);
uint16_t svmGetSavedProcPid(uint16_t id);

void    svmSetSleepLock(uint8_t val);
uint8_t svmGetSleepLock();
void    svmSetScreenShdnLock(uint8_t val);
uint8_t svmGetScreenShdnLock();


#endif