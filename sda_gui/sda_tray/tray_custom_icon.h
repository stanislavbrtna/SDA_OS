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

#ifndef SDA_CUSTOM_ICON_H
#define SDA_CUSTOM_ICON_H

#include "sda_tray.h"

void sda_custom_icon_init();

uint8_t sda_custom_icon_set(uint8_t *img, uint16_t pid, uint8_t* cb);
int16_t sda_custom_icon_handle(int16_t x2, int16_t y1, int16_t w);

void sda_custom_icon_release_spot(uint8_t spot);
void sda_custom_icon_release_pid(uint16_t pid);

uint8_t sda_custom_icon_release_spot_pid(uint8_t spot, uint16_t pid);

void sda_custom_icon_handle_svm_events();

#endif