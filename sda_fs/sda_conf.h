/*
Copyright (c) 2018 Stanislav Brtna

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

#ifndef SDA_CONF_H
#define SDA_CONF_H
#include "svp_fs.h"

#define MAX_KEY_LEN 128

typedef struct {
  svp_file fil;
  uint8_t valid;
  uint8_t escaping;
}sda_conf;

uint8_t sda_conf_open(sda_conf *fc, uint8_t * fname);
uint8_t sda_conf_close(sda_conf *fc);

void sda_conf_set_escaping(sda_conf *fc, uint8_t val);

uint8_t sda_conf_key_exists(sda_conf *fc, uint8_t* key);

uint8_t sda_conf_key_read(sda_conf *fc, uint8_t* key, uint8_t* ret_buff, uint16_t len);
int32_t sda_conf_key_read_i32(sda_conf *fc, uint8_t* key, int32_t def);
void sda_conf_key_write(sda_conf *fc, uint8_t* key, uint8_t* val_buff);
void sda_conf_key_write_i32(sda_conf *fc, uint8_t* key, int32_t val);

void sda_conf_key_remove(sda_conf *fc, uint8_t* key);

uint8_t sda_conf_key_contains(sda_conf *fc, uint8_t* key, uint8_t* value, uint8_t case_sensitive);

#endif
