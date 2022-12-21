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
#include <float.h>

static uint16_t col_screen;
static uint16_t col_id = 0xFFFF;

static uint16_t col_ok;
static uint16_t col_cancel;

static uint16_t col_show;

static uint16_t col_done;

static uint16_t col_r;
static uint16_t col_g;
static uint16_t col_b;

static float col_hsl[3];

static uint8_t redraw_sliders;

uint16_t color_overlay_init() {
  col_done = 0;

  sda_keyboard_hide();

  col_screen = gr2_add_screen(sda_current_con);

  gr2_add_text(1, 0, 8, 1, OVRL_SELECT_COLOR , col_screen, sda_current_con);
  col_show = gr2_add_cbutton(1, 1, 7, 2, (uint8_t *)"", col_screen, sda_current_con);

  redraw_sliders = 1;

  col_ok = gr2_add_button(5, 9, 7, 10, OVRL_OK, col_screen, sda_current_con);
  col_cancel = gr2_add_button(1, 9, 4, 10, OVRL_CANCEL, col_screen, sda_current_con);

  gr2_text_set_align(col_ok, GR2_ALIGN_CENTER, sda_current_con);
  gr2_text_set_align(col_cancel, GR2_ALIGN_CENTER, sda_current_con);

  col_id = setOverlayScreen(col_screen, sda_current_con);
  setOverlayDestructor(color_overlay_destructor);

  return col_id;
}


void color_overlay_destructor() {
  col_done = 2;
  gr2_destroy(col_screen, sda_current_con);
  setRedrawFlag();
  overlayDestructorDone();
}

uint8_t in_range(float a, float b, float diff) {
  if (a - diff < b && a + diff > b) {
    return 1;
  }
  return 0;
}

#define SLIDER_W 192
#define SLIDER_WF 192.0

void redraw_slider(int16_t x1, int16_t y1, uint8_t mode) {
  LCD_setDrawArea(x1, y1, x1 + SLIDER_W, y1 + 32);
  float hsl[3];
  float rgb[3];
  hsl[1] = 0.50;
  hsl[2] = 0.50;

  if (mode == 0) {
    for(int i=0; i < SLIDER_W; i++) {
      hsl[0] = (((float) i) / SLIDER_WF) * 6.0;
      fromHSLtoRGB(hsl, rgb);
      if (in_range(col_hsl[0], hsl[0], 0.05))
        LCD_DrawLine(x1 + i, y1, x1 + i, y1 + 32, ~LCD_MixColor((uint16_t) (256.0 * rgb[0]), (uint16_t) (256.0 * rgb[1]), (uint16_t) (256.0 * rgb[2])));
      else
        LCD_DrawLine(x1 + i, y1, x1 + i, y1 + 32, LCD_MixColor((uint16_t) (256.0 * rgb[0]), (uint16_t) (256.0 * rgb[1]), (uint16_t) (256.0 * rgb[2])));
    }
  }

  if (mode == 1) {
    for(int i=0; i < SLIDER_W; i++) {
      hsl[0] = col_hsl[0];
      hsl[1] = (((float) i) / SLIDER_WF);
      fromHSLtoRGB(hsl, rgb);
      if (in_range(col_hsl[1], hsl[1], 0.01))
        LCD_DrawLine(x1 + i, y1, x1 + i, y1 + 32, ~LCD_MixColor((uint16_t) (256.0 * rgb[0]), (uint16_t) (256.0 * rgb[1]), (uint16_t) (256.0 * rgb[2])));
      else
        LCD_DrawLine(x1 + i, y1, x1 + i, y1 + 32, LCD_MixColor((uint16_t) (256.0 * rgb[0]), (uint16_t) (256.0 * rgb[1]), (uint16_t) (256.0 * rgb[2])));
    }
  }

  if (mode == 2) {
    for(int i=0; i < SLIDER_W; i++) {
      hsl[0] = col_hsl[0];
      hsl[1] = col_hsl[1];
      hsl[2] = (((float) i) / SLIDER_WF);
      fromHSLtoRGB(hsl, rgb);
      if (in_range(col_hsl[2], hsl[2], 0.01))
        LCD_DrawLine(x1 + i, y1, x1 + i, y1 + 32, ~LCD_MixColor((uint16_t) (256.0 * rgb[0]), (uint16_t) (256.0 * rgb[1]), (uint16_t) (256.0 * rgb[2])));
      else
        LCD_DrawLine(x1 + i, y1, x1 + i, y1 + 32, LCD_MixColor((uint16_t) (256.0 * rgb[0]), (uint16_t) (256.0 * rgb[1]), (uint16_t) (256.0 * rgb[2])));
    }
  }

  LCD_DrawRectangle(x1, y1, x1 + SLIDER_W, y1 + 32, sda_current_con->border_color);
  
}


