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

#ifndef SDA_UTIL_H
#define SDA_UTIL_H
#include "../SDA_OS.h"

// \n is 10 on linux systems, but takes two bytes on windows, so it is hardcoded
// to 10, both for csv and cfg
#define SVP_ENDLINE 10
#define SVP_SEPARATOR '|'

void svp_ppm_set_pmc(uint8_t enable, uint16_t color);
void draw_ppm(uint16_t x,uint16_t y, uint8_t scale, uint8_t *filename);
uint16_t ppm_get_width(uint8_t *filename);

uint8_t sda_draw_p16(uint16_t x, uint16_t y, uint8_t *filename);

void svp_calibrator();

#endif
