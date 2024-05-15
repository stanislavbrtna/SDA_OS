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


uint8_t svm_text_handler(varRetVal *result, argStruct *argS, svsVM *s) {
  uint32_t x = 0;

  // set default return
  result->value = argS->arg[2];
  result->type = SVS_TYPE_STR;

  // text not active
  if (!gr2_get_value(argS->arg[1].val_s, &sda_app_con)) {
    return 1;
  }

  if(gr2_cursor_handler(argS->arg[1].val_s, svpSGlobal.uptimeMs, &sda_app_con)) {
    sda_clipboard_overlay_init(argS->arg[1].val_s);
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

  return 1;
}


uint16_t sda_load_sic(uint8_t * fname,uint8_t * callback) {
  svp_file fil;
  uint8_t icon[128];
  uint8_t c = 0;
  uint16_t i = 0;

  if (svp_fexists(fname)) {
    
    svp_fopen_rw(&fil, fname);
    while (!svp_feof(&fil) && i < 128) {
      icon[i] = svp_fread_u8(&fil);
      i++;
    }
    svp_fclose(&fil);

    return sda_custom_icon_set(icon, svmGetPid(), callback);
  } else {
    printf("%s: File does not exist!: %s\n", __FUNCTION__, fname);
  }

  return 0;
}


uint8_t sda_os_gui_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Gui

  //#!##### Set main application screen
  //#!    sys.os.gui.setMainScr([num]id);
  //#!Sets main screen to screen with given id
  //#!When you wish to display overlay only, set this to 0.
  //#!
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
  //#!
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "getMainScr", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }
     result->value.val_u = svmGetMainScreen();
     result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set root for redraw
  //#!    sys.os.gui.setRoot([num]in_apps, [str]dir);
  //#!Sets custom root directory for the redraw function.
  //#!All paths for icons and other images pased to gui functions will
  //#!use this folder as a root.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setRoot", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)){
      return 0;
    }

    uint8_t buff[128];
    if (argS->arg[1].val_s == 0) {
      sda_strcp("DATA/", buff, sizeof(buff));
      sda_str_add(buff, s->stringField + argS->arg[2].val_str);
      svmSetDrawRoot(buff);
    }

    if (argS->arg[1].val_s == 1) {
      sda_strcp("APPS/", buff, sizeof(buff));
      sda_str_add(buff, s->stringField + argS->arg[2].val_str);
      svmSetDrawRoot(buff);
    }
    
    return 1;
  }

  //#!##### Handle keypad input of a screen
  //#!    sys.os.gui.btnCtrl([num]screen_id, [num]back_btn_id);
  //#!Allows control of a given screen via buttons.
  //#!Element given as back_btn_id will be linked with back button.
  //#!When 0 is passed instead of id, back button will bring the user on the SDA_OS main screen.
  //#!
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

  //#!##### Selects element for keyboard control
  //#!    sys.os.gui.btnSelect([num]element_id);
  //#!Selects elemenet for keypad control.
  //#!If the desired element is on a sub-screen of an button controlled screen,
  //#!then the sub-screen must be also selected.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "btnSelect", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }
      gr2_ki_select((uint16_t) argS->arg[1].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Selects element for keyboard control
  //#!    sys.os.gui.btnGetSel([num]screen_id);
  //#!Gets selected element in current screen (or its sub-screens)
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "btnGetSel", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_s = gr2_ki_get_selected((uint16_t) argS->arg[1].val_s, &sda_app_con); 
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Clear button control for a screen
  //#!    sys.os.gui.btnClear([num]screen_id);
  //#!Clears keypad input for entire screen.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "btnClear", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }
      gr2_ki_unselect((uint16_t) argS->arg[1].val_s, &sda_app_con);
    return 1;
  }

  //#!#### Text field handling

  //#!##### Handle text input
  //#!    sys.os.gui.handleText([num]id, [str]text);
  //#!Handles text input fields. Id is field id. Text is default text value.
  //#!
  //#!Return: [str] New modified text value
  if (sysFuncMatch(argS->callId, "handleText", s)) {
    argType[1] = SVS_TYPE_NUM; //id
    argType[2] = SVS_TYPE_STR; //textval

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    svm_text_handler(result, argS, s);
    
    // when string differs from the param, the parameter string is set again
    if (gr2_get_str(argS->arg[1].val_s, &sda_app_con) != (uint8_t *) ((uint32_t)s->stringField + (uint32_t)argS->arg[2].val_str)) {
      gr2_set_str(argS->arg[1].val_s, s->stringField + argS->arg[2].val_str, &sda_app_con);
    }

    return 1;
  }

  //#!##### Set keyboard string
  //#!    sys.os.gui.setKbdStr([str] string);
  //#!Sets the current keyboard string (max 63 chars)
  //#!Backspace code is "\b", delete is "\bd"
  //#!
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
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "pasteClipboard", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }

    if(sda_app_con.textActive == 0) {
      printf("Warning: sys.os.gui.pasteClipboard(): no active text field!\n");
      result->value.val_u = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
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
  //#!
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
  //#! 
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

  //#!#### Notification area icons

  //#!##### Set notification area icon
  //#!    sys.os.gui.setNotif([str] path_to_sic, [str] callback);
  //#!Sets the notification area icon. Path is dependent on actual CWD.
  //#!
  //#!
  //#!Return: [num] id (1 - 3), 0 - Error, probably no empty icon spot
  if (sysFuncMatch(argS->callId, "setNotif", s)) {
    argType[1] = SVS_TYPE_STR; // notification icon
    argType[2] = SVS_TYPE_STR; // callback

    if(sysExecTypeCheck(argS, argType, 2, s)){
      return 0;
    }

    result->value.val_u = sda_load_sic(s->stringField + argS->arg[1].val_str, s->stringField + argS->arg[2].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Free notification area icon
  //#!    sys.os.gui.freeNotif([num] id);
  //#!Removes notification icon with given id.
  //#!
  //#!Return: [num] 1 - ok, 0 - Error
  if (sysFuncMatch(argS->callId, "freeNotif", s)) {
    argType[1] = SVS_TYPE_NUM; // notification icon id

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    result->value.val_u = (uint32_t)(1 - sda_custom_icon_release_spot_pid(argS->arg[1].val_u, svmGetPid()));
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}
