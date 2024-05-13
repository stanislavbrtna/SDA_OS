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

#include "tray_custom_icon.h"

extern uint16_t trayBackgroundColor;
extern volatile uint8_t irq_redraw;

#define SDA_CUSTOM_ICONS 4

typedef struct {
  uint8_t      image[128];
  gr2EventType event;
  uint16_t     pid;
  uint8_t      callback[NAME_LENGTH];
  uint8_t      valid;
} sdaTrayIcon;

sdaTrayIcon icons[SDA_CUSTOM_ICONS];

void draw_icon(int16_t x2, int16_t y1, uint8_t * icon);

void sda_custom_icon_init() {
  for(uint16_t i = 0; i < SDA_CUSTOM_ICONS; i++) {
    icons[i].valid = 0;
    icons[i].pid = 0;
  }
}


uint8_t sda_custom_icon_set(uint8_t *img, uint16_t pid, uint8_t* cb) {
  // find valid spot
  uint16_t valid_spot = 0;

  for(uint16_t i = 0; i < SDA_CUSTOM_ICONS; i++) {
    if (icons[i].valid == 0) {
      valid_spot = i + 1;
      break;
    }
  }

  if (valid_spot == 0) {
    printf("%s: No empty icon spot!\n", __FUNCTION__);
    return 0;
  }
  // push data in
  for(uint16_t i = 0; i < 128; i++) {
    icons[valid_spot - 1].image[i] = img[i];
  }

  icons[valid_spot - 1].pid = pid;
  icons[valid_spot - 1].event = EV_NONE;

  sda_strcp(cb, icons[valid_spot - 1].callback, NAME_LENGTH);

  icons[valid_spot - 1].valid = 1;
  svp_set_irq_redraw();

  // return id (1 - 4)
  return valid_spot;
}


void sda_custom_icon_release_spot(uint8_t spot) {
  if(spot >= SDA_CUSTOM_ICONS || spot == 0) {
    printf("%s: Spot out of range!\n", __FUNCTION__);
    return;
  }
  icons[spot - 1].valid = 0;
  svp_set_irq_redraw();
}


uint8_t sda_custom_icon_release_spot_pid(uint8_t spot, uint16_t pid) {
  if(spot >= SDA_CUSTOM_ICONS || spot == 0) {
    printf("%s: Spot out of range!\n", __FUNCTION__);
    return 1;
  }
  if (icons[spot - 1].pid != pid) {
    printf("%s: Icon not owned by given pid!\n", __FUNCTION__);
    return 1;
  }
  icons[spot - 1].valid = 0;
  svp_set_irq_redraw();
  return 0;
}


void sda_custom_icon_release_pid(uint16_t pid) {
  
  if (pid == 0) return;

  for(uint16_t i = 0; i < SDA_CUSTOM_ICONS; i++) {
    if (icons[i].valid == 1 && icons[i].pid == pid) {
      icons[i].valid = 0;
    }
  }
  svp_set_irq_redraw();
}


int16_t sda_custom_icon_handle(int16_t x2, int16_t y1, int16_t w) {
  static uint8_t redraw;

  int16_t x1 = x2 - w;
  int16_t icon_pos = 0;

  if((redraw == 0) || (irq_redraw)) {
    redraw = 1;

    LCD_FillRect(x1, y1, x2, 32, trayBackgroundColor);
    
    for(uint16_t i = 0; i < SDA_CUSTOM_ICONS; i++) {
      if (icons[i].valid) {
        draw_icon(x2 - 32*icon_pos, y1, icons[i].image);
        icon_pos++;
      }
    }
  }

  icon_pos = 0;
  for(uint16_t i = 0; i < SDA_CUSTOM_ICONS; i++) {
    if (icons[i].valid) {
      if (sda_tray_clicked(x2 - 32*(icon_pos + 1) , y1, x2 - 32*icon_pos, y1 + 31) == EV_RELEASED) {
        icons[i].event = EV_RELEASED;
      }
      icon_pos++;
    }
  }

  return w;
}


void draw_icon(int16_t x2, int16_t y1, uint8_t * icon) {
  int16_t x1 = x2 - 32;
  
  // using canvas
  LCD_canvas_set(x1, y1 + 1, x2 - 1, y1 + 33);
  LCD_canvas_zero();

  uint8_t  bit_n = 0;
  uint16_t icon_pos = 0;

  for(uint32_t i = 0; i < 32*32; i++) {
    if ((1 << bit_n) & icon[icon_pos]) {
      LCD_canvas_putcol(gr2_get_text_color(&sda_sys_con));
    } else {
      LCD_canvas_putcol(trayBackgroundColor);
    }
    bit_n++;
    if (bit_n == 8) {
      bit_n = 0;
      icon_pos++;
    }
  }
}


void sda_custom_icon_handle_svm_events() {
  for(uint16_t i = 0; i < SDA_CUSTOM_ICONS; i++) {
    if (icons[i].valid == 1 && icons[i].event == EV_RELEASED && icons[i].pid != 0) {
      svmHandleNotifIconCallback(icons[i].pid, icons[i].callback);
      icons[i].event = EV_NONE;
      // return and let the update loop handle stuff
      return;
    }
  }
}