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

#include "sda_svm_gr2_glue.h"

int32_t get_real_cursor_pos(uint16_t cpos_u8, uint8_t *str) {
  uint16_t len = 0;
  uint16_t x = 0;

  while (str[x] != 0) {
    if(len == cpos_u8) {
      return x;
    }

    if ((str[x] >= 0xC3) \
        && (str[x] <= 0xC5)) {
      x++;
    }
    len++;
    x++;
  }
  return x;
}


int32_t get_char_cursor_pos(uint16_t cpos, uint8_t *str) {
  uint16_t len = 0;
  uint16_t x = 0;

  while (str[x] != 0) {
    if(x == cpos) {
      return len;
    }

    if ((str[x] >= 0xC3) \
        && (str[x] <= 0xC5)) {
      x++;
    }
    len++;
    x++;   
  }
  return len;
}


uint8_t svm_text_handler(varRetVal *result, argStruct *argS, svsVM *s) {
  uint32_t x = 0;

  // set default return
  result->value = argS->arg[2];
  result->type = SVS_TYPE_STR;

  // text not active
  if (!gr2_get_value(argS->arg[1].val_s, &sda_app_con)) {
    return 0;
  }

  if(gr2_cursor_handler(argS->arg[1].val_s, svpSGlobal.uptimeMs, &sda_app_con)) {
    sda_clipboard_overlay_init(argS->arg[1].val_s);
    return 0;
  }

  if (svpSGlobal.newStringIdFlag == argS->arg[1].val_s) {
    result->value.val_str = svpSGlobal.newString;
    svpSGlobal.newStringIdFlag = 0;
    result->type = SVS_TYPE_STR;
    return 1;
  }

  // keyboard read and svs string modification
  if (sda_get_keyboard_key_flag()) {

    if(sda_keyboard_driver_get_ctrl()) {
      if(svpSGlobal.kbdKeyStr[0] == 'v') {
        sda_paste_gr2_from_clipboard(argS->arg[1].val_s, sda_current_con);
        result->value.val_str = svpSGlobal.newString;
        svpSGlobal.newStringIdFlag = 0;
        return 1;
      }

      if(svpSGlobal.kbdKeyStr[0] == 'a') {
        sda_app_con.textBlockStart = 1;
        sda_app_con.textBlockEnd = sda_strlen(gr2_get_str(argS->arg[1].val_s, sda_current_con));
        gr2_set_modified(argS->arg[1].val_s, sda_current_con);
        return 1;
      }
    }
    
    // block rewrite handling
    if (sda_app_con.textBlockStart != 0 || sda_app_con.textBlockEnd != 0) {
      if(sda_keyboard_driver_get_ctrl()) {
        if(svpSGlobal.kbdKeyStr[0] == 'c') {
          sda_copy_gr2_to_clipboard(argS->arg[1].val_s, sda_current_con);
          return 1;
        }

        if(svpSGlobal.kbdKeyStr[0] == 'x') {
          sda_cut_gr2_to_clipboard(argS->arg[1].val_s, sda_current_con);
          printf("str: %s\n", gr2_get_str(argS->arg[1].val_s, sda_current_con));
          result->value.val_str = svpSGlobal.newString;
          svpSGlobal.newStringIdFlag = 0;
          return 1;
        }
      } 

      strNewStreamInit(s);
      uint32_t i = 0;
      uint8_t * str = gr2_get_str((uint16_t)argS->arg[1].val_str, sda_current_con);
      
      while(str[i] != 0) {
        if (!(i >= (sda_current_con->textBlockStart - 1) && i < sda_current_con->textBlockEnd)) {
          strNewStreamPush(str[i], s);
        }
        i++;
      }
      gr2_set_param((uint16_t)argS->arg[1].val_str, sda_current_con->textBlockStart - 1, sda_current_con);

      argS->arg[2].val_str = strNewStreamEnd(s);
      sda_app_con.textBlockStart = 0;
      sda_app_con.textBlockEnd = 0;

      // backspace/delete
      if (svpSGlobal.kbdKeyStr[0] == 8) {
          result->value = argS->arg[2];
          result->type = SVS_TYPE_STR;
          return 1; 
      }
      
    }

    if ((svpSGlobal.kbdKeyStr[0]) != 8) { // normal chars
      result->value.val_str
        = strInsert(
                    (uint16_t)argS->arg[2].val_str,
                    strNew(svpSGlobal.kbdKeyStr, s),
                    gr2_get_param(argS->arg[1].val_s, &sda_app_con),
                    s
          ); // add char/string
      

      gr2_set_param(
        argS->arg[1].val_s,
        gr2_get_param(argS->arg[1].val_s, &sda_app_con) + sda_strlen(svpSGlobal.kbdKeyStr),
        &sda_app_con
      ); // move cursor

      result->type = SVS_TYPE_STR;
      return 1;
    } else { // delete/backspace
      uint16_t len = 0;
      uint16_t prac;
      uint16_t charIndex;
      uint8_t czFlag = 0;

      while(*(s->stringField + argS->arg[2].val_str + len) != 0) {
        len++;
      }

      result->value.val_str = strNew(s->stringField + argS->arg[2].val_str, s); // create new string

      if ((svpSGlobal.kbdKeyStr[1]) != 'd') { // backspace
        if (len > 0 && gr2_get_param(argS->arg[1].val_s, &sda_app_con) != 0) {
          charIndex = result->value.val_str + gr2_get_param(argS->arg[1].val_s, &sda_app_con);

          if (len >= 2
              && (s->stringField[charIndex - 2] >= 0xC3)
              && (s->stringField[charIndex - 2] <= 0xC5))
          {
            gr2_set_param(argS->arg[1].val_s, gr2_get_param(argS->arg[1].val_s, &sda_app_con) - 2, &sda_app_con);
            czFlag = 1;
          } else {
            gr2_set_param(argS->arg[1].val_s, gr2_get_param(argS->arg[1].val_s, &sda_app_con) - 1, &sda_app_con);
          }

        } else {
          result->value = argS->arg[2];
          result->type = SVS_TYPE_STR;
          return 1;
        }
      } else { // delete
        if (len > 0 && len != gr2_get_param(argS->arg[1].val_s, &sda_app_con)) {
          charIndex = result->value.val_str + gr2_get_param(argS->arg[1].val_s, &sda_app_con);

          if (len >= 2
              && (s->stringField[charIndex] >= 0xC3)
              && (s->stringField[charIndex] <= 0xC5))
          {
            czFlag = 1;
          }

        } else {
          result->value = argS->arg[2];
          result->type = SVS_TYPE_STR;
          return 1;
        }
      }

      prac = gr2_get_param(argS->arg[1].val_s, &sda_app_con);
      x = 0;

      while(s->stringField[result->value.val_str + prac + x] != 0) {
        s->stringField[result->value.val_str + prac + x]
          = s->stringField[result->value.val_str + prac + x + 1 + czFlag];
        x++;
      }

      *((uint8_t*)(s->stringField + result->value.val_str + len - 1)) = 0; //zkrátíme pro jistotu
      gr2_set_str(argS->arg[1].val_s,s->stringField + result->value.val_str, &sda_app_con); //nastavíme text/updatujeme
      result->type = SVS_TYPE_STR;
      return 1;
    }
  }

  return 0;
}
