/*
Copyright (c) 2019 Stanislav Brtna

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
#ifndef SDA_FS_WRAPPER
#define SDA_FS_WRAPPER

#include "../SDA_OS.h"

void sda_files_close();
uint8_t * sda_get_conf_fname();
uint8_t * sda_get_csv_fname();
uint8_t * sda_get_db_fname();
uint8_t sda_fr_fname_reopen(uint16_t index);
uint8_t sda_fr_fname_open(uint16_t index, uint8_t * fname);
uint8_t sda_files_conf_open(uint8_t * fname);
uint8_t sda_files_csv_open(uint8_t * fname);
uint8_t sda_files_db_open(uint8_t * fname);

void sda_files_copyer();

void sda_files_wrapper_init();

// internal wrapper functions
void sda_files_close_conf_csv();

void sda_files_reset();

#endif
