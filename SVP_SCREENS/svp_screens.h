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

#ifndef SVP_SCREENS_H
#define SVP_SCREENS_H

#include "../SDA_OS.h"

extern uint16_t slotScreen[];
extern uint16_t slotOnTop[];
extern uint16_t slotValid[];
extern uint16_t mainScr;

uint16_t svp_homeScreen(uint8_t init, uint8_t top);
uint16_t svp_appScreen(uint8_t init, uint8_t top);
uint16_t svp_optScreen(uint8_t init, uint8_t top);

void sda_homescreen_configure();

void sda_settings_open_security();

void taskSwitcherOpen();
void taskSwitcherUpdate();

#endif
