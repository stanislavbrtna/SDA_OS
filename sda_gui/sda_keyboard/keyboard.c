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
#include "sda_keyboard.h"

#define ICON_BKSP 1

static uint8_t spec_icon;

void pm_keyboard_draw_key(
  uint16_t x1,
  uint16_t y1,
  uint16_t x2,
  uint16_t y2,
  uint8_t *str,
  uint8_t active,
  uint8_t size
) {
  if (active == 1) {
    LCD_FillRect(x1, y1, x2, y2, sda_current_con->active_color);
  }

  LCD_DrawRectangle(x1, y1, x2, y2, sda_current_con->border_color);
  if (size == 1) {
    LCD_DrawText(x1 + 5, y1 + 8, sda_current_con->text_color, 0, str, 1);
  } else {
    LCD_DrawText_ext(x1 + 8, y1 + 8, sda_current_con->text_color, str);
  }
}

static void draw_esc_icon(int16_t x, int16_t y) {
  LCD_DrawLine(x + 8, y + 32 - 8 , x + 32 - 8, y + 8, sda_current_con->text_color);

  LCD_DrawLine(x + 4, y + 32 - 16 , x + 4, y + 32 - 4 , sda_current_con->text_color);
  LCD_DrawLine(x + 4, y + 32 - 4 , x + 16, y + 32 - 4, sda_current_con->text_color);

  LCD_DrawLine(x + 8, y + 32 - 8 , x + 4, y + 32 - 16 , sda_current_con->text_color);
  LCD_DrawLine(x + 8, y + 32 - 8 , x + 16, y + 32 - 4, sda_current_con->text_color);

}

static void draw_backspace_icon(int16_t x, int16_t y) {
  LCD_FillRect(x + 15, y + 8 , x + 40, y + 9, sda_current_con->text_color);
  LCD_FillRect(x + 15, y + 25, x + 40, y + 26, sda_current_con->text_color);
  LCD_FillRect(x + 39, y + 8 , x + 40, y + 26, sda_current_con->text_color);

  LCD_DrawLine(x + 5, y + 17 , x + 14, y + 8, sda_current_con->text_color);
  LCD_DrawLine(x + 6, y + 17 , x + 15, y + 8, sda_current_con->text_color);
  LCD_DrawLine(x + 7, y + 17 , x + 16, y + 8, sda_current_con->text_color);

  LCD_DrawLine(x + 5, y + 17 , x + 14, y + 26, sda_current_con->text_color);
  LCD_DrawLine(x + 6, y + 17 , x + 15, y + 26, sda_current_con->text_color);
  LCD_DrawLine(x + 7, y + 17 , x + 16, y + 26, sda_current_con->text_color);

  int16_t xmod = 3;

  LCD_DrawLine(x + 24 - xmod, y + 13 , x + 32 - xmod, y + 21, sda_current_con->text_color);
  LCD_DrawLine(x + 25 - xmod, y + 13 , x + 33 - xmod, y + 21, sda_current_con->text_color);

  LCD_DrawLine(x + 24 - xmod, y + 21 , x + 32 - xmod, y + 13, sda_current_con->text_color);
  LCD_DrawLine(x + 25 - xmod, y + 21 , x + 33 - xmod, y + 13, sda_current_con->text_color);
}

