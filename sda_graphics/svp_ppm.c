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

#include "../SDA_OS.h"

static uint8_t ppm_R;
static uint8_t ppm_G;
static uint8_t ppm_B;
static uint8_t ppm_use_pmc;

typedef struct {
  uint16_t width;
  uint16_t height;
} svp_ppm_header;


void svp_ppm_set_pmc(uint8_t enable, uint16_t color) {
  ppm_use_pmc = enable;
  ppm_R = (uint8_t)(((float)((color>>11)&0x1F)/32)*256);
  ppm_G = (uint8_t)(((float)(((color&0x07E0)>>5)&0x3F)/64)*256);
  ppm_B = (uint8_t)(((float)(color&0x1F)/32)*256);
}


uint8_t sda_ppm_get_header(svp_ppm_header *p, svp_file *fp) {
  uint8_t  ch2 = 0;
  uint8_t  ch[16];
  uint32_t a;

  p->width  = 0;
  p->height = 0;

  // header verification
  ch[0] = svp_fread_u8(fp);
  ch[1] = svp_fread_u8(fp);

  if ((ch[0] != 'P') && (ch[1] != '6')) {
    printf("%s: Error: wrong header\n", __FUNCTION__);
    return 1;
  }

  ch[0] = svp_fread_u8(fp);

  if (ch[0] != 10) {
    printf("%s: Error: unparsable header\n",__FUNCTION__);
    return 1;
  }

  // detect for gimp commentary
  ch[0] = svp_fread_u8(fp);
  if(ch[0] == '#') {
    while (ch[0] != 10) {  
      ch[0] = svp_fread_u8(fp);
      if (svp_feof(fp)) {
        printf("%s: Error: unparsable header (2)\n", __FUNCTION__);
        return 1;
      }
    }
  } else {
    // seek back
    svp_fseek(fp, svp_ftell(fp) - 1);
  }

  a = 0;
  while (ch2 != 10) {
    ch2 = svp_fread_u8(fp);
    ch[a] = ch2;
    a++;

    if (svp_feof(fp) || a > 15) {
      printf("%s: Error: unparsable header (2)\n", __FUNCTION__);
      return 1;
    }
  }
  ch[a] = 10;

  // parse width/lenght
  a = 0;
  while (ch[a] != ' ') {
    p->width *= 10;
    p->width += ch[a] - 48;
    a++;

    if(a > 15) {
      printf("%s: Error: unparsable width\n", __FUNCTION__);
      return 1;
    }
  }
  a++;
  while (ch[a] != 10) {
    p->height *= 10;
    p->height += ch[a] - 48;
    a++;
    if(a > 15) {
      printf("%s: Error: unparsable height\n", __FUNCTION__);
      return 0;
    }
  }
  ch2 = 0;

  // skip the max value
  while(ch2 != 10) {
    ch2 = svp_fread_u8(fp);

    if (svp_feof(fp)){
      printf("%s: Error: unparsable header (5)\n", __FUNCTION__);
      return 1;
    }
  }

  return 0;
}


void draw_ppm(uint16_t x, uint16_t y, uint8_t scale, uint8_t *filename) {
  uint16_t color;
  uint32_t fpos = 0;
  uint32_t fpos_line_begin;
  uint8_t  r, g, b, n, laneScaleCnt;
  int16_t  xi = 0;
  int16_t  yi = 0;

  svp_file fp;
  svp_ppm_header img;

  if(!sda_get_if_ppm(filename)) {
    printf("draw_ppm: File %s is unsupported!\n", filename);
    return;
  }

  if (!svp_fopen_read(&fp, filename)) {
    printf("draw_ppm: Error while opening file %s!\n", filename);
    return;
  }

  if(sda_ppm_get_header(&img, &fp)) {
    printf("%s: get header failed!\n", __FUNCTION__);
    return;
  }

  if (img.width == 0 || img.height == 0){
    printf("draw_ppm: Error: image dimension is zero\n");
    return;
  }

  touch_lock = SDA_LOCK_LOCKED;

  LCD_setSubDrawArea(x, y, x + img.width * scale, y + img.height * scale);
  LCD_canvas_set(x, y, x + img.width * scale, y + img.height * scale);
  fpos = svp_ftell(&fp);
  fpos_line_begin = fpos;
  laneScaleCnt = 0;

  uint8_t rgtmp[3];

  while (1) {
    if (scale != 1) {
      svp_fseek(&fp, sizeof(uint8_t) * (fpos));
    }

    svp_fread(&fp, rgtmp, sizeof(rgtmp));
    fpos += 3;

    r = rgtmp[0];
    g = rgtmp[1];
    b = rgtmp[2];

    if (xi == img.width - 1) {
      laneScaleCnt++;
      if (laneScaleCnt == scale) {
        yi++;
        xi = 0;
        laneScaleCnt = 0;
        fpos_line_begin = fpos;
      } else {
        xi = 0;
        fpos = fpos_line_begin;
      }
    } else {
      xi++;
    }

    if (yi == img.height - 1) {
      break;
    }

    if (ppm_use_pmc) {
      r = (uint8_t)(((uint16_t)r + (uint16_t)ppm_R) / 2);
      g = (uint8_t)(((uint16_t)g + (uint16_t)ppm_G) / 2);
      b = (uint8_t)(((uint16_t)b + (uint16_t)ppm_B) / 2);
    }

    r = ( r * 249 + 1014 ) >> 11;  // 5
    g = ( g * 253 +  505 ) >> 10;  // 6
    b = ( b * 249 + 1014 ) >> 11;  // 5

    color = r << 11 | (g & 0x3F) << 5 | (b & 0x1F);

    for(n = scale; n > 0 ; n--) {
      LCD_canvas_putcol(color);
    }
  }
  svp_fclose(&fp);
  touch_lock = SDA_LOCK_UNLOCKED;
  return;
}


uint16_t ppm_get_width(uint8_t *filename) {
  svp_file fp;
  svp_ppm_header img;

  if (!svp_fopen_read(&fp, filename)) {
    printf("%s: Error while opening file %s!\n",__FUNCTION__, filename);
    return 0;
  }

  if(sda_ppm_get_header(&img, &fp)) {
    printf("%s: get header failed!\n", __FUNCTION__);
    return 0;
  }

  svp_fclose(&fp);
  return img.width;
}


uint16_t ppm_get_height(uint8_t *filename) {
  svp_file fp;
  svp_ppm_header img;

  if (!svp_fopen_read(&fp, filename)) {
    printf("%s: Error while opening file %s!\n",__FUNCTION__, filename);
    return 0;
  }

  if(sda_ppm_get_header(&img, &fp)) {
    printf("%s: get header failed!\n", __FUNCTION__);
    return 0;
  }

  svp_fclose(&fp);
  return img.height;
}
