/*
Copyright (c) 2021 Stanislav Brtna

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

#ifndef SDA_SYSTEM_STRINGS_H
#define SDA_SYSTEM_STRINGS_H
#include "../SDA_OS.h"

uint32_t  sda_str_add(uint8_t *str, uint8_t *str2);
void sda_setbuff(uint8_t * source, uint8_t * target);
void sda_int_to_str(uint8_t * buff, int32_t val, uint32_t len);
uint32_t  sda_strcp(uint8_t *in, uint8_t *out, uint32_t len);
uint32_t sda_strlen(uint8_t * str);
void sda_time_to_str(uint8_t * buff, uint32_t val);
uint16_t sda_str_insert(uint8_t *index1, uint8_t *index2, uint8_t *buff, uint16_t pos, uint16_t len);
uint32_t sda_str_find(uint8_t* str, uint8_t* pattern);

#endif