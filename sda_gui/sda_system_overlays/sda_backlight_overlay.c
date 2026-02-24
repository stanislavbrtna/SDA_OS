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
#include <math.h>

extern uint8_t  overlayRedrawFlag;

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
  0x17, // img_width
  0x16, // img_height
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x0c, 
  0x04, 0x00, 0x07, 0x02, 0xc0, 0x03, 0x03, 0xe0, 
  0x81, 0x01, 0xf8, 0xc4, 0x00, 0x7e, 0x46, 0x80, 
  0x3f, 0x62, 0xfe, 0x1f, 0x31, 0xff, 0x8f, 0x98, 
  0xff, 0x47, 0xcc, 0xff, 0x23, 0xe6, 0xff, 0x11, 
  0xf3, 0xff, 0x88, 0x00, 0x7f, 0x44, 0x00, 0x3f, 
  0x33, 0x00, 0x9f, 0x18, 0x00, 0x0f, 0x0c, 0x00, 
  0x07, 0x02, 0x00, 0x03, 0x01, 0x00, 0x01, 0x00, 
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

uint8_t perf_icon[] = {
  0x02, // Format descriptor
  0x1a, // img_width
  0x1c, // img_height
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 
  0x63, 0x00, 0x60, 0x8c, 0x01, 0x80, 0x31, 0x06, 
  0xe0, 0xff, 0xff, 0xc1, 0xff, 0xff, 0x0f, 0xff, 
  0xff, 0x3f, 0xfc, 0x87, 0xff, 0xfc, 0x07, 0xf8, 
  0xff, 0x07, 0x80, 0x3f, 0x1f, 0x3f, 0x3e, 0x3c, 
  0xfe, 0xf1, 0xf0, 0xfc, 0xcf, 0xf3, 0xf1, 0x37, 
  0xfe, 0xc7, 0xcf, 0xf8, 0xfc, 0x0f, 0xff, 0xf0, 
  0x1f, 0xfc, 0xc3, 0x7f, 0xf8, 0xcf, 0xff, 0xe1, 
  0xff, 0xff, 0x87, 0xff, 0xf3, 0x3f, 0xff, 0xc3, 
  0xff, 0xff, 0x0f, 0xff, 0xff, 0x3f, 0xf8, 0xff, 
  0x7f, 0x00, 0xc6, 0x18, 0x00, 0x18, 0x63, 0x00, 
  0x60, 0x8c, 0x01,
};

uint8_t batt_full_icon[] = {
  0x02, // Format descriptor
  0x0e, // img_width
  0x1c, // img_height
  0x00, 0x00, 0x00, 0x80, 0x7f, 0xe0, 0x1f, 0xfe, 
  0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xf8, 
  0x00, 0x3e, 0x80, 0x1f, 0xe0, 0xff, 0xff, 0xff, 
  0x7f, 0x80, 0x0f, 0xe0, 0x03, 0xf8, 0x01, 0xfe, 
  0xff, 0xff, 0xff, 0x07, 0xf8, 0x00, 0x3e, 0x80, 
  0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 
  0x7f,
};

uint8_t batt_60_icon[] = {
  0x02, // Format descriptor
  0xe, // img_width
  0x1c, // img_height
  0x00, 0x00, 0x00, 0x80, 0x7f, 0xe0, 0x1f, 0xfe, 
  0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x7f, 0x80, 0x0f, 0xe0, 0x03, 0xf8, 0x01, 0xfe, 
  0xff, 0xff, 0xff, 0x07, 0xf8, 0x00, 0x3e, 0x80, 
  0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 
  0x7f, 
};

uint8_t batt_30_icon[] = {
  0x02, // Format descriptor
  0xe, // img_width
  0x1c, // img_height
  0x00, 0x00, 0x00, 0x80, 0x7f, 0xe0, 0x1f, 0xfe, 
  0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0x07, 0xf8, 0x00, 0x3e, 0x80, 
  0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 
  0x7f, 
};

uint8_t batt_0_icon[] = {
  0x02, // Format descriptor
  0xe, // img_width
  0x1c, // img_height
  0x00, 0x00, 0x00, 0x80, 0x7f, 0xe0, 0x1f, 0xfe, 
  0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xc0, 
  0x1f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 
  0x7f,
};

uint8_t phones_icon[] =  {
  0x02, // Format descriptor
  0x18, // img_width
  0x16, // img_height
  0x00, 0x7c, 0x00, 0x80, 0xff, 0x01, 0xc0, 0x83, 
  0x07, 0xe0, 0x00, 0x0e, 0x70, 0x00, 0x1c, 0x30, 
  0x00, 0x18, 0x18, 0x00, 0x30, 0x18, 0x00, 0x30, 
  0x18, 0x00, 0x30, 0x0c, 0x00, 0x60, 0x0c, 0x00, 
  0x60, 0x0c, 0x00, 0x60, 0x0c, 0x00, 0x60, 0x3c, 
  0x00, 0x78, 0x7e, 0x00, 0xfc, 0xfe, 0x00, 0xfe, 
  0xfe, 0x00, 0xfe, 0xfe, 0x00, 0xfe, 0xfe, 0x00, 
  0xfe, 0xfe, 0x00, 0xfe, 0x7e, 0x00, 0xfc, 0x3c, 
  0x00, 0x78, 
};


