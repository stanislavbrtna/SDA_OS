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

#ifndef SDA_SVM_MISC
#define SDA_SVM_MISC
#include "sda_svm.h"
#include "sda_svm_subproc.h"

void svmSetDrawRoot(uint8_t * str);
void svmOnTop();

void    svmAlarmSetFlag(int32_t id, int32_t param);
int8_t  svmAlarmGetFlag();
void    sdaAlarmClearFlag();

int32_t svmAlarmGetId();
int32_t svmAlarmGetParam();

void    svmBeepSetCallback(uint8_t * cb, uint32_t time);
uint8_t svmBeepHandler();

uint8_t svmRegisterUartCallback(uint8_t* callback, uint8_t val);
uint8_t svmHandleUartCallbacks();
uint8_t svmGetUartCallbackActive();

void    svmSetSuspendOnClose(uint8_t val);
uint8_t svmGetSuspendOnClose();

uint8_t svmHandleNotifIconCallback(uint16_t pid, uint8_t * callback);

// internal svm stuff
void svmInit();
uint8_t svmCheckAndExit();

void svmStoreArguments(uint8_t *buff, varType *arg, uint8_t* argType, uint8_t **svmArgs, svsVM *s);
void svmRestoreArguments(uint8_t* argType, varType *arg, uint8_t **svmArgs, svsVM *s);

#endif