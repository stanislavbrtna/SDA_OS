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

#ifndef SDA_NOTIFICATIONS_H
#define SDA_NOTIFICATIONS_H
#include "../SDA_OS.h"

uint8_t removeAlarm(int32_t id, uint8_t * appName);

uint8_t sdaGetCurentAlarm(int32_t * id, int32_t * param, uint8_t * appNameBuffer, uint16_t size);

void sdaResolveAlarm();
void sdaReloadAlarms();

int32_t sdaRegisterAlarm(
    uint8_t * appname,
    int32_t timestamp,
    uint8_t hour,
    uint8_t min,
    uint8_t wkday,
    uint8_t day,
    uint8_t month,
    int32_t param
);

void sdaPrintActiveAlarm();

// Returns 1 if there is an active alarm
uint8_t sdaGetActiveAlarm();

#endif