void svp_draw_keyboard(uint16_t x, uint16_t y, psvcKbdLayout *layout) {
  uint16_t i, line = 0;
  uint8_t curr_font;
  LCD_setDrawArea(x, y, x + 320, y + 5 * 32);
  LCD_FillRect(x, y, x + 320, y + 5 * 32, sda_current_con->fill_color);

  curr_font = LCD_Get_Font_Size();
  LCD_Set_Sys_Font(18);


  i = 0;
  uint8_t siz = 2;
  if (layout->keyStr[i][1] != 0) {
    siz = 1;
  }

  pm_keyboard_draw_key(
    x + 32 * i,
    y + line * 32,
    x + 32 * (i + 1),
    y + (line + 1) * 32,
    layout->keyStr[i],
    0,
    siz
  );

  for(i = 1; i < 10; i++) {
    pm_keyboard_draw_key(
      x + 32 * i,
      y + line * 32,
      x + 32 * (i + 1),
      y + (line + 1) * 32,
      layout->keyStr[i],
      0,
      2
    );
  }
  line++;

  for(i = 0; i < 10; i++) {
    pm_keyboard_draw_key(
      x + 32 * i,
      y + line * 32,
      x + 32 * (i + 1),
      y + (line + 1) * 32,
      layout->keyStr[i + 10],
      0,
      2
    );
  }
  line++;

  for(i = 0; i < 9; i++) {
    pm_keyboard_draw_key(
      x + 32 * (i),
      y + line * 32,
      x + 32 * (i + 1),
      y + (line + 1) * 32,
      layout->keyStr[i+20],
      0,
      2
    );
  }
  i++;

  pm_keyboard_draw_key(
    x + 32 * (i),
    y + line * 32,
    x + 32 * (i + 1),
    y + (line + 1) * 32,
    layout->keyStr[36],
    0,
    1
  );

  line++;
  for(i = 0; i < 7; i++) {
    pm_keyboard_draw_key(
      x + 16 + 32 * (i),
      y + line * 32,
      16 + x + 32 * (i + 1),
      y + (line + 1) * 32,
      layout->keyStr[i + 29],
      0,
      2
    );
  }
  i++;
  pm_keyboard_draw_key(x + 16 + 32*i,
    y + line * 32,
    16 + x + 32*(i + 2),
    y + (line + 1) * 32,
    (uint8_t *)"", // <=
    0,
    2
  );
  draw_backspace_icon(x + 16 + 32*i, y + line * 32);
  line++;

  i = 0;
  pm_keyboard_draw_key(
    x + 32 * (i),
    y + line * 32,
    x + 32 * (i + 1),
    y + (line + 1) * 32,
    (uint8_t *) "",
    0,
    1
  );
  draw_esc_icon(x + 32 * (i), y + line * 32);

  i++;
  pm_keyboard_draw_key(
    x + 32 * i,
    y + line * 32,
    x + 32 * (i + 1),
    y + (line + 1) * 32,
    layout->keyStr[38],
    0,
    1
  );
  i++;
  pm_keyboard_draw_key(
    x + 32 * i,
    y + line * 32,
    x + 32 * (i + 4),
    y + (line + 1) * 32,
    (uint8_t *)"SpaceBar",
    0,
    1
  );
  i += 4;
  pm_keyboard_draw_key(
    x + 32 * i,
    y + line * 32,
    x + 32 * (i + 2),
    y + (line + 1) * 32,
    (uint8_t *)"Shift",
    0,
    1
  );
  i += 2;
  pm_keyboard_draw_key(
    x + 32 * (i),
    y + line * 32,
    x + 32 * (i + 2),
    y + (line + 1) * 32,
    (uint8_t *)"Enter",
    0,
    1
  );
  i++;

  LCD_Set_Sys_Font(curr_font);
}

