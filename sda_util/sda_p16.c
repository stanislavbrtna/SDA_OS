/*
Copyright (c) 2019 Stanislav Brtna

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

#include "../SDA_OS.h"
#include "sda_p16.h"

static uint8_t p16_R;
static uint8_t p16_G;
static uint8_t p16_B;
static uint8_t p16_use_pmc;

static uint8_t p16_use_alpha;
static uint16_t p16_alpha_color;
static uint16_t p16_bg_color;

uint8_t sda_draw_p16_scaled_up(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *filename);


void sda_p16_set_pmc(uint8_t enable, uint16_t color) {
  p16_use_pmc = enable;
  p16_R = (uint8_t)((color>>11)&0x1F);
  p16_G = (uint8_t)(((color&0x07E0)>>5)&0x3F);
  p16_B = (uint8_t)(color&0x1F);
}


void sda_p16_set_alpha(uint8_t enable, uint16_t color, uint16_t bg_color) {
  p16_use_alpha = enable;
  p16_alpha_color = color;
  p16_bg_color = bg_color;
}


uint8_t p16_get_header(svp_file * fp, p16Header * header) {
  svp_fseek(fp, 0);

  header->version = 0;
  header->imageWidth = 0;
  header->imageHeight = 0;
  header->storageMode = 0;
  header->dataOffset = 0;

  if (svp_fread_u8(fp) != 0x1b) {
    return 1;
  }

  if (svp_fread_u8(fp) != 'P') {
    return 1;
  }

  if (svp_fread_u8(fp) != 'S') {
    return 1;
  }

  if (svp_fread_u8(fp) != 'M') {
    return 1;
  }

  if (svp_fread_u8(fp) != '1') {
    return 1;
  }

  if (svp_fread_u8(fp) != '6') {
    return 1;
  }

  header->version = svp_fread_u8(fp);

  if(header->version == 1) {
    header->imageWidth |= svp_fread_u8(fp);
    header->imageWidth |= svp_fread_u8(fp) << 8;

    header->imageHeight |= svp_fread_u8(fp);
    header->imageHeight |= svp_fread_u8(fp) << 8;

    header->storageMode = svp_fread_u8(fp);
    header->dataOffset = svp_fread_u8(fp);

    // convinient seek to data start
    if (header->dataOffset != 1) {
      svp_fseek(fp, svp_ftell(fp) + header->dataOffset - 1);
    }
  } else {
    printf("Can parse p16 version 1 only\n");
    return 1;
  }
  return 0;
}


uint16_t p16_get_pixel(svp_file * fp, p16Header * header, p16State * state) {
  uint16_t color;

  if (header->storageMode == 0) {
    svp_fread(fp, &color, sizeof(color));
    return color;
  }
  if (header->storageMode == 1) {
    if (state->init == 0) {
      svp_fread(fp, &color, sizeof(color));
      state->fpos = svp_ftell(fp);
      state->init = 1;
      state->prevVal = 0;
      state->repeat = 0;

      state->prevVal = color;
      return color;
    } else {
      if (state->repeat == 0) {
        svp_fread(fp, &color, sizeof(color));
        if (state->prevVal == color) {
          svp_fread(fp, &state->repeat, sizeof(state->repeat));
          state->repeat--;
          state->prevVal = color;
          return color;
        } else {
          state->prevVal = color;
          return color;
        }
      } else {
        state->repeat--;
        return state->prevVal;
      }
    }
  }

  return 0;
}


uint8_t sda_draw_p16(uint16_t x, uint16_t y, uint8_t *filename) {
  svp_file fp;
  p16Header header;
  p16State imageState;
  LCD_drawArea area;

  if (!svp_fopen_read(&fp, filename)) {
    printf("sda_draw_p16: Error while opening file %s!\n", filename);
    return 1;
  }
  touch_lock = SDA_LOCK_LOCKED;

  p16_get_header(&fp, &header);

  LCD_getDrawArea(&area);
  LCD_setSubDrawArea(x, y, x + header.imageWidth, y + header.imageHeight);
  LCD_canvas_set(x, y, x + header.imageWidth - 1, y + header.imageHeight - 1);

  imageState.init = 0;

  uint16_t color;
  for(uint32_t n = 0; n < header.imageWidth*header.imageHeight ; n++) {
    color = p16_get_pixel(&fp, &header, &imageState);

    if (p16_use_alpha) {
      if (color == p16_alpha_color) {
        color = p16_bg_color;
      }
    }

    if (p16_use_pmc) {
      uint8_t r, g, b;

      r = (uint8_t)((color>>11)&0x1F);
      g = (uint8_t)(((color&0x07E0)>>5)&0x3F);
      b = (uint8_t)(color&0x1F);

      color = (((r+p16_R)/2) & 0x1F)<<11 | (((g+p16_G)/2)<<5  & 0x7E0 ) | (((b+p16_B)/2)&0x1F);

    }

    LCD_canvas_putcol(color);
  }

  LCD_setDrawAreaS(&area);
  svp_fclose(&fp);
  touch_lock = SDA_LOCK_UNLOCKED;
  return 0;
}


uint8_t sda_draw_p16_scaled_up(uint16_t x, uint16_t y, uint16_t width_n, uint16_t height_n, uint8_t *filename) {
  svp_file fp;
  p16Header header;
  p16State imageState;
  uint32_t fpos, init;
  uint16_t prevVal;
  uint16_t repeat;
  LCD_drawArea area;

  if (!svp_fopen_read(&fp, filename)) {
    printf("sda_draw_p16: Error while opening file %s!\n", filename);
    return 1;
  }
  touch_lock = SDA_LOCK_LOCKED;

  p16_get_header(&fp, &header);
  LCD_getDrawArea(&area);
  LCD_setSubDrawArea(x, y, x + header.imageWidth * width_n, y + header.imageHeight * height_n);
  LCD_canvas_set(x, y, x + header.imageWidth * width_n - 1, y + header.imageHeight * height_n - 1);

  imageState.init = 0;
  imageState.repeat = 0;
  imageState.prevVal = 0;

  uint16_t color;
  uint16_t pix = 0;

  for(uint32_t n = 0; n < header.imageHeight; n++) {
    fpos = svp_ftell(&fp);
    prevVal = imageState.prevVal;
    repeat = imageState.repeat;
    init = imageState.init;

    for (int a = 0; a < height_n; a++) {
      svp_fseek(&fp, fpos);
      imageState.prevVal = prevVal;
      imageState.repeat = repeat;
      imageState.init = init;

      for(uint32_t b = 0; b < header.imageWidth; b++) {
        color = p16_get_pixel(&fp, &header, &imageState);

        if (p16_use_alpha) {
          if (color == p16_alpha_color) {
            color = p16_bg_color;
          }
        }

        if (p16_use_pmc) {
          uint8_t r, g, b;
          r = (uint8_t)((color>>11)&0x1F);
          g = (uint8_t)(((color&0x07E0)>>5)&0x3F);
          b = (uint8_t)(color&0x1F);

          color = (((r+p16_R)/2) & 0x1F)<<11 | (((g+p16_G)/2)<<5  & 0x7E0 ) | (((b+p16_B)/2)&0x1F);
        }

        for (int c = 0; c < width_n; c++) {
          LCD_canvas_putcol(color);
          pix++;
        }
      }
    }
  }

  svp_fclose(&fp);
  LCD_setDrawAreaS(&area);
  touch_lock = SDA_LOCK_UNLOCKED;
  return 0;
}


uint16_t sda_p16_get_width(uint8_t *filename) {
  svp_file fp;
  p16Header header;

  if (!svp_fopen_read(&fp, filename)) {
    printf("sda_draw_p16: Error while opening file %s!\n", filename);
    return 1;
  }

  p16_get_header(&fp, &header);
  svp_fclose(&fp);
  return header.imageWidth;
}

