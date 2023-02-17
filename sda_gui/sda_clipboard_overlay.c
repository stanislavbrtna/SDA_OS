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


#include "sda_system_overlays.h"

static uint16_t bCopy;
static uint16_t bCut;
static uint16_t bPaste;
static uint16_t bClose;
static uint16_t scr, ov_id;
static uint16_t target_id;

extern svsVM    svm;


uint16_t sda_clipboard_overlay_init(uint16_t id) {
  scr = gr2_add_screen(sda_current_con);

  uint8_t rel = gr2_get_relative_init(sda_current_con);

  gr2_set_relative_init(1, sda_current_con);

  //gr2_add_text(1, 0, 8, 1, "Clipboard", s, sda_current_con);

  bCopy = gr2_add_button(0, 0, 3, 1, "Copy", scr, sda_current_con);
  bCut = gr2_add_button(3, 0, 2, 1, "Cut", scr, sda_current_con);
  bPaste = gr2_add_button(5, 0, 3, 1, "Paste", scr, sda_current_con);
  bClose = gr2_add_button(8, 0, 1, 1, "X", scr, sda_current_con);

  ov_id = setOverlayScreen(scr, sda_current_con);

  target_id = id;

  gr2_set_relative_init(rel, sda_current_con);

  setOverlayPos(16, 262, 320 - 32, 32);

  return 0;
}

void copy_to_clipboard(uint16_t id, gr2context *c) {
  uint32_t i = 0;
  uint32_t b = 0;
  uint8_t * str = gr2_get_str(id, c);
  while(str[i] != 0) {
    if (i >= c->textBlockStart && i < c->textBlockEnd) {
      svpSGlobal.clipboard[b] = str[i];
      b++;
      if (b > 254) {
        printf("Error: String too big to fit in clipboard!\n");
        break;
      }
    }
    i++;
  }
  svpSGlobal.clipboard[b] = 0;   
}

uint16_t sda_clipboard_overlay_update() {
  
  if (ov_id != getOverlayId() || getOverlayId() == 0)  {
    return;
  }

  if (gr2_clicked(bCopy, sda_current_con)) {
    copy_to_clipboard(target_id, sda_current_con);
    destroyOverlay();
    setRedrawFlag();
    return 0;
  }

  if (gr2_clicked(bCut, sda_current_con)) {
    copy_to_clipboard(target_id, sda_current_con);
    svpSGlobal.newStringIdFlag = target_id;
    strNewStreamInit(&svm);
    uint32_t i = 0;
    uint32_t b = 0;
    uint8_t * str = gr2_get_str(target_id, sda_current_con);
    
    while(str[i] != 0) {
      if (!(i >= sda_current_con->textBlockStart && i < sda_current_con->textBlockEnd)) {
        strNewStreamPush(str[i], &svm);
      }
      i++;
    }

    gr2_set_param(target_id, sda_current_con->textBlockStart, sda_current_con);
    sda_current_con->textBlockStart = 0;
    sda_current_con->textBlockEnd = 0;

    svpSGlobal.newString = strNewStreamEnd(&svm);
     
    destroyOverlay();
    setRedrawFlag();
    return 0;
  }

  if (gr2_clicked(bPaste, sda_current_con)) {
    svpSGlobal.newStringIdFlag = target_id;
    svpSGlobal.newString = strInsert(
      (uint16_t)((uint32_t) gr2_get_str(target_id, sda_current_con) - (uint32_t) svm.stringField),
      strNew(svpSGlobal.clipboard, &svm),
      gr2_get_param(target_id, sda_current_con),
      &svm
    );

    destroyOverlay();
    setRedrawFlag();
    return 0;
  }

  if (gr2_clicked(bClose, sda_current_con)) {
    destroyOverlay();
    setRedrawFlag();
    return 0;
  }

}