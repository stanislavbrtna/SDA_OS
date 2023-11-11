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

uint16_t p16_buffer[P16_BUFFER_SIZE];
uint16_t p16_buffer_pos;
uint8_t p16_buffer_init;
uint32_t p16_read_size;

uint8_t p16_buffer_keep;
uint32_t p16_file_crc;
uint32_t p16_file_size;

uint8_t sda_draw_p16_scaled_up(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *filename);

static void break_draw_cleanup(svp_file *fp);

uint8_t sda_draw_p16_scaled(uint16_t x, uint16_t y, int16_t scale_w, int16_t scale_h, uint8_t *filename) {
  if (scale_w > 0 && scale_h > 0) {
    if (scale_w == 1 && scale_h == 1) {
      return sda_draw_p16(x, y, filename);
    }
    return sda_draw_p16_scaled_up(x, y, (uint16_t)scale_w, (uint16_t)scale_h, filename);
  } else {
    return sda_draw_p16_scaled_down(x, y, (uint16_t)(scale_w*-1), (uint16_t)(scale_h*-1), filename);
  }
}

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

  p16_buffer_reset();
  p16_file_size = svp_get_size(fp);
  return 0;
}


uint16_t p16_buffer_reset() {
  p16_buffer_pos = 0;
  if (p16_buffer_keep == 0) {
    p16_buffer_init = 0;
    for(int i = 0; i < P16_BUFFER_SIZE; i++) {
      p16_buffer[i] = 0;
    }
  }
}

uint16_t p16_read_buffered(svp_file * fp) {
  
  if (p16_buffer_init == 0) {
    if (svp_ftell(fp) + 2*P16_BUFFER_SIZE < p16_file_size) {
      p16_read_size = 2*P16_BUFFER_SIZE;
    } else {
      p16_read_size = p16_file_size - svp_ftell(fp);
    }
    svp_fread(fp, p16_buffer, p16_read_size);
    p16_buffer_pos = 0;
    p16_buffer_init = 1;
  }

  uint16_t r = p16_buffer[p16_buffer_pos];
  
  if (p16_buffer_pos == P16_BUFFER_SIZE - 1) {
    for(int i = 0; i < P16_BUFFER_SIZE; i++) {
      p16_buffer[i] = 0;
    }
    if (svp_ftell(fp) + 2*P16_BUFFER_SIZE < p16_file_size) {
      p16_read_size = 2*P16_BUFFER_SIZE;
    } else {
      p16_read_size = p16_file_size - svp_ftell(fp);
    }
    svp_fread(fp, p16_buffer, p16_read_size);
      
    p16_buffer_pos = 0;
  } else {
    p16_buffer_pos++;
  }
  
  return r;
}


uint32_t p16_get_fpos(svp_file * fp) {

  if(p16_buffer_init == 1) {
    return (svp_ftell(fp) - p16_read_size + (uint32_t)p16_buffer_pos*2);
  } else {
    return svp_ftell(fp);
  }
}


uint16_t p16_get_pixel(svp_file * fp, p16Header * header, p16State * state) {
  uint16_t color;

  if (header->storageMode == 0) {
    color = p16_read_buffered(fp);
    return color;
  }

  if (header->storageMode == 1) {
    if (state->init == 0) {
      //svp_fread(fp, &color, sizeof(color));
      color = p16_read_buffered(fp);
      state->fpos = svp_ftell(fp);
      state->init = 1;
      state->prevVal = 0;
      state->repeat  = 0;
      state->prevVal = color;
      return color;
    } else {
      if (state->repeat == 0) {
        //svp_fread(fp, &color, sizeof(color));
        color = p16_read_buffered(fp);
        if (state->prevVal == color) {
          //svp_fread(fp, &state->repeat, sizeof(state->repeat));
          state->repeat = p16_read_buffered(fp);
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

  if(svp_get_size(&fp) < P16_BUFFER_SIZE*2 && p16_file_crc == crc32b(filename)) {
    p16_buffer_keep = 1;
  } else {
    p16_buffer_keep = 0;
    p16_file_crc = crc32b(filename);
  }

  if(p16_get_header(&fp, &header)) {
    printf("%s: Error decoding header %s!\n",__FUNCTION__, filename);
    return 1;
  }

  irq_redraw_block_enable();

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
    //printf("drawing1 %s, x1: %d, y1: %d\n", c->pscgElements[id].str_value, x1, y1);
      r = (uint8_t)((color>>11)&0x1F);
      g = (uint8_t)(((color&0x07E0)>>5)&0x3F);
      b = (uint8_t)(color&0x1F);

      color = (((r+p16_R)/2) & 0x1F)<<11 | (((g+p16_G)/2)<<5  & 0x7E0 ) | (((b+p16_B)/2)&0x1F);

    }

    LCD_canvas_putcol(color);
    if (svpSGlobal.breakP16Draw == 1) {break_draw_cleanup(&fp); return 0;}

    if (LCD_canvas_get_y_overflow()) {
      break;
    }
  }

  LCD_setDrawAreaS(&area);
  svp_fclose(&fp);
  irq_redraw_block_disable();
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

  p16_buffer_keep = 0;

  if (!svp_fopen_read(&fp, filename)) {
    printf("sda_draw_p16: Error while opening file %s!\n", filename);
    return 1;
  }

  if(p16_get_header(&fp, &header)) {
    printf("%s: Error decoding header %s!\n",__FUNCTION__, filename);
    return 1;
  }

  irq_redraw_block_enable();

  LCD_getDrawArea(&area);
  LCD_setSubDrawArea(x, y, x + header.imageWidth * width_n, y + header.imageHeight * height_n);
  LCD_canvas_set(x, y, x + header.imageWidth * width_n - 1, y + header.imageHeight * height_n - 1);

  imageState.init = 0;
  imageState.repeat = 0;
  imageState.prevVal = 0;

  uint16_t color;
  uint16_t pix = 0;

  for(uint32_t n = 0; n < header.imageHeight; n++) {
    fpos = p16_get_fpos(&fp);
    prevVal = imageState.prevVal;
    repeat = imageState.repeat;
    init = imageState.init;

    for (int a = 0; a < height_n; a++) {
      svp_fseek(&fp, fpos);
      p16_buffer_reset();
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
          if (svpSGlobal.breakP16Draw == 1) {break_draw_cleanup(&fp); return 0;}
        }
        
        if (LCD_canvas_get_y_overflow()) {
          break_draw_cleanup(&fp); return 0;
        }
      }
    }
  }

  svp_fclose(&fp);
  LCD_setDrawAreaS(&area);
  irq_redraw_block_disable(); 
  return 0;
}


