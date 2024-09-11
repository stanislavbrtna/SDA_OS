/*
Copyright (c) 2023 Stanislav Brtna

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

#include "sda_images.h"

// gets if file is p16 image
uint8_t sda_get_if_p16(uint8_t * filename) {
  uint32_t fnameLen = 0;

  fnameLen = sda_strlen(filename);

  if(fnameLen < 3) {
    return 0;
  }

  if((filename[fnameLen - 3] == 'p' ||  filename[fnameLen - 3] == 'P') &&
      filename[fnameLen - 2] == '1' &&
      filename[fnameLen - 1] == '6'
    ) {
    return 1;
  }

  return 0;
}

// gets if file is ppm image
uint8_t sda_get_if_ppm(uint8_t * filename) {
  uint32_t fnameLen = 0;

  fnameLen = sda_strlen(filename);

  if(fnameLen < 3) {
    return 0;
  }

  if((filename[fnameLen - 3] == 'p' || filename[fnameLen - 3] == 'P') &&
     (filename[fnameLen - 2] == 'p' || filename[fnameLen - 3] == 'P') &&
     (filename[fnameLen - 1] == 'm' || filename[fnameLen - 3] == 'M')
    ) {
    return 1;
  }

  return 0;
}

// sets mix color for drawn image
void sda_img_set_mix_color(uint8_t enable, uint16_t color) {
  sda_p16_set_pmc(enable, color);
  svp_ppm_set_pmc(enable, color);
}


void sda_img_draw(int16_t x, int16_t y, int16_t scale_w, int16_t scale_h, uint8_t *filename) {
  if (sda_get_if_p16(filename)) {
    sda_draw_p16_scaled(x, y, scale_w, scale_h, filename);
    return;
  }

  if (sda_get_if_ppm(filename)) {
    draw_ppm(x, y, scale_w, filename);
    return;
  }

  printf("%s: %s image type not supported!\n", __FUNCTION__, filename);
}


uint16_t sda_img_get_width(uint8_t *filename) {
  if (sda_get_if_p16(filename)) {
    return sda_p16_get_width(filename);
  }

  if (sda_get_if_ppm(filename)) {
    return ppm_get_width(filename);
  }

  printf("%s: %s image type not supported!\n", __FUNCTION__, filename);
  return 0;
}

uint16_t sda_img_get_height(uint8_t *filename) {
  if (sda_get_if_p16(filename)) {
    return sda_p16_get_height(filename);
  }

  if (sda_get_if_ppm(filename)) {
    return ppm_get_height(filename);
  }

  printf("%s: %s image type not supported!\n", __FUNCTION__, filename);
  return 0;
}