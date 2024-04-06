/*
Copyright (c) 2024 Stanislav Brtna

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

#ifndef SDA_RESOURCE_LOCK_H
#define SDA_RESOURCE_LOCK_H

#include "../SDA_OS.h"

#define SDA_RESOURCES_MAX 3

typedef enum {
  EXTERNAL_EXPANSION_PORT = 1,
  INTERNAL_EXPANSION_PORT = 2
} sdaResource;

void sda_resource_lock_init();

uint8_t sda_resource_claim(sdaResource res, uint16_t pid);
uint8_t sda_resource_free(sdaResource res, uint16_t pid);
void sda_resource_free_pid(uint16_t pid);
sdaLockState sda_resource_get_lock(sdaResource res, uint16_t pid);

#endif