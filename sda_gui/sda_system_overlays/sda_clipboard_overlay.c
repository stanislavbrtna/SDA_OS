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


#include "sda_clipboard_overlay.h"

static uint16_t bCopy;
static uint16_t bCut;
static uint16_t bPaste;
static uint16_t bClose;
static uint16_t scr, ov_id;
static uint16_t target_id;

extern svsVM    svm;
extern uint8_t  overlayRedrawFlag;

sdaOverlayType prevOv;
uint8_t restore;

void reset_clipboard_overlay();


uint16_t sda_clipboard_overlay_init(uint16_t id) {
  //printf("init called: target_id: %u, ov_id: %u\n", target_id, ov_id);
  target_id = id;
  return 0;
}

void init_screen() {
  scr = gr2_add_screen(sda_current_con);

  uint8_t rel = gr2_get_relative_init(sda_current_con);

  gr2_set_relative_init(1, sda_current_con);
  
#ifdef LANG_CZ
  bCopy = gr2_add_button(0, 0, 3, 1, OVRL_COPY_COPY, scr, sda_current_con);
  bCut = gr2_add_button(3, 0, 3, 1, OVRL_COPY_CUT, scr, sda_current_con);
  bPaste = gr2_add_button(6, 0, 2, 1, OVRL_COPY_PASTE, scr, sda_current_con);
#endif

#ifdef LANG_EN
  bCopy = gr2_add_button(0, 0, 3, 1, OVRL_COPY_COPY , scr, sda_current_con);
  bCut = gr2_add_button(3, 0, 2, 1, OVRL_COPY_CUT, scr, sda_current_con);
  bPaste = gr2_add_button(5, 0, 3, 1, OVRL_COPY_PASTE, scr, sda_current_con);
#endif

  bClose = gr2_add_button(8, 0, 1, 1, (uint8_t *)"X", scr, sda_current_con);

  gr2_set_rounded(bCut, 0, sda_current_con);
  gr2_set_rounded(bCopy, 0, sda_current_con);
  gr2_set_rounded(bPaste, 0, sda_current_con);
  gr2_set_rounded(bClose, 0, sda_current_con);

  gr2_text_set_align(bCopy, GR2_ALIGN_CENTER, sda_current_con);
  gr2_text_set_align(bCut, GR2_ALIGN_CENTER, sda_current_con);
  gr2_text_set_align(bPaste, GR2_ALIGN_CENTER, sda_current_con);
  gr2_text_set_align(bClose, GR2_ALIGN_CENTER, sda_current_con);

  if(svpSGlobal.clipboard[0] == 0) {
    gr2_set_grayout(bPaste, 1, sda_current_con);
  } else {
    gr2_set_grayout(bPaste, 0, sda_current_con);
  }

  if (getOverlayId() != 0) {
    overlayStore(&prevOv);
    restore = 1;
  } else {
    restore = 0;
  }

  ov_id = setOverlayScreen(scr, sda_current_con);

  setOverlayDestructor(reset_clipboard_overlay);

  gr2_set_relative_init(rel, sda_current_con);

  setOverlayPos(16, 262, 320 - 32, 32);
}

void restore_overlay() {
  if (restore) {
    overlayRestore(&prevOv);
  }
}

void sda_copy_gr2_to_clipboard(uint16_t id, gr2context *c) {
  uint32_t i = 0;
  uint32_t b = 0;
  uint8_t * str = gr2_get_str(id, c);

  if (c->textBlockStart == 0 || c->textBlockEnd == 0) {
    svpSGlobal.clipboard[0] = 0;
    return;
  }

  while(str[i] != 0) {
    if (i >= c->textBlockStart - 1 && i < c->textBlockEnd) {
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

void sda_cut_gr2_to_clipboard(uint16_t id, gr2context *c) {
  sda_copy_gr2_to_clipboard(id, c);
  svpSGlobal.newStringIdFlag = id;
  strNewStreamInit(&svm);
  uint32_t i = 0;
  uint8_t *str = gr2_get_str(id, c);
  
  while(str[i] != 0) {
    if (!(i >= (c->textBlockStart - 1) && i < c->textBlockEnd)) {
      strNewStreamPush(str[i], &svm);
    }
    i++;
  }

  gr2_set_param(id, c->textBlockStart - 1, c);
  c->textBlockStart = 0;
  c->textBlockEnd = 0;

  svpSGlobal.newString = strNewStreamEnd(&svm); 
}

void sda_paste_gr2_from_clipboard(uint16_t id, gr2context *c) {
  svpSGlobal.newStringIdFlag = id;
  svpSGlobal.newString = strInsert(
    (uint16_t)((uint32_t) gr2_get_str(id, c) - (uint32_t) svm.stringField),
    strNew(svpSGlobal.clipboard, &svm),
    gr2_get_param(id, c),
    &svm
  );
  gr2_set_param(id, gr2_get_param(id, c) + sda_strlen(svpSGlobal.clipboard), c);
}

void handle_clipboard_exit() {
  destroyOverlay();
  restore_overlay();
  target_id = 0;
  ov_id = 0;
  setRedrawFlag();
  overlayRedrawFlag = 1;
}


// exit without restoring overlay (X was pressed, S! was pressed...)
void reset_clipboard_overlay() {
  target_id = 0;
  ov_id = 0;
  gr2_destroy(scr, sda_current_con);
  overlayDestructorDone();
}


uint16_t sda_clipboard_overlay_update() {
  
  if ((ov_id != getOverlayId() || getOverlayId() == 0) && target_id == 0)  {
    return 0;
  }

  // this is here to init the overlay outside of the application
  if (target_id != 0 && ov_id == 0) {
    init_screen();
  }

  if (gr2_clicked(bCopy, sda_current_con)) {
    sda_copy_gr2_to_clipboard(target_id, sda_current_con);
    handle_clipboard_exit();
    return 0;
  }

  if (gr2_clicked(bCut, sda_current_con)) {
    sda_cut_gr2_to_clipboard(target_id, sda_current_con);
    handle_clipboard_exit();
    return 0;
  }

  if (gr2_clicked(bPaste, sda_current_con)) {
    sda_paste_gr2_from_clipboard(target_id, sda_current_con);
    handle_clipboard_exit();
    return 0;
  }

  if (gr2_clicked(bClose, sda_current_con)) {
    handle_clipboard_exit();
    return 0;
  }

  return 0;
}


uint8_t sda_clipboard_overlay_visible() {
  if ((ov_id != getOverlayId() || getOverlayId() == 0) && target_id == 0)  {
    return 0;
  }

  return 1;
}