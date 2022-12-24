/*
Copyright (c) 2022 Stanislav Brtna

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

#include "sda_svm_load_save.h"

uint8_t sdaSvmLoader(uint16_t id, uint8_t * tail, void *target, uint32_t size) {
  uint8_t cacheBuffer[256];
  uint8_t numbuff[25];
  svp_file svmFile;

  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");

  sda_int_to_str(numbuff, (int32_t)id, sizeof(numbuff));
  sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
  sda_str_add(cacheBuffer, numbuff);
  sda_str_add(cacheBuffer, tail);

  if(svp_fopen_read(&svmFile, cacheBuffer) == 0) {
    printf("sdaSvmLoader: file open error (%s)\n", cacheBuffer);
    return 0;
  }

  svp_fread(&svmFile, target, size);
  svp_fclose(&svmFile);

  return 1;
}


void sdaSvmSaver(uint16_t id, uint8_t * tail, void *target, uint32_t size) {
  uint8_t cacheBuffer[256];
  uint8_t numbuff[25];
  svp_file svmFile;

  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");

  sda_int_to_str(numbuff, (int32_t)id, sizeof(numbuff));
  sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
  sda_str_add(cacheBuffer, numbuff);
  sda_str_add(cacheBuffer, tail);

  if(svp_fopen_rw(&svmFile, cacheBuffer) == 0) {
    printf("sdaSvmSaver: file open error\n");
    return;
  }
  svp_fwrite(&svmFile, target, size);

  svp_fclose(&svmFile);
}
