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

#ifndef SDA_SVM_WAKE_SUSPEND_H
#define SDA_SVM_WAKE_SUSPEND_H

#include "sda_svm.h"
#include "sda_svm_misc.h"

// Gets Pid from id
uint16_t svmGetSuspendedPid(uint16_t id);

// Gets name from id
uint8_t *svmGetSuspendedName(uint16_t id);

// inits new entry in process table
void svmSuspendInitPid(uint16_t pid, uint8_t * name);

void svmSuspend();
uint8_t svmStoreRunning();
uint8_t svmWake(uint16_t id);
// wakes app and performs wakeup function with given arguments
uint8_t svmWakeArgs(uint16_t pid, uint8_t* argType, varType *arg, uint8_t **svmArgs);

// executes the suspend call
uint8_t svmExecSuspend();

void svmHandleHomeButton();

#endif