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

#ifndef SDA_TRAY_H
#define SDA_TRAY_H
#include "../../SDA_OS.h"
#include "tray_custom_icon.h"

#define OPT_HOLD_CNT_MAX 12
#define OPT_HOLD_CNT_BEGIN 5

void svp_set_irq_redraw();

uint8_t svp_tray();
uint8_t svp_irq();

void sda_tray_alarm_enable();
void sda_tray_alarm_disable();

void irq_redraw_block_enable();
void irq_redraw_block_disable();

// internal functions

gr2EventType sda_tray_clicked(
  uint16_t x1,
  uint16_t y1,
  uint16_t x2,
  uint16_t y2
);

int16_t sda_tray_time(int16_t x1, int16_t y1, uint16_t w);
int16_t sda_tray_battery(int16_t x1, int16_t y1, int16_t w);
int16_t sda_tray_home(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

#endif
