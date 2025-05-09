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

#include "sda_os_icons.h"

uint8_t back[] = {
  0x02,
  0x1c, //img_width
  0x1c, //img_height
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x8,0x0,0x0,0xf0,0x0,0x0,0xc0,
  0xf,0x0,0x80,0xff,0x0,0x0,0xfe,0xff,
  0x7,0xe0,0xff,0x7f,0x0,0xf8,0xf,0x0,
  0x0,0xfc,0x0,0x0,0x0,0xf,0x0,0x0,
  0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0
};


uint8_t forward[] = {
  0x02,
  0x1c, //img_width
  0x1c, //img_height
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x1,0x0,0x0,0xf0,0x0,0x0,0x0,
  0x3f,0x0,0x0,0xf0,0x1f,0x0,0xfe,0xff,
  0x7,0xe0,0xff,0x7f,0x0,0x0,0xff,0x1,
  0x0,0xf0,0x3,0x0,0x0,0xf,0x0,0x0,
  0x10,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0
};


uint8_t none[] = {
  0x02,
  0x1c, //img_width
  0x1c, //img_height
  0x0,0x0,0x0,0x80,0xff,0xff,0x1f,0xfc,
  0xff,0xff,0xe3,0xff,0xff,0x7f,0x1e,0x0,
  0x80,0xe7,0x0,0x0,0x70,0xe,0xf8,0x1,
  0xe7,0xc0,0x3f,0x70,0xe,0xfe,0x7,0xe7,
  0xe0,0x78,0x70,0xe,0x0,0x7,0xe7,0x0,
  0x38,0x70,0xe,0xc0,0x3,0xe7,0x0,0x1c,
  0x70,0xe,0xe0,0x0,0xe7,0x0,0xf,0x70,
  0xe,0x70,0x0,0xe7,0x0,0x0,0x70,0xe,
  0x0,0x0,0xe7,0x0,0x7,0x70,0xe,0xf8,
  0x0,0xe7,0x0,0x7,0x70,0xe,0x0,0x0,
  0xe7,0x1,0x0,0x78,0xfe,0xff,0xff,0xc7,
  0xff,0xff,0x3f,0xf8,0xff,0xff,0x1,0x0,
  0x0,0x0
};


uint8_t * sda_get_icon(uint8_t ic) {
  if(ic == SDA_ICON_BACK) {
    return back;
  }

  if(ic == SDA_ICON_FORWARD) {
    return forward;
  }

  return none;
}