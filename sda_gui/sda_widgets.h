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

#ifndef SDA_WIDGETS_H
#define SDA_WIDGETS_H
#include "../SDA_OS.h"

typedef struct {
  uint16_t dateSelectorId;
  uint8_t dayCount;
  uint8_t startDay;
  uint16_t buttons[43];
  uint8_t selectedDay;
} dateSelectorWidgetType;

extern const uint8_t *date_month_names[];
extern const uint8_t date_month_lenghts[];
extern const uint8_t * date_days_strs[];

//return: pscg screen kde date widget sedí
uint16_t date_select_widget_init(dateSelectorWidgetType *d, uint16_t year, uint8_t month, uint8_t day);

// nastavuje datum widgetu
void date_select_widget_set_date(dateSelectorWidgetType *d, uint16_t year, uint8_t month, uint8_t day);

//return: 1 pokud bylo kliknuto, jinak 0
uint16_t date_select_widget_update(dateSelectorWidgetType *d);

// higliths day
uint16_t date_select_highlight(dateSelectorWidgetType *d, uint8_t day);

//return: date of selected day
uint8_t date_select_widget_get_day(dateSelectorWidgetType *d);

//misc date functions
void svp_write_date_string(uint8_t *str, uint8_t month_as_num, uint8_t write_year);

#endif
