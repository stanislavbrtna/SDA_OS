/*
Copyright (c) 2024 Stanislav Brtna

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

#include "sda_sic.h"

static uint8_t last_width;

void sda_draw_sic(int16_t x1, int16_t y1, uint16_t front_color, uint16_t back_color, uint8_t * icon) {
  // using canvas
  LCD_canvas_set(x1, y1 + 1, x1 + icon[1], y1 + icon[2]);
  LCD_canvas_zero();

  uint8_t  bit_n    = 0;
  uint16_t icon_pos = 3;

  for(uint32_t i = 0; i < icon[1]*icon[2]; i++) {
    if ((1 << bit_n) & icon[icon_pos]) {
      LCD_canvas_putcol(front_color);
    } else {
      LCD_canvas_putcol(back_color);
    }
    bit_n++;
    if (bit_n == 8) {
      bit_n = 0;
      icon_pos++;
    }
  }
}


uint8_t sda_draw_sic_file(int16_t x1, int16_t y1, uint16_t front_color, uint16_t back_color, uint8_t * fname) {
  svp_file fp;
  uint8_t image_width;
  uint8_t image_height;
  LCD_drawArea area;

  if(fname[0] == 2) { // binary sic detection
    sda_draw_sic(x1, y1, front_color, back_color, fname);
    last_width = fname[1];
    return 0;
  }

  if (!svp_fopen_read(&fp, fname)) {
    printf("%s: Error while opening file %s!\n",__FUNCTION__, fname);
    return 1;
  }

  if (svp_fread_u8(&fp) != 2) {
    printf("%s: Error: Only sic v.2 are supported! (%s)\n",__FUNCTION__, fname);
    return 1;
  }

  image_width  = svp_fread_u8(&fp);
  image_height = svp_fread_u8(&fp);

  LCD_getDrawArea(&area);
  LCD_setSubDrawArea(x1, y1, x1 + image_width, y1 + image_width);
  LCD_canvas_set(x1, y1, x1 + image_width, y1 + image_height);

  uint8_t bit_n   = 0;
  uint8_t current = svp_fread_u8(&fp);

  for(uint32_t i = 0; i < image_width * image_height; i++) {
    if ((1 << bit_n) & current) {
      LCD_canvas_putcol(front_color);
    } else {
      LCD_canvas_putcol(back_color);
    }
    bit_n++;
    if (bit_n == 8) {
      bit_n = 0;
      current = svp_fread_u8(&fp);
    }

    if(LCD_canvas_get_y_overflow()) {
      break;
    }
  }

  LCD_setDrawAreaS(&area);
  svp_fclose(&fp);

  last_width = image_width;
  return 0;
}


uint8_t sda_sic_get_last_width() {
  return last_width;
}


uint8_t sda_sic_get_width(uint8_t * fname) {
  svp_file fp;
  uint8_t image_width;

  if(fname[0] == 2) { // binary sic detection
    return fname[1];
  }

  if (!svp_fopen_read(&fp, fname)) {
    printf("%s: Error while opening file %s!\n",__FUNCTION__, fname);
    return 0;
  }

  if (svp_fread_u8(&fp) != 2) {
    printf("%s: Error: Only sic v.2 are supported! (%s)\n",__FUNCTION__, fname);
    return 0;
  }

  image_width  = svp_fread_u8(&fp);

  svp_fclose(&fp);
  return image_width;
}


uint8_t sda_sic_get_height(uint8_t * fname) {
  svp_file fp;
  uint8_t image_height;

  if(fname[0] == 2) { // binary sic detection
    return fname[1];
  }

  if (!svp_fopen_read(&fp, fname)) {
    printf("%s: Error while opening file %s!\n",__FUNCTION__, fname);
    return 0;
  }

  if (svp_fread_u8(&fp) != 2) {
    printf("%s: Error: Only sic v.2 are supported! (%s)\n",__FUNCTION__, fname);
    return 0;
  }

  svp_fread_u8(&fp); // dummy read
  image_height = svp_fread_u8(&fp);

  svp_fclose(&fp);
  return image_height;
}