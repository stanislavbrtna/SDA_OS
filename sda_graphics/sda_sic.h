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

#ifndef SDA_SIC_H
#define SDA_SIC_H

#include "../SDA_OS.h"

void sda_draw_sic(int16_t x2, int16_t y1, uint16_t front_color, uint16_t back_color, uint8_t * icon);
uint8_t sda_draw_sic_file(int16_t x1, int16_t y1, uint16_t front_color, uint16_t back_color, uint8_t * fname);
uint8_t sda_sic_get_last_width();
uint8_t sda_sic_get_width(uint8_t * fname);
uint8_t sda_sic_get_height(uint8_t * fname);

#endif