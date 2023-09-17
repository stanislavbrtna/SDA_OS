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

#ifndef SDA_SVM_CACHE_H
#define SDA_SVM_CACHE_H
#include "sda_svm.h"
#include "sda_svm_misc.h"

void svmInitRemoveCache(uint8_t *ext);
void svmRemoveCachedProc(uint16_t id);
void svmRemoveCachedFile(uint16_t id, uint8_t * tail);
void svmPrecacheGetName(uint8_t* buffer, uint32_t len, int32_t crc, uint8_t* ext);
void svmPrecacheFile(uint8_t *fname);
uint8_t svmPreCachedExists(int32_t crc, uint32_t siz);
uint8_t svmLoadPrecached(int32_t crc);

#endif