static uint16_t ovrY2;

void sda_batt_overlay_init() {
  if (batt_overlay_flag == 0) {
    destroyOverlay();
    ovrY2 = batt_overlay_handle(1) + 80;
    setOverlayScreen(batt_overlay, &sda_sys_con);
    setOverlayDestructor(sda_batt_overlay_destructor);
    batt_overlay_flag = 1;
    setOverlayY2(ovrY2);
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

static uint16_t backlightOk;

static uint16_t backlightSlider;
static uint16_t volumeSlider;
static uint16_t soundEnable;
static uint16_t hapticEnable;

static uint16_t perfText;
static uint16_t perfImage;
static uint16_t perfScr;
static uint16_t perfBtn;
static uint16_t perfPerfBtn;
static uint16_t perfEcoBtn;
static uint16_t perfAutoBtn;

static uint8_t perfTextStr[32];


static void moveButtons(int16_t val) {
  gr2_set_y1(soundEnable, gr2_get_y1(soundEnable, &sda_sys_con) + val, &sda_sys_con);

  #ifdef SDA_FEATURE_NOTIF_VIBRO
  gr2_set_y1(hapticEnable, gr2_get_y1(hapticEnable, &sda_sys_con) + val, &sda_sys_con);
  gr2_set_y2(hapticEnable, gr2_get_y2(hapticEnable, &sda_sys_con) + val, &sda_sys_con);
#endif

  gr2_set_y1(backlightOk, gr2_get_y1(backlightOk, &sda_sys_con) + val, &sda_sys_con);
  gr2_set_y2(soundEnable, gr2_get_y2(soundEnable, &sda_sys_con) + val, &sda_sys_con);
  gr2_set_y2(backlightOk, gr2_get_y2(backlightOk, &sda_sys_con) + val, &sda_sys_con);
}

static void setVolIcon() {
  if(svpSGlobal.mute) {
    gr2_set_ghost(soundEnable, 0, &sda_sys_con);
    gr2_set_grayout(volumeSlider, 1, &sda_sys_con);
  } else {
    gr2_set_ghost(soundEnable, 1, &sda_sys_con);
    gr2_set_grayout(volumeSlider, 0, &sda_sys_con);
  }
}

static void perfTextUpdate(uint8_t init) {
  static uint8_t percentage;
  static uint16_t freq;
  uint8_t buff[16];

  if (
    svpSGlobal.battPercentage == percentage &&
    svpSGlobal.systemClock == freq &&
    init == 0
  ) {
    return;
  }

  percentage = svpSGlobal.battPercentage;
  freq = svpSGlobal.systemClock;

  sda_int_to_str(buff, svpSGlobal.battPercentage, sizeof(buff));
  
  sda_strcp(buff, perfTextStr, sizeof(buff));
  
  sda_str_add(perfTextStr, "% ");

  sda_int_to_str(buff, svpSGlobal.systemClock, sizeof(buff));

  sda_str_add(perfTextStr, buff);
  
  sda_str_add(perfTextStr, "MHz");

  gr2_set_modified(perfText, &sda_sys_con);

  if(svpSGlobal.battPercentage > 60) {
    gr2_set_str(perfImage, batt_full_icon, &sda_sys_con);
  } else if(svpSGlobal.battPercentage > 30) {
    gr2_set_str(perfImage, batt_60_icon, &sda_sys_con);
  } else if(svpSGlobal.battPercentage > 10){
    gr2_set_str(perfImage, batt_30_icon, &sda_sys_con);
  } else {
    gr2_set_str(perfImage, batt_0_icon, &sda_sys_con);
  }

  gr2_set_modified(perfImage, &sda_sys_con);
}

void perfBtnsUpdate() {
  gr2_set_ghost(perfPerfBtn, 1, &sda_sys_con);
  gr2_set_ghost(perfEcoBtn, 1, &sda_sys_con);
  gr2_set_ghost(perfAutoBtn, 1, &sda_sys_con);

  if(svpSGlobal.pwrClockScalingMode == PERFORMANCE) {
    gr2_set_ghost(perfPerfBtn, 0, &sda_sys_con);
  }

  if(svpSGlobal.pwrClockScalingMode == POWERSAVE) {
    gr2_set_ghost(perfEcoBtn, 0, &sda_sys_con);
  }

  if(svpSGlobal.pwrClockScalingMode == AUTO) {
    gr2_set_ghost(perfAutoBtn, 0, &sda_sys_con);
  }
}

int16_t batt_overlay_handle(uint8_t init) {
  static uint8_t backlightOld;
  static sdaPCMOutputType outputOld;

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
    gr2_set_y_offset(backlightSlider, 0, &sda_sys_con);
    y1 += 3;

#ifdef SDA_FEATURE_PCM_SOUND
    volumeSlider = gr2_add_slider_h(
      1, y1, 14,  2,
      MAX_VOLUME_SLIDER_VALUE,
      sda_get_volume(),
      batt_overlay,
      &sda_sys_con
    );

    gr2_set_y_offset(volumeSlider, 0, &sda_sys_con);

    if(svpSGlobal.outputPCM == SPEAKER) {
      gr2_set_str2(volumeSlider, vol_sld_icon, &sda_sys_con);  
    } else {
      gr2_set_str2(volumeSlider, phones_icon, &sda_sys_con);
    }
    y1 += 3;
#endif

#ifdef SDA_FEATURE_FREQ_SCALING
    perfImage = gr2_add_image(1, y1, 2, 2, batt_full_icon, batt_overlay, &sda_sys_con);

    perfTextUpdate(1);
    perfText = gr2_add_text(
      2, y1, 9, 2,
      perfTextStr,
      batt_overlay,
      &sda_sys_con
    );

    perfBtn = gr2_add_button(
      11, y1, 4, 2,
      "",
      batt_overlay,
      &sda_sys_con
    );
    gr2_set_x_offset(perfBtn, 16, &sda_sys_con);
    
    gr2_set_str2(perfBtn, perf_icon, &sda_sys_con);

    y1 += 3;

    perfScr = gr2_add_screen_ext(1, y1, 14, 2, batt_overlay, &sda_sys_con);

    gr2_set_cell_space_bottom(perfScr, 2, &sda_sys_con);
    gr2_set_cell_space_left(perfScr, 2, &sda_sys_con);
    gr2_set_x_cell(perfScr, 36, &sda_sys_con);
    gr2_set_xscroll(perfScr, -3, &sda_sys_con);
    gr2_set_yscroll(perfScr, -2, &sda_sys_con);

    perfPerfBtn = gr2_add_button(0, 0, 2, 1, "Perf.", perfScr, &sda_sys_con);
    perfEcoBtn = gr2_add_button(2, 0, 2, 1, "Eco.", perfScr, &sda_sys_con);
    perfAutoBtn = gr2_add_button(4, 0, 2, 1, "Auto", perfScr, &sda_sys_con);

    perfBtnsUpdate();

    gr2_set_visible(perfScr, 0, &sda_sys_con);
    
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
    gr2_set_x_offset(soundEnable, -3, &sda_sys_con);

#ifdef SDA_FEATURE_NOTIF_VIBRO
    hapticEnable = gr2_add_button(
      4, y1, 3, 2,
      "",
      batt_overlay,
      &sda_sys_con
    );

    gr2_set_str2(hapticEnable, haptic_icon, &sda_sys_con);
    gr2_set_x_offset(hapticEnable, 4, &sda_sys_con);
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

#ifdef SDA_FEATURE_FREQ_SCALING
  perfTextUpdate(0);
  if(gr2_clicked(perfBtn, &sda_sys_con)) {
    if(gr2_get_visible(perfScr, &sda_sys_con)){
      // hide
      gr2_set_visible(perfScr, 0, &sda_sys_con);
      moveButtons(-3);
      setOverlayY2(ovrY2);
      overlayRedrawFlag = 1;
    } else {
      // show
      gr2_set_visible(perfScr, 1, &sda_sys_con);
      moveButtons(3);
      setOverlayY2(ovrY2 + 48);
    }
    setRedrawFlag();
  }

  if (gr2_clicked(perfPerfBtn, &sda_sys_con)) {
    sda_set_perf_mode(PERFORMANCE);
    perfBtnsUpdate();
  }

  if (gr2_clicked(perfEcoBtn, &sda_sys_con)) {
    sda_set_perf_mode(POWERSAVE);
    perfBtnsUpdate();
  }

  if (gr2_clicked(perfAutoBtn, &sda_sys_con)) {
    sda_set_perf_mode(AUTO);
    perfBtnsUpdate();
  }
#endif

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
  if (gr2_get_event(volumeSlider, &sda_sys_con)) {
    sda_set_volume(gr2_get_value(volumeSlider, &sda_sys_con));
  }
  if (gr2_get_event(volumeSlider, &sda_sys_con) == EV_RELEASED) {
    sda_store_pcm_config();
  }
  gr2_set_event(volumeSlider, EV_NONE, &sda_sys_con);
#endif
  
  if (svpSGlobal.lcdBacklight != backlightOld) {
    gr2_set_value(
        backlightSlider,
        svpSGlobal.lcdBacklight - MIN_BACKLIGHT_VALUE,
        &sda_sys_con
    );
    backlightOld = svpSGlobal.lcdBacklight;
  }

#ifdef SDA_FEATURE_PCM_SOUND
  if (svpSGlobal.outputPCM != outputOld) {
    if(svpSGlobal.outputPCM == SPEAKER) {
      gr2_set_str2(volumeSlider, vol_sld_icon, &sda_sys_con);
    } else {
      gr2_set_str2(volumeSlider, phones_icon, &sda_sys_con);
    }
    gr2_set_value(volumeSlider, sda_get_volume(), &sda_sys_con);
    gr2_set_modified(volumeSlider, &sda_sys_con);
    outputOld = svpSGlobal.outputPCM;
  }
#endif

  sda_screen_button_handler(batt_overlay, backlightOk, &sda_sys_con);
  
  return 0;
}