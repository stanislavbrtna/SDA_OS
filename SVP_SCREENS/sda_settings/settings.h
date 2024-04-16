/*
Copyright (c) 2020 Stanislav Brtna

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

#ifndef SDA_SETTINGS_SUBSCREENS_H
#define SDA_SETTINGS_SUBSCREENS_H

#include "../../SDA_OS.h"

uint16_t sda_settings_info_screen(uint8_t init);
uint16_t sda_settings_debug_screen(uint8_t init);
uint16_t sda_settings_notif_screen(uint8_t init);
uint16_t sda_settings_time_screen(uint8_t init);
uint16_t sda_settings_display_screen(uint8_t init);
uint16_t sda_settings_color_screen(uint8_t init);
uint16_t sda_settings_security_screen(uint8_t init);

#define SDA_SETTINGS_SPACER 40

void svp_settings_set_spacing(uint16_t id);

void sda_settings_stack_add(uint16_t scr, uint8_t * label);

#endif
