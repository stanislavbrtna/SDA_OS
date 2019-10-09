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
#ifndef SDA_SVS_WRAPPER
#define SDA_SVS_WRAPPER

#include "../SDA_OS.h"

uint8_t wrap_get_lcdOffButtons();
void wrap_set_lcdOffButtons(uint8_t val);

void sx_set_touch_ev(gr2EventType ev, uint16_t x, uint16_t y);

void svsSVPWrapInit();
void svsDirectSWrapInit();

uint8_t sda_os_sound_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_hw_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_counter_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_crypto_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_time_alarm_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_time_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_overlay_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_overlay_time_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_overlay_date_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_gui_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_cal_widget_wrapper(varRetVal *result, argStruct *argS, svsVM *s);

#endif
