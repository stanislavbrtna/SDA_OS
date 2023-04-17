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

#ifndef SDA_SYSTEM_OVERLAYS_H
#define SDA_SYSTEM_OVERLAYS_H

#include "../../SDA_OS.h"

// time selector overlay
uint16_t time_overlay_init(); //ret: overlay id
void time_overlay_set_time(uint16_t ovId, uint16_t hour, uint16_t min);
void time_overlay_update(uint16_t ovId); //updates given overlay
uint16_t time_overlay_get_ok(uint16_t ovId); //ret 1 if "ok" was pressed
void time_overlay_clear_ok(uint16_t ovId); //clears ok flag

uint16_t time_overlay_get_hours(uint16_t ovId); //ret: hours
uint16_t time_overlay_get_minutes(uint16_t ovId); //ret: seconds
void time_overlay_destructor();

// color overlay
uint16_t color_overlay_init();
void color_overlay_update(uint16_t ovId);
uint16_t color_overlay_get_color(uint16_t ovId);
void color_overlay_set_color(uint16_t ovId, uint16_t col);
uint16_t color_overlay_get_ok(uint16_t ovId);
void color_overlay_clear_ok(uint16_t ovId);
void color_overlay_destructor();

// date overlay
uint16_t date_overlay_init(uint16_t yr, uint8_t mn, uint8_t dy);
void date_overlay_update(uint16_t ovId);
uint16_t date_overlay_get_ok(uint16_t ovId);
uint16_t date_overlay_get_year(uint16_t ovId);
uint8_t date_overlay_get_month(uint16_t ovId);
uint8_t date_overlay_get_day(uint16_t ovId);
uint16_t date_overlay_get_ok(uint16_t ovId);
void date_overlay_clear_ok(uint16_t ovId);
void date_overlay_destructor();

// error overlay
void sda_error_overlay_destructor();
void svp_errSoftPrint(svsVM *s);
void sda_show_error_message(uint8_t * text);
void sda_error_overlay_handle();

// battery overlay
void sda_batt_overlay_init();
void batt_overlay_handle(uint8_t init);
uint8_t sda_batt_overlay_shown();
void sda_batt_overlay_destructor();

// password overlay
uint16_t password_overlay_init();
void password_overlay_update(uint16_t ovId);
uint16_t password_overlay_get_ok(uint16_t ovId);
void password_overlay_clear_ok(uint16_t ovId);

// auth overlay
void sda_show_auth_overlay_init();
void sda_auth_overlay_handle();

#include "sda_clipboard_overlay.h"

#endif
