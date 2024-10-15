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

#include "sda_gr2_wrap.h"


//#!#### Global text modificators
//#! Functions that works on all elements that display text.
  

uint8_t sda_gr2_txt_subwrap(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];
  //#!##### Set text size
  //#!    sys.gui.setTxtSize([num]Id, [num]val);
  //#!Sets size of text inside buttons or text fields.
  //#!Possible values are those used by LCD_Set_Sys_Font
  //#!By default they are: 12, 18 (default), 32, 70, 87
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTxtSize", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    gr2_text_set_size(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!##### Get text size
  //#!    sys.gui.getTxtSize([num]Id);
  //#!Gets size of text inside buttons or text fields.
  //#!
  //#!Return: [num] font_size
  if (sysFuncMatch(argS->callId, "getTxtSize", s)) {
    argType[1] = SVS_TYPE_NUM; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_text_get_size(argS->arg[1].val_s, &sda_app_con);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set text alignment
  //#!    sys.gui.setTxtAlign([num]Id, [num]val);
  //#!    sys.gui.setTexAlign([num]Id, [num]val); # TBR
  //#!Sets text alignment. (uses consts: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTxtAlign", s) || sysFuncMatch(argS->callId, "setTexAlign", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_text_set_align(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!##### Set text alignment
  //#!    sys.gui.getTxtAlign([num]Id);
  //#!    sys.gui.getTexAlign([num]Id); # TBR
  //#!Gets text alignment.
  //#!
  //#!Return: [num]alignment (uses consts: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
  if (sysFuncMatch(argS->callId, "getTexAlign", s) || sysFuncMatch(argS->callId, "getTxtAlign", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_text_get_align(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;

    return 1;
  }

  //#!#### Misc
  //#!##### Set default text size for a screen
  //#!    sys.gui.defTxtSize([num]screenId, [num] val);
  //#!    sys.gui.setDefFont([num]screenId, [num] val); # TBR
  //#!Sets defalt screen text size.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setDefFont", s) || sysFuncMatch(argS->callId, "defTxtSize", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_default_font(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!#### Text element modificators
  //#! Functions that work only on text element.

  //#!#####  Set text field as password
  //#!    sys.gui.setTxtPwd([num]Id, [num]val);
  //#!    sys.gui.setTexPwd([num]Id, [num]val); # TBR
  //#!Sets text field as password field. Draws stars instead of characters.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTexPwd", s) || sysFuncMatch(argS->callId, "setTxtPwd", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_text_set_pwd(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!##### Get password value
  //#!    sys.gui.getTxtPwd([num]Id);
  //#!    sys.gui.getTexPwd([num]Id); # TBR
  //#!Gets if text field is a password field.
  //#!
  //#!Return: [num]isPassword
  if (sysFuncMatch(argS->callId, "getTexPwd", s) || sysFuncMatch(argS->callId, "getTxtPwd", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_text_get_pwd(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;

    return 1;
  }

  //#!##### Set text fit
  //#!    sys.gui.setTxtFit([num]Id, [num]val);
  //#!    sys.gui.setTexFit([num]Id, [num]val); # TBR
  //#!Sets automatic line-breaking. val: 1 - enabled, 0 - disabled
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTxtFit", s) || sysFuncMatch(argS->callId, "setTexFit", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_text_set_fit(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Set text fit
  //#!    sys.gui.setTxtEd([num]Id, [num]val);
  //#!    sys.gui.setTexEd([num]Id, [num]val); # TBR
  //#!Sets text field as editable.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTxtEd", s) || sysFuncMatch(argS->callId, "setTexEd", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    gr2_text_set_editable(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    // block mode enabled for all editable text fields
    gr2_set_block_enable(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!#### Text element activation/deactivation

  //#!##### Set text active
  //#!    sys.gui.setTxtAct([num]Id);
  //#!    sys.gui.setTexAct([num]Id); # TBR
  //#!Sets given editable text field as currently active.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTxtAct", s) || sysFuncMatch(argS->callId, "setTexAct", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    gr2_activate_text(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;

    return 1;
  }

  //#!##### Get text active
  //#!    sys.gui.getTxtAct([num]Id);
  //#!    sys.gui.getTexAct([num]Id);
  //#!Gets if given editable text field is currently active.
  //#!
  //#!Return: [num]isActive
  if (sysFuncMatch(argS->callId, "getTxtAct", s) || sysFuncMatch(argS->callId, "getTexAct", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_text_active(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;

    return 1;
  }

  //#!##### Deactivate active text
  //#!    sys.gui.txtDeact();
  //#!    sys.gui.texDeact(); # TBR
  //#!Deactivates currently active text field.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "txtDeact", s) || sysFuncMatch(argS->callId, "texDeact", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    gr2_text_deactivate(&sda_app_con);
    result->type = 0;

    return 1;
  }

  //#!#### Text block functions

  //#!##### Enable block select mode
  //#!    sys.gui.setTxtBlk([num]Id, [num]val);
  //#!    sys.gui.setTexBlk([num]Id, [num]val); # TBR
  //#!Enables block selection in a text field.
  //#!This is enabled by default, so it's mainly for disabling block mode.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTxtBlk", s) || sysFuncMatch(argS->callId, "setTexBlk", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_block_enable(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }
  
  //#!##### Set text block
  //#!    sys.gui.setBlk([num] id, [num] start, [num] stop);
  //#!Sets start and stop of a block in active text field
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setBlk", s)) {
    argType[1] = SVS_TYPE_NUM; // id
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }

    result->value.val_s = 0;
    result->type = SVS_TYPE_NUM;

    if (gr2_text_get_editable(argS->arg[1].val_s, &sda_app_con) && gr2_get_text_active(argS->arg[1].val_s, &sda_app_con)) {
      sda_app_con.textBlockStart = get_char_cursor_pos(argS->arg[2].val_s, gr2_get_str(argS->arg[1].val_s, &sda_app_con));
      sda_app_con.textBlockEnd = get_char_cursor_pos(argS->arg[3].val_s, gr2_get_str(argS->arg[1].val_s, &sda_app_con));
      result->value.val_s = 1;

      gr2_set_modified(argS->arg[1].val_s, &sda_app_con);
    }

    return 1;
  }

  //#!##### Get block start
  //#!    sys.gui.getBlkStart([num] id);
  //#!Gets text field block start.
  //#!
  //#!Return: [num] block_start
  if (sysFuncMatch(argS->callId, "getBlkStart", s)) {
    argType[1] = SVS_TYPE_NUM; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = 0;
    result->type = SVS_TYPE_NUM;

    if (gr2_text_get_editable(argS->arg[1].val_s, &sda_app_con) && gr2_get_text_active(argS->arg[1].val_s, &sda_app_con)) {
      result->value.val_s = get_char_cursor_pos(sda_app_con.textBlockStart, gr2_get_str(argS->arg[1].val_s, &sda_app_con));
    }

    return 1;
  }

  //#!##### Get block end
  //#!    sys.gui.getBlkEnd([num] id);
  //#!Gets text field block end.
  //#!
  //#!Return: [num] block_end
  if (sysFuncMatch(argS->callId, "getBlkEnd", s)) {
    argType[1] = SVS_TYPE_NUM; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = 0;
    result->type = SVS_TYPE_NUM;

    if (gr2_text_get_editable(argS->arg[1].val_s, &sda_app_con) && gr2_get_text_active(argS->arg[1].val_s, &sda_app_con)) {
      result->value.val_s = get_char_cursor_pos(sda_app_con.textBlockEnd, gr2_get_str(argS->arg[1].val_s, &sda_app_con));
    }

    return 1;
  }

  return 2;
}