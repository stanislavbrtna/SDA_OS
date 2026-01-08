/*
Copyright (c) 2021 Stanislav Brtna

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

// power options overlay
uint16_t batt_overlay;
uint8_t batt_overlay_flag;

static uint8_t bl_icon[] = {
  0x02, // Format descriptor
  0x18, // img_width
  0x18, // img_height
  0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 
  0x00, 0x00, 0x18, 0x00, 0x30, 0xff, 0x0c, 0xf0, 
  0xff, 0x0f, 0xe0, 0x8f, 0x07, 0xe0, 0x0f, 0x06, 
  0xf0, 0x0f, 0x0e, 0xf0, 0x0f, 0x0c, 0xf0, 0x0f, 
  0x0c, 0xff, 0x0f, 0xfc, 0xff, 0x0f, 0xfc, 0xf0, 
  0x0f, 0x0c, 0xf0, 0x0f, 0x0c, 0xf0, 0x0f, 0x0e, 
  0xe0, 0x0f, 0x06, 0xe0, 0x8f, 0x07, 0xf0, 0xff, 
  0x0f, 0x30, 0xff, 0x0c, 0x00, 0x18, 0x00, 0x00, 
  0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 
};

extern uint8_t mute_icon[];

//TBR??
uint8_t vol_up_icon[] = {
  0x02, // Format descriptor
  0x20, // img_width
  0x20, // img_height
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
  0x00, 0x80, 0x01, 0x02, 0x00, 0xc0, 0x01, 0x02, 
  0x00, 0xe0, 0x01, 0x06, 0x00, 0xf0, 0x01, 0x06, 
  0x00, 0xf8, 0x11, 0x06, 0x00, 0xfc, 0x31, 0x0e, 
  0xe0, 0xff, 0x21, 0x0e, 0xe0, 0xff, 0x21, 0x0e, 
  0xe0, 0xff, 0x21, 0x0e, 0xe0, 0xff, 0x21, 0x0e, 
  0xe0, 0xff, 0x21, 0x0e, 0xe0, 0xff, 0x21, 0x0e, 
  0xe0, 0xff, 0x21, 0x0e, 0x00, 0xfc, 0x21, 0x06, 
  0x00, 0xf8, 0x31, 0x06, 0x00, 0xf0, 0x11, 0x06, 
  0x00, 0xe0, 0x01, 0x06, 0x00, 0xc0, 0x01, 0x02, 
  0x00, 0x80, 0x01, 0x02, 0x00, 0x00, 0x01, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

uint8_t vol_sld_icon[] = {
  0x02, // Format descriptor
  0x19, // img_width
  0x18, // img_height
  0x00, 0x10, 0x00, 0x00, 0x30, 0xc0, 0x00, 0x70, 
  0x80, 0x01, 0xf0, 0x00, 0x07, 0xf0, 0x01, 0x0e, 
  0xf0, 0x23, 0x1c, 0xf0, 0xc7, 0x30, 0xf0, 0x8f, 
  0xe3, 0xff, 0x1f, 0xc6, 0xff, 0x3f, 0x8c, 0xff, 
  0x7f, 0x18, 0xff, 0xff, 0x30, 0xfe, 0xff, 0x61, 
  0xfc, 0xff, 0xc3, 0xf8, 0xff, 0x87, 0xf1, 0xff, 
  0x0f, 0x63, 0xc0, 0x1f, 0xc7, 0x00, 0x3f, 0xc6, 
  0x01, 0x7c, 0x84, 0x03, 0xf0, 0x00, 0x07, 0xc0, 
  0x01, 0x06, 0x00, 0x03, 0x0c, 0x00, 0x04, 0x00, 
  0x00, 0x00, 0x00,  
};

uint8_t haptic_icon[] = {
  0x02, // Format descriptor
  0x23, // img_width
  0x1c, // img_height
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 
  0x07, 0x00, 0xe0, 0xff, 0x3f, 0x00, 0x00, 0xff, 
  0xff, 0x01, 0x00, 0x38, 0x00, 0x0e, 0x80, 0xc4, 
  0x01, 0x70, 0x24, 0x26, 0x0e, 0x80, 0x23, 0xb3, 
  0x71, 0x00, 0x1c, 0x9b, 0x8e, 0x03, 0xe0, 0xb8, 
  0x36, 0x1c, 0x00, 0x87, 0xbd, 0xe1, 0x00, 0x38, 
  0xec, 0x0d, 0x07, 0xc0, 0x61, 0x67, 0x38, 0x00, 
  0x0e, 0x33, 0xc3, 0x01, 0x70, 0x98, 0x1b, 0x0e, 
  0x80, 0xc3, 0xde, 0x70, 0x00, 0x1c, 0xf6, 0x86, 
  0x03, 0xe0, 0xb0, 0x75, 0x1c, 0x00, 0xc7, 0x65, 
  0xe3, 0x00, 0x38, 0x36, 0x1b, 0xff, 0xff, 0xb1, 
  0xd0, 0xf8, 0xff, 0x8f, 0x04, 0xc0, 0xff, 0x7f, 
  0x00, 0x00, 0xce, 0x98, 0x03, 0x00, 0x70, 0xc6, 
  0x1c, 0x00, 0x80, 0xff, 0xff, 0x00, 0x00, 0xfc, 
  0xff, 0x07, 0x00,
};


void sda_batt_overlay_init() {
  if (batt_overlay_flag == 0) {
    destroyOverlay();
    int16_t h = batt_overlay_handle(1);
    setOverlayScreen(batt_overlay, &sda_sys_con);
    setOverlayDestructor(sda_batt_overlay_destructor);
    batt_overlay_flag = 1;
    setOverlayY2(h + 80);
  }
}


void sda_batt_overlay_destructor() {
  gr2_destroy(batt_overlay, &sda_sys_con);
  setRedrawFlag();
  batt_overlay_flag = 0;
  batt_overlay = 0xFFFF;
}


uint8_t sda_batt_overlay_shown() {
  return batt_overlay_flag;
}

static uint16_t backlightSlider;
static uint16_t volumeSlider;
static uint16_t soundEnable;
static uint16_t hapticEnable;

static void setVolIcon() {
  if(svpSGlobal.mute) {
    gr2_set_ghost(soundEnable, 0, &sda_sys_con);
    gr2_set_grayout(volumeSlider, 1, &sda_sys_con);
  } else {
    gr2_set_ghost(soundEnable, 1, &sda_sys_con);
    gr2_set_grayout(volumeSlider, 0, &sda_sys_con);
  }
}


int16_t batt_overlay_handle(uint8_t init) {
  static uint16_t backlightOk;
  static uint8_t  backlightOld;

  if (init == 1) {
    uint8_t relInit = gr2_get_relative_init(&sda_sys_con);
    gr2_set_relative_init(1, &sda_sys_con);
    int16_t y1 = 1;
    
    batt_overlay = gr2_add_screen(&sda_sys_con);
    gr2_set_yscroll(batt_overlay, 4, &sda_sys_con);
    gr2_set_x_cell(batt_overlay, 16, &sda_sys_con);
    gr2_set_y_cell(batt_overlay, 17, &sda_sys_con);
    
    backlightSlider = gr2_add_slider_h(
      1, y1, 14,  2,
      255 - MIN_BACKLIGHT_VALUE,
      svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
      batt_overlay,
      &sda_sys_con
    );
    gr2_set_str2(backlightSlider, bl_icon, &sda_sys_con);
    y1 += 3;

#ifdef SDA_FEATURE_PCM_SOUND
    volumeSlider = gr2_add_slider_h(
      1, y1, 14,  2,
      255 - MIN_BACKLIGHT_VALUE,
      svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
      batt_overlay,
      &sda_sys_con
    );
    gr2_set_str2(volumeSlider, vol_sld_icon, &sda_sys_con);
    y1 += 3;
#endif

    backlightOk = gr2_add_button(
      11, y1, 4, 2,
      OVRL_OK,
      batt_overlay,
      &sda_sys_con
    );
    gr2_text_set_align(backlightOk, GR2_ALIGN_CENTER, &sda_sys_con);
    
    soundEnable = gr2_add_button(
      1, y1, 2, 2,
      "",
      batt_overlay,
      &sda_sys_con
    );

    gr2_set_str2(soundEnable, mute_icon, &sda_sys_con);
    gr2_set_param(soundEnable, -5, &sda_sys_con);

#ifdef SDA_FEATURE_NOTIF_VIBRO
    hapticEnable = gr2_add_button(
      4, y1, 3, 2,
      "",
      batt_overlay,
      &sda_sys_con
    );

    gr2_set_str2(hapticEnable, haptic_icon, &sda_sys_con);
    gr2_set_param(hapticEnable, 9, &sda_sys_con);
    gr2_set_ghost(hapticEnable, 1 - svpSGlobal.haptics, &sda_sys_con);
#endif
    y1 += 2;

    setVolIcon();
    gr2_set_relative_init(relInit, &sda_sys_con);
    return 16*y1;
  }

  if (gr2_get_event(backlightSlider, &sda_sys_con)) {
    if ((gr2_get_value(backlightSlider, &sda_sys_con) + MIN_BACKLIGHT_VALUE) > 255){
      svpSGlobal.lcdBacklight = 255;
    } else {
      svpSGlobal.lcdBacklight
        = (uint8_t) (gr2_get_value(backlightSlider, &sda_sys_con) + MIN_BACKLIGHT_VALUE);
    }
    svp_set_backlight(svpSGlobal.lcdBacklight);
  }
  gr2_set_event(backlightSlider, EV_NONE, &sda_sys_con);

  if (svpSGlobal.systemPwrLongPress == 1) {
    svpSGlobal.systemPwrLongPress = 0;
    svpSGlobal.lcdBacklight = 255;
    svp_set_backlight(svpSGlobal.lcdBacklight);
    gr2_set_value(
      backlightSlider,
      svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
      &sda_sys_con
    );
  }

  if (gr2_get_event(backlightOk, &sda_sys_con) == EV_RELEASED || svpSGlobal.lcdState == LCD_OFF) {
    batt_overlay_flag = 0;
    setRedrawFlag();
    destroyOverlay();
    return 0;
  }
  gr2_set_event(backlightOk, EV_NONE, &sda_sys_con);

#ifdef SDA_FEATURE_NOTIF_VIBRO
  if(gr2_clicked(hapticEnable, &sda_sys_con)) {
    sda_set_haptics( 1 - svpSGlobal.haptics);
    gr2_set_ghost(hapticEnable, 1 - svpSGlobal.haptics, &sda_sys_con);
  }
#endif

  if (gr2_get_event(soundEnable, &sda_sys_con) == EV_RELEASED) {
    sda_set_mute(1 - svpSGlobal.mute);
    setVolIcon();
  }
  gr2_set_event(soundEnable, EV_NONE, &sda_sys_con);

  
#ifdef SDA_FEATURE_PCM_SOUND
#error "TODO: implement handler..."
#endif
  
  if (svpSGlobal.lcdBacklight != backlightOld) {
    gr2_set_value(
        backlightSlider,
        svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
        &sda_sys_con
    );
  }

  sda_screen_button_handler(batt_overlay, backlightOk, &sda_sys_con);

  backlightOld = svpSGlobal.lcdBacklight;

  return 0;
}