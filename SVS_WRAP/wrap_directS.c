/*
Copyright (c) 2017 Stanislav Brtna

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

#define IRQ_BLOCK irq_redraw_block_enable();
#define IRQ_ENABLE irq_redraw_block_disable();


uint8_t svsDirectSWrap(varRetVal *result, argStruct *argS, svsVM *s);

static uint16_t touchX;
static uint16_t touchY;
static gr2EventType touchEv;

void sx_set_touch_ev(gr2EventType ev, uint16_t x, uint16_t y) {
  touchX = x;
  touchY = y;
  touchEv = ev;
}

gr2EventType sx_get_touch_ev() {
  return touchEv;
}

uint16_t sx_get_touch_x() {
  return touchX;
}

uint16_t sx_get_touch_y() {
  return touchY;
}

void svsDirectSWrapInit() {
  addSysWrapper(svsDirectSWrap, (uint8_t *)"ds");
}

uint8_t svsDirectSWrap(varRetVal *result, argStruct *argS, svsVM *s){
  uint8_t argType[11];
  static int16_t x1;
  static int16_t y1;
  static int16_t x2;
  static int16_t y2;

  //#!Automatically generated documentation on wrap_directS.c
  //#!
  //#!### Direct Screen draw functions
  //#!
  //#!####  Color & Areas
  //#!

  //#!##### Get color from RGB
  //#!    sys.ds.mixColor([num]r, [num]g, [num]b);
  //#!Mixes the right color from red, green and blue values (0 - 255)
  //#!
  //#!Return: [num] Color (16bit RGB565)
  if (sysFuncMatch(argS->callId, "mixColor", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }

    result->value.val_u
      = LCD_MixColor(argS->arg[1].val_u, argS->arg[2].val_u, argS->arg[3].val_u);

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set draw area
  //#!    sys.ds.setArea([num] x1, [num] y1, [num] x2, [num] y2);
  //#!Sets the draw area. Uses hardware coordinates.
  //#!For example: sys.ds.setArea(0, 32, 319, 479);
  //#!This will init all app available space as a draw area.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setArea", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 4, s)) {
      return 0;
    }

    x1 = argS->arg[1].val_s;
    y1 = argS->arg[2].val_s;
    x2 = argS->arg[3].val_s;
    y2 = argS->arg[4].val_s;

    if (sda_os_overlay_visible()){
      return 1;
    }

    LCD_setDrawArea(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s
    );

    return 1;
  }

  //#!##### Draw rectangle
  //#!    sys.ds.drawRect([num] x1, [num] y1, [num] x2, [num] y2, [num]col);
  //#!Draws rectangle outline
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "drawRect", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;
    argType[5] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()){
      return 1;
    }

    IRQ_BLOCK
    LCD_DrawRectangle(
      x1 + argS->arg[1].val_s,
      y1 + argS->arg[2].val_s,
      x1 + argS->arg[3].val_s,
      y1 + argS->arg[4].val_s,
      argS->arg[5].val_u
    );
    IRQ_ENABLE
    return 1;
  }

  //#!##### Draw filled rectangle
  //#!    sys.ds.fillRect([num] x1, [num] y1, [num] x2, [num] y2, [num]col);
  //#!Draws filled rectangle
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "fillRect", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;
    argType[5] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()){
      return 1;
    }

    IRQ_BLOCK
    LCD_FillRect(
      x1 + argS->arg[1].val_s,
      y1 + argS->arg[2].val_s,
      x1 + argS->arg[3].val_s,
      y1 + argS->arg[4].val_s,
      argS->arg[5].val_u
    );
    IRQ_ENABLE
    return 1;
  }

  //#!##### Draw circle
  //#!    sys.ds.drawCircle([num] x1, [num] y1, [num] radius, [num]col);
  //#!Draws circle
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "drawCircle", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 4, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()) {
      return 1;
    }

    IRQ_BLOCK
    LCD_DrawCircle(
      x1 + argS->arg[1].val_s,
      y1 + argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_u
    );
    IRQ_ENABLE
    return 1;
  }

  //#!##### Draw filled circle
  //#!    sys.ds.fillCircle([num] x1, [num] y1, [num] radius, [num]col);
  //#!Fills circle
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "fillCircle", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 4, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()) {
      return 1;
    }

    IRQ_BLOCK
    LCD_FillCircle(
        x1 + argS->arg[1].val_s,
        y1 + argS->arg[2].val_s,
        argS->arg[3].val_s,
        argS->arg[4].val_u
    );
    IRQ_ENABLE
    return 1;
  }

  
  //#!##### Draw portion of a circle
  //#!    sys.ds.drawCPart([num] x1, [num] y1, [num] radius, [num] part, [num]col);
  //#!Draws part of a circle
  //#!
  //#!Parts:
  //#!| 0 | 1 |
  //#!| 2 | 3 |
  //#!Return: None
  if (sysFuncMatch(argS->callId, "drawCPart", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;
    argType[5] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()) {
      return 1;
    }

    IRQ_BLOCK
    LCD_DrawCirclePart(
        x1 + argS->arg[1].val_s,
        y1 + argS->arg[2].val_s,
        argS->arg[3].val_s,
        argS->arg[4].val_s,
        argS->arg[5].val_u
    );
    IRQ_ENABLE
    return 1;
  }


  //#!##### Fill portion of a circle
  //#!    sys.ds.fillCPart([num] x1, [num] y1, [num] radius, [num] part, [num]col);
  //#!Fills a part of a circle
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "fillCPart", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;
    argType[5] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()) {
      return 1;
    }

    IRQ_BLOCK
    LCD_FillCirclePart(
        x1 + argS->arg[1].val_s,
        y1 + argS->arg[2].val_s,
        argS->arg[3].val_s,
        argS->arg[4].val_s,
        argS->arg[5].val_u
    );
    IRQ_ENABLE
    return 1;
  }

  //#!##### Draw line
  //#!    sys.ds.drawLine([num] x1, [num] y1, [num] x2, [num] y2, [num]col);
  //#!Draws line
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "drawLine", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;
    argType[5] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()) {
      return 1;
    }

    IRQ_BLOCK
    LCD_DrawLine(
      x1 + argS->arg[1].val_s,
      y1 + argS->arg[2].val_s,
      x1 + argS->arg[3].val_s,
      y1 + argS->arg[4].val_s,
      argS->arg[5].val_u
    );
    IRQ_ENABLE
    return 1;
  }

  //#!##### Draw text
  //#!    sys.ds.drawText([num] x1, [num] y1, [str] txt, [num]col);
  //#!Draws text
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "drawText", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_STR;
    argType[4] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 4, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()) {
      return 1;
    }

    IRQ_BLOCK
    LCD_DrawText_ext(
      x1 + argS->arg[1].val_s,
      y1 + argS->arg[2].val_s,
      argS->arg[4].val_u,
      s->stringField + argS->arg[3].val_str
    );
    IRQ_ENABLE
    return 1;
  }

  //#!##### Set text to fit specified width
  //#!    sys.ds.setTextFit([num] enable, [num] width);
  //#!Sets max width for next drawn text.
  //#!(Redraw of UI elements might reset it)
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTextFit", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    LCD_set_fitText(argS->arg[1].val_s, argS->arg[2].val_s);
    return 1;
  }

  //#!##### Get text width
  //#!    sys.ds.getTextWidth([str] txt);
  //#!Gets width of a string, when drawn with current font.
  //#!
  //#!Return: [num] width (px)
  if (sysFuncMatch(argS->callId, "getTextWidth", s)) {
    argType[1] = SVS_TYPE_STR;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s  = (int32_t) LCD_Text_Get_Width(s->stringField + argS->arg[1].val_str, 0);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get text height
  //#!    sys.ds.getTextHeight([str] txt);
  //#!Gets height of a string, when drawn with current font.
  //#!
  //#!Return: [num] height (px)
  if (sysFuncMatch(argS->callId, "getTextHeight", s)) {
    argType[1] = SVS_TYPE_STR;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s  = (int32_t) LCD_Text_Get_Height(s->stringField + argS->arg[1].val_str, 0);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Fill area with color
  //#!    sys.ds.clearArea([num]col);
  //#!Clears draw area with given color
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "clearArea", s)) {
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()) {
      return 1;
    }

    IRQ_BLOCK
    LCD_FillRect(x1, y1, x2, y2, argS->arg[1].val_u);
    IRQ_ENABLE
    return 1;
  }

  //#!#### P16 image tools

  //#!##### Draws P16 image
  //#!    sys.ds.drawImage([num]x, [num]y, [num]scale_w, [num]scale_h, [str]name);
  //#!Draws p16 image from the working directory. Supports upscaling, and downscaling.
  //#!
  //#!Scale table:
  //#!
  //#!|  Scale value | Image size |
  //#!|      ---     |   ---      |
  //#!|    -3        |   1/16     |
  //#!|    -2        |   1/8      |
  //#!|    -1        |   1/4      |
  //#!|     0        |   1/2      |
  //#!|     1        |    1       |
  //#!|     2        |    2x      |
  //#!|     n        |    n*x     |
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "drawImage", s)) {
    argType[1] = SVS_TYPE_NUM; //x
    argType[2] = SVS_TYPE_NUM; //y
    argType[3] = SVS_TYPE_NUM; //width
    argType[4] = SVS_TYPE_NUM; //height
    argType[5] = SVS_TYPE_STR; //fname
    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()) {
      return 1;
    }

#ifdef PPM_SUPPORT_ENABLED
    IRQ_BLOCK

    sda_draw_p16_scaled(
      x1 + (int16_t)argS->arg[1].val_s,
      y1 + (int16_t) argS->arg[2].val_s,
      (int16_t) argS->arg[3].val_s,
      (int16_t) argS->arg[4].val_s,
      s->stringField + argS->arg[5].val_str
    );
    IRQ_ENABLE
#endif
    return 1;
  }

  //#!##### Get P16 image width
  //#!    sys.ds.getImageW([str]name);
  //#!Gets width of given p16 file.
  //#!
  //#!Return: [num] width (px)
  if (sysFuncMatch(argS->callId, "getImageW", s)) {
    argType[1] = SVS_TYPE_STR;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s  = sda_p16_get_width(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get P16 image height
  //#!    sys.ds.getImageH([str]name);
  //#!Gets height of given p16 file.
  //#!
  //#!Return: [num] height (px)
  if (sysFuncMatch(argS->callId, "getImageH", s)) {
    argType[1] = SVS_TYPE_STR;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s  = sda_p16_get_height(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Draws PPM
  //#!    sys.ds.drawPPM([num]x, [num]y, [num]scale, [str]name);
  //#!Draws ppm image (To be removed).
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "drawPPM", s)) {
    argType[1] = SVS_TYPE_NUM; //x
    argType[2] = SVS_TYPE_NUM; //y
    argType[3] = SVS_TYPE_NUM; //scale
    argType[4] = SVS_TYPE_STR; //fname
    if(sysExecTypeCheck(argS, argType, 4, s)) {
      return 0;
    }

    if (sda_os_overlay_visible()) {
      return 1;
    }

#ifdef PPM_SUPPORT_ENABLED
    IRQ_BLOCK
    draw_ppm(argS->arg[1].val_s, argS->arg[2].val_s, argS->arg[3].val_s, s->stringField + argS->arg[4].val_str);
    IRQ_ENABLE
#endif
    return 1;
  }

  //#!#### Touch API
  //#!##### Get if screen is touched
  //#!    sys.ds.touchEv();
  //#!Gets if screen is touched, returns last touch event
  //#!
  //#!Return: [num] Touch event (from event defines)
  if (sysFuncMatch(argS->callId, "touchEv", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (touchEv == EV_PRESSED) {
      result->value.val_s  = 1;
    } else if (touchEv == EV_RELEASED) {
      result->value.val_s  = 3;
    } else if (touchEv == EV_HOLD) {
      result->value.val_s  = 2;
    } else {
      result->value.val_s  = 0;
    }

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Clears touch event
  //#!    sys.ds.touchClr();
  //#!Clears touch event
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "touchClr", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    touchEv = EV_NONE;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get touch y
  //#!    sys.ds.touchY();
  //#!Gives y coordinate of touch event
  //#!
  //#!Return: [num] Touch y coordinate
  if (sysFuncMatch(argS->callId, "touchY", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s  = touchY;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get touch x
  //#!    sys.ds.touchX();
  //#!Gives x coordinate of touch event
  //#!
  //#!Return: [num] Touch y coordinate
  if (sysFuncMatch(argS->callId, "touchX", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s  = touchX;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}