uint8_t touch_slider(uint16_t x1, uint16_t y1, uint8_t mode) {
  float rgb[3];
  if (sx_get_touch_x() > x1 && sx_get_touch_x() < x1 + SLIDER_W && sx_get_touch_y() > y1 && sx_get_touch_y() < y1 + 32 && sx_get_touch_ev() == EV_HOLD) {
    if (mode == 0) {
      col_hsl[0] = (((float) (sx_get_touch_x() - x1)) / SLIDER_WF) * 6.0;
    }

    if (mode == 1) {
      col_hsl[1] = (((float) (sx_get_touch_x() - x1)) / SLIDER_WF) * 1.0;
    }

    if (mode == 2) {
      col_hsl[2] = (((float) (sx_get_touch_x() - x1)) / SLIDER_WF) * 1.0;
    }
    
    fromHSLtoRGB(col_hsl, rgb);
    col_r = (uint8_t) (256.0 * rgb[0]);
    col_g = (uint8_t) (256.0 * rgb[1]);
    col_b = (uint8_t) (256.0 * rgb[2]);

    redraw_sliders = 2;

    gr2_set_value(
      col_show,
      LCD_MixColor(
        (uint8_t)col_r,
        (uint8_t)col_g,
        (uint8_t)col_b),
      sda_current_con
    );

    return 1;
  } else {
    return 0;
  }
}

void color_overlay_update(uint16_t ovId) {
  if (col_id != ovId) {
    return;
  }

  if ((col_done == 1) || (col_done == 2)) {
    return;
  }

  if (redraw_sliders == 2) {  
    redraw_slider(64, 156, 0);
    redraw_slider(64, 156 + 64, 1);
    redraw_slider(64, 156 + 128, 2);
    redraw_sliders = 3;
  } else if (redraw_sliders < 2) {
    redraw_sliders++;
  }

  touch_slider(64, 156, 0);
  touch_slider(64, 156 + 64, 1);
  touch_slider(64, 156 + 128, 2);

  if (gr2_get_event(col_ok, sda_current_con) == EV_RELEASED) {
    destroyOverlay();
    col_done = 1;
    return;
  }

  if (gr2_get_event(col_cancel, sda_current_con) == EV_RELEASED) {
    destroyOverlay();
    col_done = 2;
    return;
  }

  gr2_clear_screen_ev(col_screen, sda_current_con);
  sda_screen_button_handler(col_screen, col_cancel, sda_current_con);
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

  return LCD_MixColor(col_r, col_g, col_b);
}


void color_overlay_set_color(uint16_t ovId, uint16_t col) {
  uint8_t r, g, b;
  if (col_id != ovId) {
    return;
  }

  r = (uint8_t)(((float)((col >> 11) & 0x1F) / 32) * 256);
  g = (uint8_t)(((float)(((col & 0x07E0) >> 5) & 0x3F) / 64) * 256);
  b = (uint8_t)(((float)(col & 0x1F) / 32) * 256);

  gr2_set_value(col_show, col, sda_current_con);
  float rgb[3];

  rgb[0] = (float)r / 255.0;
  rgb[1] = (float)g / 255.0;
  rgb[2] = (float)b / 255.0;

  fromRGBtoHSL(rgb, col_hsl);

  for(int i = 1; i < 3; i++) {
    if (col_hsl[i] == 1) {
      col_hsl[i] = 0.99;
    }
  }
  redraw_sliders = 0;
}


void color_overlay_clear_ok(uint16_t ovId) {
  if (col_id != ovId) {
    return;
  }
  col_done = 0;
  col_id = 0xFFFF;
}


float max(float a, float b) {
  if (a > b) {
    return a;
  } else {
    return b;
  }
}

float min(float a, float b) {
  if (a < b) {
    return a;
  } else {
    return b;
  }
}

//https://stackoverflow.com/a/49433742

void fromRGBtoHSL(float rgb[], float hsl[])
{
     const float maxRGB = max(rgb[0], max(rgb[1], rgb[2]));
     const float minRGB = min(rgb[0], min(rgb[1], rgb[2]));
     const float delta2 = maxRGB + minRGB;
     hsl[2] = delta2 * 0.5f;

     const float delta = maxRGB - minRGB;
     if (delta < FLT_MIN)
         hsl[0] = hsl[1] = 0.0f;
     else
     {
         hsl[1] = delta / (hsl[2] > 0.5f ? 2.0f - delta2 : delta2);
         if (rgb[0] >= maxRGB)
         {
             hsl[0] = (rgb[1] - rgb[2]) / delta;
             if (hsl[0] < 0.0f)
                 hsl[0] += 6.0f;
         }
         else if (rgb[1] >= maxRGB)
             hsl[0] = 2.0f + (rgb[2] - rgb[0]) / delta;
         else
             hsl[0] = 4.0f + (rgb[0] - rgb[1]) / delta;
     }
}

void fromHSLtoRGB(const float hsl[], float rgb[])
{
    if(hsl[1] < FLT_MIN)
        rgb[0] = rgb[1] = rgb[2] = hsl[2];
    else if(hsl[2] < FLT_MIN)
        rgb[0] = rgb[1] = rgb[2] = 0.0f;
    else
    {
        const float q = hsl[2] < 0.5f ? hsl[2] * (1.0f + hsl[1]) : hsl[2] + hsl[1] - hsl[2] * hsl[1];
        const float p = 2.0f * hsl[2] - q;
        float t[] = {hsl[0] + 2.0f, hsl[0], hsl[0] - 2.0f};

        for(int i=0; i<3; ++i)
        {
            if(t[i] < 0.0f)
                t[i] += 6.0f;
            else if(t[i] > 6.0f)
                t[i] -= 6.0f;

            if(t[i] < 1.0f)
                rgb[i] = p + (q - p) * t[i];
            else if(t[i] < 3.0f)
                rgb[i] = q;
            else if(t[i] < 4.0f)
                rgb[i] = p + (q - p) * (4.0f - t[i]);
            else
                rgb[i] = p;
          }
      }
}