uint8_t pm_keyboard_touch_key(
  gr2EventType event,
  uint8_t *prevState,
  uint16_t x1,
  uint16_t y1,
  uint16_t x2,
  uint16_t y2,
  uint8_t *str,
  uint8_t size,
  uint16_t touch_x,
  uint16_t touch_y
) {
  uint8_t curr_font;
  static uint8_t repeatCnt;
  static uint8_t repeatQuickCnt;
  curr_font = LCD_Get_Font_Size();
  LCD_Set_Sys_Font(18);
  if((x1 < touch_x) && (x2 > touch_x) && (y1 < touch_y) && (y2 > touch_y)) {
    if ((event == EV_HOLD) && (*prevState == 0)) {
      LCD_FillRect(x1, y1, x2, y2, sda_current_con->active_color);
      *prevState = 1;
      LCD_DrawRectangle(x1, y1, x2, y2, sda_current_con->border_color);
      repeatCnt = 0;
      repeatQuickCnt = 0;
      if (spec_icon == 0) {
        if (size == 1) {
          LCD_DrawText(x1 + 5, y1 + 8, sda_current_con->text_color, 0, str, 1);
        } else {
          LCD_DrawText_ext(x1 + 8, y1 + 8, sda_current_con->text_color, str);
        }
      } else {
        draw_backspace_icon(x1, y1);
      }
    }
    if ((event == EV_HOLD) && (*prevState == 1)) {
      repeatCnt++;
      if ((repeatCnt > 10) || ((repeatCnt > 3) && (repeatQuickCnt > 2))) {
        LCD_Set_Sys_Font(curr_font);
        repeatCnt = 0;
        if (repeatQuickCnt<=2){
          repeatQuickCnt++;
        }
        return 1;
      }
    }
    if (event == EV_RELEASED) {
      LCD_FillRect(x1, y1, x2, y2, sda_current_con->fill_color);
      LCD_DrawRectangle(x1, y1, x2, y2, sda_current_con->border_color);
      if (spec_icon == 0) {
        if (size == 1) {
          LCD_DrawText(x1 + 5, y1 + 8, sda_current_con->text_color, 0, str, 1);
        } else {
          LCD_DrawText_ext(x1 + 8, y1 + 8, sda_current_con->text_color, str);
        }
      } else {
        draw_backspace_icon(x1, y1);
      }
      *prevState = 0;
      repeatCnt = 0;
      repeatQuickCnt = 0;
      LCD_Set_Sys_Font(curr_font);
      return 1;
    }
  } else {
    if (*prevState == 1) {
      LCD_FillRect(x1, y1, x2, y2, sda_current_con->fill_color);
      LCD_DrawRectangle(x1, y1, x2, y2, sda_current_con->border_color);
      if (spec_icon == 0) {
        if (size == 1) {
          LCD_DrawText(x1 + 5, y1 + 8, sda_current_con->text_color, 0, str, 1);
        } else {
          LCD_DrawText_ext(x1 + 8, y1 + 8, sda_current_con->text_color, str);
        }
      } else {
        draw_backspace_icon(x1, y1);
      }
      *prevState = 0;
      repeatCnt = 0;
      repeatQuickCnt = 0;
    }
  }

  LCD_Set_Sys_Font(curr_font);
  return 0;
}


void retStrCpy(uint8_t * str, uint8_t * str2) {
  uint16_t i;
  for (i = 0; (i < 10) && (str2[i] != 0); i++) {
    str[i] = str2[i];
  }
  str[i] = 0;
}


