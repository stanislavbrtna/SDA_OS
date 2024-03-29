/*
Copyright (c) 2022 Stanislav Brtna

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

#include "sda_os_wrapper.h"


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


uint8_t sda_os_gui_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint16_t x;
  uint8_t argType[11];

  //#!#### Gui

  //#!##### Set main application screen
  //#!    sys.os.gui.setMainScr([num]id);
  //#!Sets main screen to screen with given id
  //#!When you wish to display overlay only, set this to 0.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setMainScr", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }
     svmSetMainScreen((uint16_t) argS->arg[1].val_s );
    return 1;
  }

  //#!##### Get main application screen
  //#!    sys.os.gui.getMainScr();
  //#!Gets main screen id
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "getMainScr", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }
     result->value.val_u = svmGetMainScreen();
     result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Handle keypad input of a screen
  //#!    sys.os.gui.btnCtrl([num]screen_id, [num]back_btn_id);
  //#!Allows control of a given screen via buttons.
  //#!Element given as back_btn_id will be linked with back button,
  //#!otherwise back button will bring the user on the SDA_OS main screen.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "btnCtrl", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)){
      return 0;
    }
      sda_screen_button_handler((uint16_t) argS->arg[1].val_s, (uint16_t) argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!#### Text field handling

  //#!##### Handle text input
  //#!    sys.os.gui.handleText([num]id, [str]text);
  //#!Handles text input fields. Id is field id. Text is default text value.
  //#!Return: [str] New modified text value
  if (sysFuncMatch(argS->callId, "handleText", s)) {
    argType[1] = SVS_TYPE_NUM; //id
    argType[2] = SVS_TYPE_STR; //textval

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    x = 0;
    while(*(s->stringField + argS->arg[2].val_str + x) != 0) {
      x++;
    }

    if (gr2_get_value(argS->arg[1].val_s, &sda_app_con)) {

      if(gr2_cursor_handler(argS->arg[1].val_s, svpSGlobal.uptimeMs, &sda_app_con)) {
        sda_clipboard_overlay_init(argS->arg[1].val_s);
        result->value = argS->arg[2];
        result->type = SVS_TYPE_STR;
        return 1;
      }

      if (svpSGlobal.newStringIdFlag == argS->arg[1].val_s) {
        result->value.val_str = svpSGlobal.newString;
        svpSGlobal.newStringIdFlag = 0;
        result->type = SVS_TYPE_STR;
        return 1;
      }

      // keyboard read and svs string modification
      if (sda_get_keyboard_key_flag()) {
        
        // block rewrite handling
        if (sda_app_con.textBlockStart != 0 || sda_app_con.textBlockEnd != 0) {
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
    }

    // when string differs from the param, the parameter string is set again
    if (gr2_get_str(argS->arg[1].val_s, &sda_app_con) != (uint8_t *) ((uint32_t)s->stringField + (uint32_t)argS->arg[2].val_str)) {
      gr2_set_str(argS->arg[1].val_s, s->stringField + argS->arg[2].val_str, &sda_app_con);
    }
    
    result->value = argS->arg[2];
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Set keyboard string
  //#!    sys.os.gui.setKbdStr([str] string);
  //#!Sets the current keyboard string (max 63 chars)
  //#!Backspace code is "\b", delete is "\bd"
  //#!Return: [num] 1 - ok, 0 - string too long
  if (sysFuncMatch(argS->callId, "setKbdStr", s)) {
    argType[1] = SVS_TYPE_STR; // new keyboard string

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    if (sda_strlen(s->stringField + argS->arg[1].val_str) < sizeof(svpSGlobal.kbdKeyStr)) {
      sda_strcp(s->stringField + argS->arg[1].val_str, svpSGlobal.kbdKeyStr, sizeof(svpSGlobal.kbdKeyStr));
      svpSGlobal.kbdFlag = 1;
      result->value.val_u = 1;
    } else {
      result->value.val_u = 0;
    }
    
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Paste clipboard
  //#!    sys.os.gui.pasteClipboard();
  //#!Pastes clipboard into active text field
  //#!Return: none
  if (sysFuncMatch(argS->callId, "pasteClipboard", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }

    svpSGlobal.newStringIdFlag = sda_app_con.textActiveId;
    svpSGlobal.newString = strInsert(
      (uint16_t)((uint32_t) gr2_get_str(
        sda_app_con.textActiveId,
        &sda_app_con) - (uint32_t) s->stringField
      ),
      strNew(svpSGlobal.clipboard, s),
      gr2_get_param(sda_app_con.textActiveId, &sda_app_con),
      s
    );
    gr2_set_param(
      sda_app_con.textActiveId,
      gr2_get_param(sda_app_con.textActiveId, &sda_app_con) + sda_strlen(svpSGlobal.clipboard),
      &sda_app_con),
    
    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get text cursor position
  //#!    sys.os.gui.getCPos([num] id);
  //#!Gets the cursor position of a text field
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "getCPos", s)) {
    argType[1] = SVS_TYPE_NUM; //id

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }
    
    result->value.val_u = get_char_cursor_pos(gr2_get_param(argS->arg[1].val_s, &sda_app_con), gr2_get_str(argS->arg[1].val_s, &sda_app_con));
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set text cursor position
  //#!    sys.os.gui.setCPos([num] id, [num]val);
  //#!Sets the cursor position of a text field
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "setCPos", s)) {
    argType[1] = SVS_TYPE_NUM; //id
    argType[2] = SVS_TYPE_NUM; //val

    if(sysExecTypeCheck(argS, argType, 2, s)){
      return 0;
    }
    
    if (argS->arg[2].val_s >= 0) {
      gr2_set_param(argS->arg[1].val_s, get_real_cursor_pos(argS->arg[2].val_s, gr2_get_str(argS->arg[1].val_s, &sda_app_con)), &sda_app_con);
    }

    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Switch between landscape and portrait mode
  //#!    sys.os.gui.setLandscape([num]val);
  //#!Sets the orientation of the display.
  //#!1 - Landscape
  //#!0 - Portrait
  //#!Return: none
  if (sysFuncMatch(argS->callId, "setLandscape", s)) {
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }
    
    sda_set_landscape(argS->arg[1].val_s);
    svmSetLandscape(argS->arg[1].val_s);

    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get display orientation
  //#!    sys.os.gui.getLandscape();
  //#!Gets the orientation of the display.
  //#!1 - Landscape
  //#!0 - Portrait
  //#!Return: [num]val 
  if (sysFuncMatch(argS->callId, "getLandscape", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_u = svpSGlobal.lcdLandscape;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}
