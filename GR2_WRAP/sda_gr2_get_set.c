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

#include "sda_gr2_wrap.h"

uint8_t sda_gr2_getset_subwrap(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Getters & setters

  //#!##### Value
  //#!    sys.gui.getValue([num]Id);
  //#!Gets value of pscg item.
  //#!Return: [num]value
  if (sysFuncMatch(argS->callId,"getValue",s)){
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_value(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setValue([num]Id, [num]value);
  //#!Sets value of pscg item.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setValue", s)) {
    argType[1] = 0; //id
    argType[2] = 0; //val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_value(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Modified flag
  //#!    sys.gui.setModif([num]Id);
  //#!Sets modified flag of an element.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setModif", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    gr2_set_modified(argS->arg[1].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Parameter
  //#!    sys.gui.getParam([num]Id);
  //#!Gets elements parameter value.
  //#!Return: [num]param
  if (sysFuncMatch(argS->callId, "getParam", s)) {
    argType[1] = 0; //id
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_s = gr2_get_param(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setParam([num]Id, [num]value);
  //#!Sets elements parameter value.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setParam", s)) {
    argType[1] = 0; //id
    argType[2] = 0; //val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_param(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!##### Events
  //#!    sys.gui.getEvent([num]Id);
  //#!Gets event from an element.
  //#!Return: [num] event value, one of EV_ defines.
  if (sysFuncMatch(argS->callId, "getEvent", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    uint16_t ret = 0;
    if (gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_PRESSED) {
      ret = 1;
    } else if(gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_HOLD) {
      ret = 2;
    } else if(gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_RELEASED) {
      ret = 3;
    } else if(gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_DRAGOUT) {
      ret = 4;
    }
    result->value.val_s = ret;
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.getEventC([num]Id);
  //#!Gets event from an element and clears the event.
  //#!Return: [num] event value, one of EV_ defines.
  if (sysFuncMatch(argS->callId, "getEventC", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    uint16_t ret = 0;
    if (gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_PRESSED) {
      ret = 1;
    } else if(gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_HOLD) {
      ret = 2;
    } else if(gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_RELEASED) {
      ret = 3;
    } else if(gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_DRAGOUT) {
      ret = 4;
    }
    result->value.val_s = ret;
    result->type = 0;

    gr2_set_event(argS->arg[1].val_s, EV_NONE, &sda_app_con);

    return 1;
  }

  //#!
  //#!    sys.gui.setEvent([num]Id, [num] event);
  //#!Sets event of an element.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setEvent", s)) {
    argType[1] = 0; //id
    argType[2] = 0; //val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2EventType ev = EV_NONE;

    if (argS->arg[2].val_s == 1) {
      ev = EV_PRESSED;
    }else if(argS->arg[2].val_s == 2) {
      ev = EV_HOLD;
    }else if(argS->arg[2].val_s == 3) {
      ev = EV_RELEASED;
    }

    gr2_set_event(argS->arg[1].val_s, ev, &sda_app_con);
    return 1;
  }

  //#!
  //#!    sys.gui.clrScrEv([num]Id);
  //#!Clears event for whole screen and its sub-screens.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "clrScrEv", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    gr2_clear_screen_ev((uint16_t)argS->arg[1].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Screen
  //#!    sys.gui.setScreen([num]Id, [num]screenId);
  //#!Sets element screen parameter.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setScreen", s)) {
    argType[1] = 0; //id
    argType[2] = 0; //screenId

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_screen(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Grayout
  //#!    sys.gui.getGrayout([num]Id);
  //#!Gets element grayout.
  //#!Return: [num] grayout, 1 if element is grey, 0 if it is normal
  if (sysFuncMatch(argS->callId, "getGrayout", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_grayout(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setGrayout([num]Id, [num]grayout);
  //#!Sets element grayout.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setGrayout", s)) {
    argType[1] = 0; //id
    argType[2] = 0; //val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    gr2_set_grayout(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Visibility
  //#!    sys.gui.getVisible([num]Id);
  //#!Gets element visibility.
  //#!Return: [num] visibility
  if (sysFuncMatch(argS->callId, "getVisible", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_visible(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setVisible([num]Id, [num]visibility);
  //#!Sets element visibility.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setVisible", s)) {
    argType[1] = 0; //id
    argType[2] = 0; //val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_visible(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Ghost buttons
  //#!    sys.gui.getGhost([num]Id);
  //#!Gets element ghost parameter.
  //#!Return: [num] isGhost
  if (sysFuncMatch(argS->callId, "getGhost", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_ghost(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setGhost([num]Id, [num]ghost);
  //#!Sets element ghost parameter.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setGhost", s)) {
    argType[1] = 0; //id
    argType[2] = 0; //val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_ghost(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Selected buttons
  //#!    sys.gui.getSelect([num]Id);
  //#!Gets element selected parameter.
  //#!Return: [num] isSelected
  if (sysFuncMatch(argS->callId, "getSelect", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_select(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setSelect([num]Id, [num]select);
  //#!Sets element select parameter.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setSelect", s)) {
    argType[1] = 0; //id
    argType[2] = 0; //val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_select(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Slider size
  //#!
  //#!    sys.gui.setSliderSize([num]Id, [num]val);
  //#!Sets size of slider in pixels.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setSliderSize", s)) {
    argType[1] = SVS_TYPE_NUM; // id
    argType[2] = SVS_TYPE_NUM; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    gr2_set_slider_size(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!##### String parameter
  //#!    sys.gui.getStr([num]Id);
  //#!Gets element value_str parameter.
  //#!Return: [str]str
  if (sysFuncMatch(argS->callId, "getStr", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_str = strNew(gr2_get_str(argS->arg[1].val_s, &sda_app_con), s);
    result->type = 1;
    return 1;
  }

  //#!
  //#!    sys.gui.setStr([num]Id, [str]str);
  //#!Sets element value_str parameter.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setStr", s)) {
    argType[1] = 0; // id
    argType[2] = 1; // str

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_str(
      argS->arg[1].val_s,
      s->stringField + argS->arg[2].val_str,
      &sda_app_con
    );

    return 1;
  }

  //#!
  //#!    sys.gui.setTxtSize([num]Id, [num]val);
  //#!Sets size of text inside buttons or text fields.
  //#!Possible values are those used by LCD_Set_Sys_Font
  //#!By default they are: 12, 18, 32, 70, 87
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

  //#!##### Keypad control
  //#!    sys.gui.getBtnSel([num]screenId);
  //#!Gets element selected by the keypad input method
  //#!from the given screen.
  //#!Return: [num] Id if something is selected, otherwise 0
  if (sysFuncMatch(argS->callId, "getBtnSel", s)) {
    argType[1] = SVS_TYPE_NUM; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_ki_get_selected(argS->arg[1].val_s, &sda_app_con);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Size and placement
  //#!
  //#!    sys.gui.setRelInit([num]val);
  //#!Sets aplication gr2 context to relative init mode.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setRelInit", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    gr2_set_relative_init(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setXYXY([num]Id, [num] x1, [num] y1, [num] x2, [num] y2);
  //#!Sets position of element inside screen.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setXYXY", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // x1
    argType[3] = 0; // y1
    argType[4] = 0; // x2
    argType[5] = 0; // y2

    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }
    gr2_set_x1y1x2y2(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      argS->arg[5].val_s,
      &sda_app_con
    );
    return 1;
  }

  //#!
  //#!    sys.gui.setX1([num]Id, [num] x1);
  //#!Sets position of element inside screen.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setX1", s)) {
    argType[1] = SVS_TYPE_NUM; // id
    argType[2] = SVS_TYPE_NUM; // x1

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    gr2_set_x1(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!
  //#!    sys.gui.setX2([num]Id, [num] x2);
  //#!Sets position of element inside screen.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setX2", s)) {
    argType[1] = SVS_TYPE_NUM; // id
    argType[2] = SVS_TYPE_NUM; // x2

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    gr2_set_x2(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!
  //#!    sys.gui.setY1([num]Id, [num] y1);
  //#!Sets position of element inside screen.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setY1", s)) {
    argType[1] = SVS_TYPE_NUM; // id
    argType[2] = SVS_TYPE_NUM; // y1

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    gr2_set_y1(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!
  //#!    sys.gui.setY2([num]Id, [num] y2);
  //#!Sets position of element inside screen.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setY2", s)) {
    argType[1] = SVS_TYPE_NUM; // id
    argType[2] = SVS_TYPE_NUM; // y2

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    gr2_set_y2(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!
  //#!    sys.gui.getX1([num]Id);
  //#!Gets element position.
  //#!Return: [num] x1
  if (sysFuncMatch(argS->callId, "getX1", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_x1(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.getX2([num]Id);
  //#!Gets element position.
  //#!Return: [num] x2
  if (sysFuncMatch(argS->callId, "getX2", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_x2(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.getY1([num]Id);
  //#!Gets element position.
  //#!Return: [num] y1
  if (sysFuncMatch(argS->callId, "getY1", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_y1(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.getY2([num]Id);
  //#!Gets element position.
  //#!Return: [num] y2
  if (sysFuncMatch(argS->callId, "getY2", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_y2(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setSpacing([num]Id, [num] left, [num] right, [num] top, [num] bottom);
  //#!Sets element spacing atributes of given screen.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setSpacing", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // left
    argType[3] = 0; // right
    argType[4] = 0; // top
    argType[5] = 0; // bottom

    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }
    gr2_set_cell_space_left(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    gr2_set_cell_space_right(argS->arg[1].val_s, argS->arg[3].val_s, &sda_app_con);
    gr2_set_cell_space_top(argS->arg[1].val_s, argS->arg[4].val_s, &sda_app_con);
    gr2_set_cell_space_bottom(argS->arg[1].val_s, argS->arg[5].val_s, &sda_app_con);
    return 1;
  }

  //#!
  //#!    sys.gui.getXcell([num]screenId);
  //#!Gets screen Xcell parameter.
  //#!Return: [num] Xcell
  if (sysFuncMatch(argS->callId, "getXcell", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_x_cell(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setXcell([num]screenId, [num] Xcell);
  //#!Sets screen Xcell parameter. (32 by default)
  //#!Return: None
  if (sysFuncMatch(argS->callId,"setXcell",s)) {
    argType[1] = 0; //id
    argType[2] = 0; //val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_x_cell(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!
  //#!    sys.gui.getYcell([num]screenId);
  //#!Gets screen Ycell parameter.
  //#!Return: [num] Ycell
  if (sysFuncMatch(argS->callId, "getYcell", s)){
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_y_cell(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setYcell([num]screenId, [num] Ycell);
  //#!Sets screen Ycell parameter. (32 by default)
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setYcell", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_y_cell(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!
  //#!    sys.gui.getXscroll([num]screenId);
  //#!Gets screen Xscroll.
  //#!Return: [num] Xscroll
  if (sysFuncMatch(argS->callId, "getXscroll", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_xscroll(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setXscroll([num]screenId, [num]Xscroll);
  //#!Sets Xscroll parameter.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setXscroll", s)) {
    argType[1] = 0; //id
    argType[2] = 0; //val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_xscroll(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!
  //#!    sys.gui.getYscroll([num]screenId);
  //#!Gets Yscroll parameter.
  //#!Return: [num] Yscroll
  if (sysFuncMatch(argS->callId, "getYscroll", s)) {
    argType[1] = 0; //id

    if(sysExecTypeCheck(argS, argType, 1,s)){
      return 0;
    }

    result->value.val_s = gr2_get_yscroll(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!
  //#!    sys.gui.setYscroll([num]screenId, [num]Yscroll);
  //#!Sets Yscroll parameter.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setYscroll", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2,s)) {
      return 0;
    }

    gr2_set_yscroll(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!##### Fonts & texts
  //#!    sys.gui.setDefFont([num]screenId, [num] val);
  //#!Sets defalt screen font.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setDefFont", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_default_font(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!
  //#!    sys.gui.getTexAct([num]Id);
  //#!Gets if given editable text field is currently active.
  //#!Return: [num]isActive
  if (sysFuncMatch(argS->callId, "getTexAct", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_get_text_active(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;

    return 1;
  }

  //#!
  //#!    sys.gui.setTexAct([num]Id);
  //#!Sets given editable text field as currently active.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTexAct", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    gr2_activate_text(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;

    return 1;
  }

  //#!
  //#!    sys.gui.texDeact();
  //#!Deactivates currently active text field.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "texDeact", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    gr2_text_deactivate(&sda_app_con);
    result->type = 0;

    return 1;
  }

  //#!
  //#!    sys.gui.setTexFit([num]Id, [num]val);
  //#!Sets automatic line-breaking. Position of first line break is stored in the parameter.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTexFit", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_text_set_fit(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
    return 1;
  }

  //#!
  //#!    sys.gui.setTexEd([num]Id, [num]val);
  //#!Sets text field as editable.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTexEd", s)) {
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

  //#!
  //#!    sys.gui.setBlk([num] id, [num] start, [num] stop);
  //#!Sets start and stop of a block in active text field
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

  //#!
  //#!    sys.gui.getBlkStart([num] id);
  //#!Gets text field block start.
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

  //#!
  //#!    sys.gui.getBlkEnd([num] id);
  //#!Gets text field block end.
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

  //#!
  //#!    sys.gui.setTexBlk([num]Id, [num]val);
  //#!Enables block selection in a text field.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTexBlk", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_block_enable(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!
  //#!    sys.gui.setTexPwd([num]Id, [num]val);
  //#!Sets text field as password field.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTexPwd", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_text_set_pwd(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!
  //#!    sys.gui.getTexPwd([num]Id);
  //#!Gets if text field is a password field.
  //#!Return: [num]isPassword
  if (sysFuncMatch(argS->callId, "getTexPwd", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_text_get_pwd(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;

    return 1;
  }

  //#!
  //#!    sys.gui.setTexAlign([num]Id, [num]val);
  //#!Sets text alignment. (uses consts: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTexAlign", s)) {
    argType[1] = 0; // id
    argType[2] = 0; // val

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_text_set_align(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

    return 1;
  }

  //#!
  //#!    sys.gui.getTexAlign([num]Id);
  //#!Gets text alignment.
  //#!Return: [num]alignment (uses consts: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER)
  if (sysFuncMatch(argS->callId, "getTexAlign", s)) {
    argType[1] = 0; // id

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = gr2_text_get_align(argS->arg[1].val_s, &sda_app_con);
    result->type = 0;

    return 1;
  }


  //#!##### Colours
  //#!
  //#!    sys.gui.setColor([num]Col, [num]val);
  //#!Sets given color to given value.
  //#!Available system colors: COL_BORDER, COL_TEXT, COL_BACKGROUND, COL_FILL, COL_ACTIVE
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setColor", s)) {
    argType[1] = 0;
    argType[2] = 0;

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    if (argS->arg[1].val_s == 1) {
      gr2_set_border_color(argS->arg[2].val_s, &sda_app_con);
    } else if (argS->arg[1].val_s == 2) {
      gr2_set_text_color(argS->arg[2].val_s, &sda_app_con);
    } else if (argS->arg[1].val_s == 3) {
      gr2_set_background_color(argS->arg[2].val_s, &sda_app_con);
    } else if (argS->arg[1].val_s == 4) {
      gr2_set_fill_color(argS->arg[2].val_s, &sda_app_con);
    } else if (argS->arg[1].val_s == 5) {
      gr2_set_active_color(argS->arg[2].val_s, &sda_app_con);
    }

    return 1;
  }

  //#!
  //#!    sys.gui.getColor([num]Col);
  //#!Gets value of given color define.
  //#!Available system colors: COL_BORDER, COL_TEXT, COL_BACKGROUND, COL_FILL, COL_ACTIVE
  //#!Return: [num]val
  if (sysFuncMatch(argS->callId, "getColor", s)) {
    argType[1] = 0;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (argS->arg[1].val_s == 1) {
      result->value.val_s = gr2_get_border_color(&sda_app_con);
    } else if (argS->arg[1].val_s == 2) {
      result->value.val_s = gr2_get_text_color(&sda_app_con);
    } else if (argS->arg[1].val_s == 3) {
      result->value.val_s = gr2_get_background_color(&sda_app_con);
    } else if (argS->arg[1].val_s == 4) {
      result->value.val_s = gr2_get_fill_color(&sda_app_con);
    } else if (argS->arg[1].val_s == 5) {
      result->value.val_s = gr2_get_active_color(&sda_app_con);
    }
    result->type = 0;
    return 1;
  }
  
  return 2;
}