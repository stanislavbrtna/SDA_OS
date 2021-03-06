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

#include "sda_system_overlays.h"

static uint8_t date_year_name[6];

static uint16_t date_done;
static uint16_t date_screen;
static uint16_t date_year_prev;
static uint16_t date_year_next;
static uint16_t date_year_text;
static uint16_t date_month_prev;
static uint16_t date_month_next;
static uint16_t date_month_text;

//input/output date values
static uint16_t date_year_val;
static uint8_t date_month_val;
static uint8_t date_day_val;

static uint16_t date_selector_screen;
static dateSelectorWidgetType date_widget;

static uint16_t date_ok;
static uint16_t date_cancel;
static uint16_t date_id = 0xFFFF;


void set_year_string(uint8_t * str, uint16_t year) {
  uint16_t a, b;

  for(a = 0; a < 5; a++) {
    str[a] = ' ';
  }

  str[5] = '\0';

  a = 0;

  if (year < 9999) {
    while (0 != year) {
      str[4 - a] = (year % 10 + 48);
      year = year / 10;
      a++;
    }

    str[5] = 0;
  } else {
    str[0] = 'E';
    str[1] = 'R';
    str[2] = 'R';
    str[3] = 0;
  }

  a = 0;
  while(str[a] == ' ') {
    b = 0;
    while(str[b] != 0) {
      str[b] = str[b + 1];
      b++;
    }
  }

}


void svp_write_date_string(
    uint8_t *str,
    uint8_t month_as_num,
    uint8_t write_year) {
  uint8_t year_str[6];

  str[0] = 0;

  svp_str_add(str, (uint8_t *)date_days_strs[svpSGlobal.day]);

  svp_str_add(str, (uint8_t *)". ");

  if (month_as_num) {
    svp_str_add(str, (uint8_t *)date_days_strs[svpSGlobal.month]);
    svp_str_add(str, (uint8_t *)". ");
  } else {
    svp_str_add(str, (uint8_t *)date_month_names[svpSGlobal.month]);
    svp_str_add(str, (uint8_t *)" ");
  }

  if(write_year) {
    set_year_string(year_str, svpSGlobal.year);
    svp_str_add(str, year_str);
  }
}


uint16_t date_overlay_init(uint16_t yr, uint8_t mn, uint8_t dy) {
  date_done = 0;
  date_year_val = yr;
  date_month_val = mn;
  date_day_val = dy;

  hideKeyboard();

  date_screen = pscg_add_screen(sda_current_con);
  pscg_set_cell_space_bottom(date_screen, 4, sda_current_con);
  pscg_set_y_cell(date_screen, 34, sda_current_con);

  pscg_set_x_cell(date_screen, 16, sda_current_con);

  pscg_add_text(1, 0, 16, 1, OVRL_ENTER_DATE, date_screen, sda_current_con);

  set_year_string(date_year_name, date_year_val);

  date_year_text = pscg_add_text(5, 1, 12, 2, date_year_name, date_screen, sda_current_con);
  date_year_prev = pscg_add_button(1, 1, 3, 2, (uint8_t *)"<", date_screen, sda_current_con);
  date_year_next = pscg_add_button(13, 1, 15, 2, (uint8_t *)">", date_screen, sda_current_con);

  date_month_text
    = pscg_add_text(5, 2, 12, 3, (uint8_t *)date_month_names[date_month_val], date_screen, sda_current_con);
  date_month_prev = pscg_add_button(1, 2, 3, 3, (uint8_t *)"<", date_screen, sda_current_con);
  date_month_next = pscg_add_button(13, 2, 15, 3, (uint8_t *)">", date_screen, sda_current_con);

  date_selector_screen
    = date_select_widget_init(&date_widget, date_year_val, date_month_val, date_day_val);

  pscg_set_x1y1x2y2(date_selector_screen, 1, 3, 15, 10, sda_current_con);

  pscg_set_screen(date_selector_screen, date_screen, sda_current_con);

  date_ok = pscg_add_button(10, 10, 14, 11, OVRL_OK, date_screen, sda_current_con);
  date_cancel = pscg_add_button(2, 10, 6, 11, OVRL_CANCEL, date_screen, sda_current_con);

  date_id = setOverlayScreen(date_screen, sda_current_con);

  setOverlayDestructor(date_overlay_destructor);

  setOverlayY2(442);

  return date_id;
}


void date_overlay_destructor() {
  date_done = 2;
  pscg_clear_screen_ev(date_screen, sda_current_con);
  pscg_destroy_screen(date_screen, sda_current_con);
  setRedrawFlag();
  overlayDestructorDone();
}


