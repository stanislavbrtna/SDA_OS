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

void svp_ppm_set_pmc(uint8_t enable, uint16_t color) {
  ppm_use_pmc = enable;
  ppm_R = (uint8_t)(((float)((color>>11)&0x1F)/32)*256);
  ppm_G = (uint8_t)(((float)(((color&0x07E0)>>5)&0x3F)/64)*256);
  ppm_B = (uint8_t)(((float)(color&0x1F)/32)*256);

  sda_p16_set_pmc(enable, color);
}

uint8_t get_if_p16(uint8_t * filename) {
  uint32_t fnameLen = 0;

  fnameLen = sda_strlen(filename);

  if(filename[fnameLen - 3] == 'p' && filename[fnameLen - 2] == '1' && filename[fnameLen-1] == '6') {
    return 1;
  }

  return 0;
}

void draw_ppm(uint16_t x, uint16_t y, uint8_t scale, uint8_t *filename) {

	svp_file fp;
	uint8_t ch[16];
	uint8_t ch2 = 0;
	uint16_t a, color;
	uint32_t fpos = 0;
	uint32_t fpos_line_begin;
	uint8_t r, g, b;
	uint16_t img_width = 0;
	uint16_t img_height = 0;
	int16_t xi = 0;
	int16_t yi = 0;
	uint8_t n, laneScaleCnt;


  if(get_if_p16(filename)) {
    sda_draw_p16(x, y, filename);
    return;
  }


	if (!svp_fopen_read(&fp,filename)) {
	  printf("draw_ppm: Error while opening file %s!\n", filename);
	  return;
	}
	touch_lock = SDA_LOCK_LOCKED;
	ch[0] = svp_fread_u8(&fp);
	ch[1] = svp_fread_u8(&fp);
	if ((ch[0] != 'P') && (ch[1] != '6')) {
		printf("draw_ppm: Error: wrong header\n");
		touch_lock = SDA_LOCK_UNLOCKED;
		return;
	}
	fpos = 3;
	while (ch2 != 10) {
		svp_fseek(&fp, sizeof(uint8_t) * (fpos));
		ch2 = svp_fread_u8(&fp);
		fpos++;
	}
	ch2 = 0;
	a = 0;
	while (ch2 != 10) {
		svp_fseek(&fp, sizeof(uint8_t) * (fpos));
		ch2 = svp_fread_u8(&fp);
		ch[a] = ch2;
		fpos++;
		a++;
	}
	ch[a] = 10;

	a = 0;
	while (ch[a] != ' ') {
		img_width *= 10;
		img_width += ch[a] - 48;
		a++;
	}
	a++;
	while (ch[a] != 10) {
		img_height *= 10;
		img_height += ch[a] - 48;
		a++;
	}

	ch2 = 0;

	while(ch2 != 10) {
		svp_fseek(&fp, sizeof(uint8_t) * (fpos));
		ch2 = svp_fread_u8(&fp);
		fpos++;
	}

	LCD_setSubDrawArea(x, y, x + img_width * scale, y + img_height * scale);
	LCD_canvas_set(x, y, x + img_width * scale - 1, y + img_height * scale - 1);
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

		if (xi == img_width - 1) {
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

		if (yi == img_height - 1) {
			break;
		}

		if (ppm_use_pmc) {
		  r = (uint8_t)(((uint16_t)r + (uint16_t)ppm_R) / 2);
		  g = (uint8_t)(((uint16_t)g + (uint16_t)ppm_G) / 2);
		  b = (uint8_t)(((uint16_t)b + (uint16_t)ppm_B) / 2);
		}

		r = ( r * 249 + 1014 ) >> 11; //5
		g = ( g * 253 + 505 ) >> 10; //6
		b = ( b * 249 + 1014 ) >> 11; //5

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
	uint8_t ch[16];
	uint8_t ch2 = 0;
	uint16_t a;
	uint32_t fpos = 0;
	uint16_t img_width = 0;
	uint16_t img_height = 0;

  if(get_if_p16(filename)){
    return sda_p16_get_width(filename);
  }

	ch[0] = 0;

	if (!svp_fopen_read(&fp, filename)) {
	  printf("ppm_get_width: Error while opening file %s!\n",filename);
	  return 0;
	}

	ch[0] = svp_fread_u8(&fp);
	ch[1] = svp_fread_u8(&fp);
	if ((ch[0] != 'P') && (ch[1] != '6')) {
		printf("ppm_get_width: Error: wrong header\n");
		return 0;
	}
	fpos = 3;
	while (ch2 != 10) {
		svp_fseek(&fp, sizeof(uint8_t) * (fpos));
		ch2 = svp_fread_u8(&fp);
		fpos++;
	}

	ch2 = 0;

	a = 0;
	while (ch2 != 10) {
		svp_fseek(&fp, sizeof(uint8_t) * (fpos));
		ch2 = svp_fread_u8(&fp);
		ch[a] = ch2;
		fpos++;
		a++;
	}
	ch[a] = 10;

	a = 0;
	if (ch[0] == 0) {
	  printf("ppm_get_width: Error: file header corruped!\n");
	  return 0;
	}
	while (ch[a] != ' ') {
		img_width *= 10;
		img_width += ch[a] - 48;
		a++;
	}
	a++;
	while (ch[a] != 10) {
		img_height *= 10;
		img_height += ch[a] - 48;
		a++;
	}

	svp_fclose(&fp);
	return img_width;
}
