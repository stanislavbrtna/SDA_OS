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

#ifndef SVM_SUBPROC_H
#define SVM_SUBPROC_H

#include "sda_svm.h"
#include "sda_svm_misc.h"

uint8_t * svmGetCallback();
uint8_t svmRunPerformCall();

// Set argument from out of SVM
void svmSetArgumentNum(uint8_t id, int32_t val);
void svmSetArgumentStr(uint8_t id, uint8_t* val);
void svmSetArgumentFlt(uint8_t id, float val);

void svmCallSubProc(
    uint8_t *name,
    uint8_t *callback,
    varType arg0, uint8_t type0,
    varType arg1, uint8_t type1,
    varType arg2, uint8_t type2
  );
void svmSetSubProcRetval(varType arg0, uint8_t type0, varType arg1, uint8_t type1, varType arg2, uint8_t type2);
void svmStoreArguments(uint8_t *buff, varType *arg, uint8_t* argType, uint8_t **svmArgs, svsVM *s);
void svmRestoreArguments(uint8_t* argType, varType *arg, uint8_t **svmArgs, svsVM *s);
void svmClearArguments(svsVM *s);

void svmSaveProcData();
uint8_t svmLoadProcData(uint16_t id);

uint8_t sdaSvmLoader(uint16_t id, uint8_t * tail, void *target, uint32_t size);
void sdaSvmSaver(uint16_t id, uint8_t * tail, void *target, uint32_t size);

uint8_t svmLoadPrevious();

#endif