// size 0 - 1/2, 1 - 1/4, 2 - 1/8, 3 - 1/16

uint8_t sda_draw_p16_scaled_down(uint16_t x, uint16_t y, uint16_t width_n, uint16_t height_n, uint8_t *filename) {
  svp_file fp;
  p16Header header;
  p16State imageState;
  LCD_drawArea area;
  uint32_t drawn_width;
  uint32_t drawn_height;

  if (height_n == 0) {
    height_n = 2;
  } else if (height_n == 1) {
    height_n = 4;
  } else if (height_n == 2) {
    height_n = 8;
  } else {
    height_n = 16;
  }

  if (width_n == 0) {
    width_n = 2;
  } else if (width_n == 1) {
    width_n = 4;
  } else if (width_n == 2) {
    width_n = 8;
  } else {
    width_n = 16;
  }

  if (!svp_fopen_read(&fp, filename)) {
    printf("sda_draw_p16: Error while opening file %s!\n", filename);
    return 1;
  }

  if(p16_get_header(&fp, &header)) {
    printf("%s: Error decoding header %s!\n",__FUNCTION__, filename);
    return 1;
  }

  irq_redraw_block_enable();

  LCD_getDrawArea(&area);
  
  drawn_width = header.imageWidth / width_n + header.imageWidth % width_n;
  drawn_height = header.imageHeight / height_n + header.imageHeight % height_n;
  
  LCD_setSubDrawArea(x, y, x + drawn_width, y + drawn_height);
  LCD_canvas_set(x, y, x + drawn_width - 1, y + drawn_height - 1);

  //printf("draw: n:%u %u, size: %u %u\n", width_n, height_n, drawn_height, drawn_width);

  imageState.init = 0;
  imageState.repeat = 0;
  imageState.prevVal = 0;

  uint16_t color;
  uint32_t pix = 0;

  for(uint32_t n = 0; n < header.imageHeight; n++) {
    for(uint32_t b = 0; b < drawn_width; b++) {
      for(uint32_t c = 0; c < width_n; c++) {
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
      }

      LCD_canvas_putcol(color);
      pix++;
      if (svpSGlobal.breakP16Draw == 1) {break_draw_cleanup(&fp); return 0;}
    }
    
    if (header.imageWidth % 2) {
      color = p16_get_pixel(&fp, &header, &imageState);
      LCD_canvas_putcol(color);
    }

    for(uint16_t x = 0; x < height_n - 1; x++) {
      for(uint16_t b = 0; b < header.imageWidth; b++) {
        color = p16_get_pixel(&fp, &header, &imageState);
      }
    }

    if (pix >= drawn_width*drawn_height) {
      break;
    }

    if (LCD_canvas_get_y_overflow()) {
      break;
    }
  }

  svp_fclose(&fp);
  LCD_setDrawAreaS(&area);
  irq_redraw_block_disable();
  return 0;
}


uint16_t sda_p16_get_width(uint8_t *filename) {
  svp_file fp;
  p16Header header;

  if (!svp_fopen_read(&fp, filename)) {
    printf("sda_p16_get_width: Error while opening file %s!\n", filename);
    return 1;
  }

  if(p16_get_header(&fp, &header)) {
    printf("%s: Error decoding header %s!\n",__FUNCTION__, filename);
    return 1;
  }

  svp_fclose(&fp);
  return header.imageWidth;
}

uint16_t sda_p16_get_height(uint8_t *filename) {
  svp_file fp;
  p16Header header;

  if (!svp_fopen_read(&fp, filename)) {
    printf("sda_p16_get_height: Error while opening file %s!\n", filename);
    return 1;
  }

  p16_get_header(&fp, &header);
  svp_fclose(&fp);
  return header.imageHeight;
}

static void break_draw_cleanup(svp_file *fp) {
  svp_fclose(fp);
  svpSGlobal.breakP16Draw = 0;
  irq_redraw_block_disable();
}
