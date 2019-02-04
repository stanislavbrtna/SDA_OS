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

// get color overlay

static uint16_t col_screen;
static uint16_t col_id = 0xFFFF;

static uint16_t col_ok;
static uint16_t col_cancel;

static uint16_t col_show;

static uint16_t col_done;

static uint16_t col_rsl;
static uint16_t col_gsl;
static uint16_t col_bsl;

uint16_t color_overlay_init() {
  col_done = 0;

  hideKeyboard();

  col_screen = pscg_add_screen(sda_current_con);

  pscg_add_text(1, 0, 8, 1, OVRL_SELECT_COLOR , col_screen, sda_current_con);
  col_show = pscg_add_cbutton(1, 1, 7, 2, (uint8_t *)"", col_screen, sda_current_con);

  pscg_add_text(1, 3, 2, 4, (uint8_t *)"R:" , col_screen, sda_current_con);
  col_rsl = pscg_add_slider_h(2, 3, 7, 4, 255, 255, col_screen, sda_current_con);
  pscg_add_text(1, 5, 2, 6, (uint8_t *)"G:", col_screen, sda_current_con);
  col_gsl = pscg_add_slider_h(2, 5, 7, 6, 255, 255, col_screen, sda_current_con);
  pscg_add_text(1, 7, 2, 8, (uint8_t *)"B:" , col_screen, sda_current_con);
  col_bsl = pscg_add_slider_h(2, 7, 7, 8, 255, 255, col_screen, sda_current_con);

  col_ok = pscg_add_button(4, 9, 6, 10, OVRL_OK, col_screen, sda_current_con);
  col_cancel = pscg_add_button(1, 9, 3, 10, OVRL_CANCEL, col_screen, sda_current_con);

  col_id = setOverlayScreen(col_screen, sda_current_con);
	setOverlayDestructor(color_overlay_destructor);
  //setOverlayY2(288);

  return col_id;
}

void color_overlay_destructor() {
	col_done = 2;
  pscg_clear_screen_ev(col_screen, sda_current_con);
	pscg_destroy_screen(col_screen, sda_current_con);
	setRedrawFlag();
	overlayDestructorDone();
}

void color_overlay_update(uint16_t ovId) {
  if (col_id != ovId) {
    return;
  }

  if ((col_done == 1) || (col_done == 2)) {
    return;
  }

  if (pscg_get_event(col_ok, sda_current_con) == EV_RELEASED) {
	  destroyOverlay();
	  col_done = 1;
		return;
	}

	if (pscg_get_event(col_cancel, sda_current_con) == EV_RELEASED) {
	  destroyOverlay();
	  col_done = 2;
		return;
	}

	if ((pscg_get_event(col_rsl, sda_current_con))||(pscg_get_event(col_gsl, sda_current_con))||(pscg_get_event(col_bsl, sda_current_con))){
	  pscg_set_value(
	  							col_show,
	  							LCD_MixColor(
	  													(uint8_t)pscg_get_value(col_rsl, sda_current_con),
	  													(uint8_t)pscg_get_value(col_gsl, sda_current_con),
	  													(uint8_t)pscg_get_value(col_bsl, sda_current_con)
	  							)
								, sda_current_con
	  );
	}

  pscg_clear_screen_ev(col_screen, sda_current_con);
}

uint16_t color_overlay_get_ok(uint16_t ovId) {
  if (col_id != ovId) {
    return 0;
  }
  if (col_done == 1) {
    return 1;
  } else {
    return 0;
  }
}

uint16_t color_overlay_get_color(uint16_t ovId) {
  if (col_id != ovId) {
    return 0;
  }

  return LCD_MixColor(
  									(uint8_t)pscg_get_value(col_rsl, sda_current_con),
  									(uint8_t)pscg_get_value(col_gsl, sda_current_con),
  									(uint8_t)pscg_get_value(col_bsl, sda_current_con)
  				);
}

void color_overlay_set_color(uint16_t ovId, uint16_t col) {
  uint8_t r, g, b;
  if (col_id != ovId) {
    return;
  }

  r = (uint8_t)(((float)((col >> 11) & 0x1F) / 32) * 256);
	g = (uint8_t)(((float)(((col & 0x07E0) >> 5) & 0x3F) / 64) * 256);
	b = (uint8_t)(((float)(col & 0x1F) / 32) * 256);

  pscg_set_value(col_show, col, sda_current_con);
  pscg_set_value(col_rsl, r, sda_current_con);
  pscg_set_value(col_gsl, g, sda_current_con);
  pscg_set_value(col_bsl, b, sda_current_con);

}

void color_overlay_clear_ok(uint16_t ovId) {
  if (col_id != ovId) {
    return;
  }
  col_done = 0;
  col_id = 0xFFFF;
}