void date_overlay_update(uint16_t ovId) {

  if (date_id != ovId) {
    return;
  }

  if((date_done == 1) || (date_done == 2)) {
    return;
  }

  date_select_widget_update(&date_widget);

  if (pscg_get_event(date_ok, sda_current_con) == EV_RELEASED) {
    destroyOverlay();
    date_done = 1;
    return;
  }
  pscg_set_event(date_ok, EV_NONE, sda_current_con);

  if (pscg_get_event(date_cancel, sda_current_con) == EV_RELEASED) {
    destroyOverlay();
    date_done = 2;
    return;
  }
  pscg_set_event(date_cancel, EV_NONE, sda_current_con);

  if (pscg_get_event(date_year_prev, sda_current_con) == EV_RELEASED) {
    if (date_year_val > 1904) {
      date_year_val--;
      set_year_string(date_year_name, date_year_val);
      pscg_set_modified(date_year_text, sda_current_con);
      date_select_widget_set_date(
        &date_widget,
        date_year_val,
        date_month_val,
        date_select_widget_get_day(&date_widget)
      );
    }
  }
  pscg_set_event(date_year_prev, EV_NONE, sda_current_con);

  if (pscg_get_event(date_year_next, sda_current_con) == EV_RELEASED) {
    if (date_year_val < 2500) {
      date_year_val++;
      set_year_string(date_year_name, date_year_val);
      pscg_set_modified(date_year_text, sda_current_con);
      date_select_widget_set_date(
        &date_widget,
        date_year_val,
        date_month_val,
        date_select_widget_get_day(&date_widget)
      );
    }
  }
  pscg_set_event(date_year_next, EV_NONE, sda_current_con);

  if (pscg_get_event(date_month_prev, sda_current_con) == EV_RELEASED) {
    if (date_month_val > 1) {
      date_month_val--;
      pscg_set_str(date_month_text, (uint8_t *)date_month_names[date_month_val], sda_current_con);
      date_select_widget_set_date(
        &date_widget,
        date_year_val,
        date_month_val,
        date_select_widget_get_day(&date_widget)
      );

    } else {
      if (date_year_val > 1904) {
        date_month_val = 12;
        pscg_set_str(date_month_text, (uint8_t *)date_month_names[date_month_val], sda_current_con);
        date_year_val--;
        set_year_string(date_year_name, date_year_val);
        pscg_set_modified(date_year_text, sda_current_con);
        date_select_widget_set_date(
          &date_widget,
          date_year_val,
          date_month_val,
          date_select_widget_get_day(&date_widget)
        );
      }
    }
  }
  pscg_set_event(date_month_prev, EV_NONE, sda_current_con);

  if (pscg_get_event(date_month_next, sda_current_con) == EV_RELEASED) {
    if (date_month_val < 12) {
      date_month_val++;
      pscg_set_str(date_month_text, (uint8_t *)date_month_names[date_month_val], sda_current_con);
      date_select_widget_set_date(
        &date_widget,
        date_year_val,
        date_month_val,
        date_select_widget_get_day(&date_widget)
      );
    } else {
      if (date_year_val < 2500) {
        date_month_val = 1;
        pscg_set_str(date_month_text, (uint8_t *)date_month_names[date_month_val], sda_current_con);
        date_year_val++;
        set_year_string(date_year_name, date_year_val);
        pscg_set_modified(date_year_text, sda_current_con);
        date_select_widget_set_date(
          &date_widget,
          date_year_val,
          date_month_val,
          date_select_widget_get_day(&date_widget)
        );
      }
    }
  }
  pscg_set_event(date_month_next, EV_NONE, sda_current_con);
}


uint16_t date_overlay_get_ok(uint16_t ovId) {
  if (date_id != ovId) {
    return 0;
  }
  if (date_done == 1) {
    return 1;
  } else {
    return 0;
  }
}


uint16_t date_overlay_get_year(uint16_t ovId) {
  if (date_id != ovId) {
    return 0;
  }

  return date_year_val;
}


uint8_t date_overlay_get_month(uint16_t ovId) {
  if (date_id != ovId) {
    return 0;
  }

  return date_month_val;
}


uint8_t date_overlay_get_day(uint16_t ovId) {
  if (date_id != ovId) {
    return 0;
  }

  return date_select_widget_get_day(&date_widget);
}


void date_overlay_clear_ok(uint16_t ovId) {
  if (date_id != ovId) {
    return;
  }
  date_done = 0;
  date_id=  0xFFFF;
}
