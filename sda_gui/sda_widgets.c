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

#include "sda_widgets.h"

// date select widget
//aby to šlo 1 až 12
const uint8_t *date_month_names[] = {
  (uint8_t *)"",
  OVRL_MON1,
  OVRL_MON2,
  OVRL_MON3,
  OVRL_MON4,
  OVRL_MON5,
  OVRL_MON6,
  OVRL_MON7,
  OVRL_MON8,
  OVRL_MON9,
  OVRL_MON10,
  OVRL_MON11,
  OVRL_MON12
};

//únor 28 nebo 29, třeba vyrobit funkci s detekcí přechodného roku, 28 je magic pro únor
const uint8_t date_month_lenghts[] = {
  0,
  31,
  28,
  31,
  30,
  31,
  30,
  31,
  31,
  30,
  31,
  30,
  31
};

const uint8_t * date_days_strs[] = {
  (uint8_t *)"0",
  (uint8_t *)"1",
  (uint8_t *)"2",
  (uint8_t *)"3",
  (uint8_t *)"4",
  (uint8_t *)"5",
  (uint8_t *)"6",
  (uint8_t *)"7",
  (uint8_t *)"8",
  (uint8_t *)"9",
  (uint8_t *)"10",
  (uint8_t *)"11",
  (uint8_t *)"12",
  (uint8_t *)"13",
  (uint8_t *)"14",
  (uint8_t *)"15",
  (uint8_t *)"16",
  (uint8_t *)"17",
  (uint8_t *)"18",
  (uint8_t *)"19",
  (uint8_t *)"20",
  (uint8_t *)"21",
  (uint8_t *)"22",
  (uint8_t *)"23",
  (uint8_t *)"24",
  (uint8_t *)"25",
  (uint8_t *)"26",
  (uint8_t *)"27",
  (uint8_t *)"28",
  (uint8_t *)"29",
  (uint8_t *)"30",
  (uint8_t *)"31"
};


// works only for years from 1904 to 25xx
uint8_t date_get_leap(uint16_t yr) {
  if (((yr % 4 == 0) && (yr % 100 != 0)) || (yr % 400 == 0)) {
    return 1;
  } else {
    return 0;
  }
}


uint8_t date_get_starting_day(uint16_t year, uint8_t month) {
  //1.1.1904 = pátek/friday
  uint16_t prac_yr = 0, i = 0;
  uint32_t day_cnt = 0;

  if (year >= 1904) {
    year -= 1904;
  } else {
    return 0;
  }

  while (year > prac_yr) {
    day_cnt += 365;
    if(date_get_leap(prac_yr + 1904)) {
      day_cnt++;
    }
    prac_yr++;
  }

  for (i = 1; i < month; i++) {
    if (i != 2) {
      day_cnt += date_month_lenghts[i];
    } else {
      if (date_get_leap(year)) {
        day_cnt += 29;
      } else {
        day_cnt += 28;
      }
    }
  }

  if ((day_cnt % 7 + 1) <= 3) {
    return (day_cnt % 7 + 1) + 4;
  } else {
    return (day_cnt % 7 + 1) - 3;
  }

}


void date_select_widget_load_days(
    dateSelectorWidgetType *d,
    uint16_t year,
    uint8_t month,
    uint8_t day
  ) {
  uint16_t x, y;

  if (month != 2) {
    d->dayCount = date_month_lenghts[month];
  } else {
    if (date_get_leap(year)) {
      d->dayCount = 29;
    } else {
      d->dayCount = 28;
    }
  }

  d->startDay = date_get_starting_day(year, month);

  if (day > d->dayCount) {
    day = d->dayCount;
  }

  y = 1;
  for(x = d->startDay - 1; x < (d->dayCount + d->startDay - 1); x++) {
      d->buttons[y] = pscg_add_button(
          0 + x % 7,
          1 + x / 7,
          1 + x % 7,
          2 + x / 7,
          (uint8_t *)date_days_strs[y],
          d->dateSelectorId,
          sda_current_con
      );

      if (d->useHighlight) {
        pscg_set_ghost(d->buttons[y], 1, sda_current_con);
      }

      if (y == day) {
        pscg_set_select(d->buttons[y], 1, sda_current_con);
      }
      y++; // counts day in month
  }
  d->selectedDay = day;
  d->currentMonth = month;
  d->currentYear = year;
}


uint16_t date_select_widget_init(
    dateSelectorWidgetType *d,
    uint16_t year,
    uint8_t month,
    uint8_t day
  ) {

  uint16_t scr;

  scr = pscg_add_screen(sda_current_con);
  pscg_add_text(0, 0, 1, 1, OVRL_DAY1, scr, sda_current_con);
  pscg_add_text(1, 0, 2, 1, OVRL_DAY2, scr, sda_current_con);
  pscg_add_text(2, 0, 3, 1, OVRL_DAY3, scr, sda_current_con);
  pscg_add_text(3, 0, 4, 1, OVRL_DAY4, scr, sda_current_con);
  pscg_add_text(4, 0, 5, 1, OVRL_DAY5, scr, sda_current_con);
  pscg_add_text(5, 0, 6, 1, OVRL_DAY6, scr, sda_current_con);
  pscg_add_text(6, 0, 7, 1, OVRL_DAY7, scr, sda_current_con);
  d->dateSelectorId = scr;
  d->selectedDay = day;
  d->currentMonth = month;
  d->currentYear = year;
  date_select_widget_load_days(d, year, month, day);

  return scr;
}

void date_select_widget_set_date(
    dateSelectorWidgetType *d,
    uint16_t year,
    uint8_t month,
    uint8_t day) {

  uint16_t y;

  if (year == d->currentYear && month == d->currentMonth) {
    pscg_set_select(d->buttons[d->selectedDay], 0, sda_current_con);
    pscg_set_select(d->buttons[day], 1, sda_current_con);
    d->selectedDay = day;
  } else {
    //remove days
    for (y = 1; y <= d->dayCount; y++) {
      pscg_destroy(d->buttons[y], sda_current_con);
    }
    //load new ones
    date_select_widget_load_days(d, year, month, day);
  }
}

uint16_t date_select_widget_update(dateSelectorWidgetType *d) {
  uint16_t y;
  uint16_t prac;
  uint16_t retval = 0;

  for(y = 1; y <= d->dayCount; y++) {
    if (pscg_get_event(d->buttons[y], sda_current_con) == EV_RELEASED) {
      retval = 2;
      if (y != d->selectedDay) {
        pscg_set_select(d->buttons[d->selectedDay], 0, sda_current_con);
        pscg_set_select(d->buttons[y], 1, sda_current_con);
        d->selectedDay = y;
        retval = 1;
      }

    }
    pscg_set_event(d->buttons[y], EV_NONE, sda_current_con);
  }

  return retval;
}


uint16_t date_select_highlight(dateSelectorWidgetType *d, uint8_t day) {
  uint16_t prac;

  // input validation
  if ((day == 0) || (day > 31)) {
    return 0;
  }

  if (d->useHighlight) {
    pscg_set_ghost(d->buttons[day], 0, sda_current_con);
  } else {
    pscg_set_select(d->buttons[day], 1, sda_current_con);
  }
  return 0;
}


uint8_t date_select_widget_get_day(dateSelectorWidgetType *d) {
  return d->selectedDay;
}

uint16_t date_select_widget_get_day_id(dateSelectorWidgetType *d, uint8_t day) {
  if ((day == 0) || (day > 31)) {
    return 0;
  }

  return d->buttons[day];
}

void date_select_set_highlight(dateSelectorWidgetType *d, uint8_t val) {
  d->useHighlight = val;
}