uint8_t svp_touch_keyboard(
  uint16_t x,
  uint16_t y,
  psvcKbdLayout *layout,
  uint16_t touch_x,
  uint16_t touch_y,
  uint8_t* retStr,
  gr2EventType event
) {
  uint16_t i, line = 0;
  static uint8_t prevState[50];

  LCD_setDrawArea(x, y, x + 320, y + 5 * 32);

  if(event == EV_PRESSED) {
    for (i = 0; i < 50; i++){
      prevState[i] = 0;
    }
  }

  i = 0;
  uint8_t siz = 2;
  if (layout->keyStr[i][1] != 0) {
    siz = 1;
  }
  if (pm_keyboard_touch_key(event,&prevState[i],x+32*(i),y+line*32,x+32*(i+1),y+(line+1)*32,layout->keyStr[i], siz, touch_x, touch_y)) {
    retStrCpy(retStr, layout->key[i]);
    return 1;
  }

  for(i = 1; i < 10; i++) {
    if (pm_keyboard_touch_key(event,&prevState[i],x+32*(i),y+line*32,x+32*(i+1),y+(line+1)*32,layout->keyStr[i], 2, touch_x, touch_y)) {
      retStrCpy(retStr, layout->key[i]);
      //printf("dbg: %s\n", retStr) ;
      return 1;
    }
  }
  line++;
  for(i=0;i<10;i++){
    if (pm_keyboard_touch_key(event,&prevState[i+10],x+32*(i),y+line*32,x+32*(i+1),y+(line+1)*32,layout->keyStr[i+10],2, touch_x, touch_y)){
      retStrCpy(retStr, layout->key[i+10]);
      return 1;
    }
  }
  line++;
  for(i=0;i<9;i++){
    if(pm_keyboard_touch_key(event,&prevState[i+20],x+32*(i),y+line*32,x+32*(i+1),y+(line+1)*32,layout->keyStr[i+20],2, touch_x, touch_y)){
      retStrCpy(retStr, layout->key[i+20]);
      return 1;
    }
  }
  i++;
  if (pm_keyboard_touch_key(event,&prevState[36],x+32*(i),y+line*32,x+32*(i+1),y+(line+1)*32,layout->keyStr[36],1, touch_x, touch_y)){
    retStrCpy(retStr, layout->key[36]); //prog nechodí
    return 1;
  }

  line++;
  for(i=0;i<7;i++){
    if(pm_keyboard_touch_key(event,&prevState[i+29],x+16+32*(i),y+line*32,16+x+32*(i+1),y+(line+1)*32,layout->keyStr[i+29], 2, touch_x, touch_y)){
      retStrCpy(retStr, layout->key[i+29]);
      return 1;
    }
  }
  i++;
  spec_icon = ICON_BKSP;
  if(pm_keyboard_touch_key(event,&prevState[40],x+16+32*(i),y+line*32,x+32*(i+2),y+(line+1)*32,(uint8_t *)"",2, touch_x, touch_y)) {
    spec_icon = 0;
    retStrCpy(retStr, (uint8_t *)"\x08\x00");
    return 1;
  }
  spec_icon = 0;
  line++;
  i=0;
  if(pm_keyboard_touch_key(event,&prevState[37],x+32*(i),y+line*32,x+32*(i+1),y+(line+1)*32,(uint8_t *)"",1, touch_x, touch_y)){
    //retStr=layout->key[37];
    retStrCpy(retStr, layout->key[37]); //esc
    return 2;
  }
  i++;
  if(pm_keyboard_touch_key(event,&prevState[38],x+32*(i),y+line*32,x+32*(i+1),y+(line+1)*32,layout->keyStr[38],1, touch_x, touch_y)){
    retStrCpy(retStr, layout->key[38]);
    return 1; //spec
  }
  i++;
  if(pm_keyboard_touch_key(event,&prevState[41],x+32*(i),y+line*32,x+32*(i+4),y+(line+1)*32, (uint8_t *)"SpaceBar", 1, touch_x, touch_y)){
    retStrCpy(retStr, (uint8_t *)" ");
    return 1;
  }
  i+=4;
  if(pm_keyboard_touch_key(event,&prevState[42],x+32*(i),y+line*32,x+32*(i+2),y+(line+1)*32, (uint8_t *)"Shift", 1, touch_x, touch_y)){
    retStrCpy(retStr, layout->key[39]);
    return 1;
  }
  i+=2;
  if(pm_keyboard_touch_key(event,&prevState[43],x+32*(i),y+line*32,x+32*(i+2),y+(line+1)*32, (uint8_t *)"Enter", 1, touch_x, touch_y)){
    retStrCpy(retStr, (uint8_t *)"\n");
    return 1;
  }
  i++;
  return 0;
}


uint8_t sda_keyboard_set_layout(uint8_t layoutId, psvcKbdLayout *layout) {
  if (layoutId == 1) {
    init_kblayout_uppcase(layout);
  } else if (layoutId == 2) {
    init_kblayout_special(layout);
  } else if (layoutId == 3) {
    init_kblayout_special_shift(layout);
  } else if (layoutId == 4) {
    init_kblayout_numeric(layout);
  } else {
    // default
    init_kblayout_standard(layout);
  }
  return 0;
}
