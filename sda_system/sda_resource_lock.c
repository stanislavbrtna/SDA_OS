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

#include "sda_resource_lock.h"

sdaLockState sda_resource_locks[SDA_RESOURCES_MAX];
uint16_t     sda_resource_pids[SDA_RESOURCES_MAX];


void sda_resource_lock_init() {
  uint32_t i = 0;

  for (i = 0; i < SDA_RESOURCES_MAX; i++) {
    sda_resource_locks[i] = SDA_LOCK_UNLOCKED;
    sda_resource_pids[i]  = 0;
  }
}

uint8_t sda_resource_claim(sdaResource res, uint16_t pid) {
  if (sda_resource_locks[res] == SDA_LOCK_LOCKED && sda_resource_locks[res] != pid) {
    printf("%s: Resource id %u already locked!\n", __FUNCTION__, res);
    return 1;
  }

  sda_resource_locks[res] = SDA_LOCK_LOCKED;
  sda_resource_pids[res]  = pid;
  return 0;
}


uint8_t sda_resource_free(sdaResource res, uint16_t pid) {
  if (sda_resource_locks[res] == SDA_LOCK_UNLOCKED) {
    return 0;
  }

  if (sda_resource_locks[res] == SDA_LOCK_LOCKED) {
    // app that locked it is unlocking it
    if (sda_resource_pids[res] == pid) {
      sda_resource_locks[res] = SDA_LOCK_UNLOCKED;
      return 0;
    }

    // os is unlocking it
    if (pid == 0) {
      sda_resource_locks[res] = SDA_LOCK_UNLOCKED;
      return 0;
    }

    printf("%s: Trying to unlock id %u without permissions!\n", __FUNCTION__, res);
    return 1;
  }
}


void sda_resource_free_pid(uint16_t pid) {
  uint32_t i = 0;

  for (i = 0; i < SDA_RESOURCES_MAX; i++) {
    if (sda_resource_pids[i] == pid) {
      sda_resource_locks[i] = SDA_LOCK_UNLOCKED;
    }
  }
}


sdaLockState sda_resource_get_lock(sdaResource res, uint16_t pid) {
  if (sda_resource_locks[res] == SDA_LOCK_UNLOCKED) {
    return SDA_LOCK_UNLOCKED;
  }
  
  if (sda_resource_locks[res] == SDA_LOCK_LOCKED && sda_resource_pids[res] == pid && pid != 0) {
    return SDA_LOCK_UNLOCKED;
  }

  return SDA_LOCK_LOCKED;
}


uint16_t sda_resource_get_pid(sdaResource res) {
  if (sda_resource_locks[res] == SDA_LOCK_UNLOCKED) {
    return 0;
  }

  return sda_resource_pids[res];
}