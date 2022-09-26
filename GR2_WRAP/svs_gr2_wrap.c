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

//#!  Automatically generated documentation for GR2 SVS wrapper, follows markdown syntax.
//#!
//#!#### Constants
//#!| Constant | Value | Meaning |
//#!|---|---|---|
//#!|EV_PRESSED|1| Event: pressed|
//#!|EV_RELEASED|3| Event: released|
//#!|EV_HOLD|2| Event: hold|
//#!|EV_NONE|0| Event: none|

//#!|COL_BORDER|1| Color: Border|
//#!|COL_TEXT|2| Color: Text|
//#!|COL_BACKGROUND|3| Color: Background|
//#!|COL_FILL|4| Color: Fill|
//#!|COL_ACTIVE|5| Color: active|
//#!|ALIGN_LEFT|5| Text align: Left|
//#!|ALIGN_RIGHT|5| Text align: Right|
//#!|ALIGN_CENTER|5| Text align: Center|

svsConstType gr2WrapConsts[] = {
  {"EV_PRESSED", 1},
  {"EV_RELEASED", 3},
  {"EV_HOLD", 2},
  {"EV_NONE", 0},

  {"COL_BORDER", 1},
  {"COL_TEXT", 2},
  {"COL_BACKGROUND", 3},
  {"COL_FILL", 4},
  {"COL_ACTIVE", 5},

  {"ALIGN_LEFT", 0},
  {"ALIGN_RIGHT", 1},
  {"ALIGN_CENTER", 2},

  {"end", 0}
};


uint8_t svsGr2Wrap(varRetVal *result, argStruct *argS, svsVM *s);

void svsGr2WrapInit(){
  addSysWrapper(svsGr2Wrap, (uint8_t *)"gui");
  addSysConsts(gr2WrapConsts);
}

uint8_t svsGr2Wrap(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];
  //#!
  //#!#### Element constructors
  //#!

  //#!##### New screen
  //#!    sys.gui.addScreen();
  //#!Creates new screen.
  //#!Return: [num]scrId
  if (sysFuncMatch(argS->callId, "addScreen", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = gr2_add_screen(&sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!##### New frame
  //#!    sys.gui.addFrame([num]x1, [num]y1, [num]x2, [num]y2, [num]value, [num]scrId);
  //#!Creates new pscg frame. Value contains id of screen inside frame.
  //#!Return: [num]elementId
  if (sysFuncMatch(argS->callId,"addFrame",s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 0; //val
    argType[6] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 6, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_frame(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      argS->arg[5].val_s,
      argS->arg[6].val_s,
      &sda_app_con);
    result->type = 0;
    return 1;
  }

  //#!##### New text field
  //#!    sys.gui.addText([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
  //#!Adds a new text field.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "addText", s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 1; //str
    argType[6] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 6, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_text(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      s->stringField + argS->arg[5].val_str,
      argS->arg[6].val_s,
      &sda_app_con
    );
    result->type = 0;
    return 1;
  }

  //#!##### New button
  //#!    sys.gui.addButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
  //#!Creates new button.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "addButton", s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 1; //str
    argType[6] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 6, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_button(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      s->stringField + argS->arg[5].val_str,
      argS->arg[6].val_s,
      &sda_app_con
    );
    result->type = 0;
    return 1;
  }

  //#!##### New color button
  //#!    sys.gui.addCButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
  //#!Adds color button, color is stored in its value.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "addCButton", s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 1; //str
    argType[6] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 6, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_cbutton(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      s->stringField + argS->arg[5].val_str,
      argS->arg[6].val_s,
      &sda_app_con
    );
    result->type = 0;
    return 1;
  }

  //#!##### New check box
  //#!    sys.gui.addCheckBox([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
  //#!Creates new checkbox. Checkbox state is stored in its value.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "addCheckBox", s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 1; //str
    argType[6] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 6, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_checkbox(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      s->stringField + argS->arg[5].val_str,
      argS->arg[6].val_s,
      &sda_app_con
    );
    result->type = 0;
    return 1;
  }

  //#!##### New icon
  //#!    sys.gui.addIcon([num]x1, [num]y1, [num]x2, [num]y2, [str]description, [str]image, [num]scrId);
  //#!Adds icon. Image must be a file in current working directory, with resolution 64x64px.
  //#!When parameter of icon element is not zero, color of value param - 1 (16bit RGB565) is drawn as transparent.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "addIcon", s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 1; //str
    argType[6] = 1; //path to image
    argType[7] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 7, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_icon(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      s->stringField + argS->arg[5].val_str,
      s->stringField + argS->arg[6].val_str,
      argS->arg[7].val_s,
      &sda_app_con
    );
    result->type = 0;
    return 1;
  }

  //#!##### Set icon
  //#!    sys.gui.setIcon([num]id, [str]image);
  //#!Sets image for given icon. Image must be a file in current working directory, with resolution 64x64px.
  //#!When parameter of icon element is not zero, color of value param - 1 (16bit RGB565) is drawn as transparent.
  //#!Return: none

  if (sysFuncMatch(argS->callId, "setIcon", s)) {
    argType[1] = 0; //id
    argType[2] = 1; //path to image
    
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    gr2_set_str2(argS->arg[1].val_s, s->stringField + argS->arg[2].val_str, &sda_app_con);

    result->value.val_s = 0;    
    result->type = 0;
    return 1;
  }

  //#!##### New image
  //#!    sys.gui.addImage([num]x1, [num]y1, [num]x2, [num]y2, [str]fname, [num]scrId);
  //#!Creates new .ppm image container. Name of image is stored in str_value
  //#!Size attribute is stored in value, (one by default)
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "addImage", s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 1; //str
    argType[6] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 6, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_image(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      s->stringField + argS->arg[5].val_str,
      argS->arg[6].val_s,
      &sda_app_con
    );
    result->type = 0;
    return 1;
  }

  //#!##### New vertical slider
  //#!    sys.gui.addSliderV([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
  //#!Adds a new vertical slider. (pAddSlider is also accepted)
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "addSlider", s) || sysFuncMatch(argS->callId, "addSliderV", s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 0; //z_kolika
    argType[6] = 0; //kolik
    argType[7] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 7, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_slider_v(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      argS->arg[5].val_s,
      argS->arg[6].val_s,
      argS->arg[7].val_s,
      &sda_app_con
    );
    result->type = 0;
    return 1;
  }

  //#!##### New horizontal slider
  //#!    sys.gui.addSliderH([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
  //#!Adds a new horizontal slider.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "addSliderH", s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 0; //z_kolika
    argType[6] = 0; //kolik
    argType[7] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 7, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_slider_h(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      argS->arg[5].val_s,
      argS->arg[6].val_s,
      argS->arg[7].val_s,
      &sda_app_con
    );
    result->type = 0;
    return 1;
  }

  //#!##### New progress bar
  //#!    sys.gui.addBar([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
  //#!Adds progress bar. Orientation depends on bar dimensions.
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "addBar", s)) {
    argType[1] = 0; //x1
    argType[2] = 0; //y1
    argType[3] = 0; //x2
    argType[4] = 0; //y2
    argType[5] = 0; //z_kolika
    argType[6] = 0; //kolik
    argType[7] = 0; //scrId

    if(sysExecTypeCheck(argS, argType, 7, s)) {
      return 0;
    }

    result->value.val_s = gr2_add_progbar_v(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      argS->arg[5].val_s,
      argS->arg[6].val_s,
      argS->arg[7].val_s,
      &sda_app_con
    );
    result->type = 0;
    return 1;
  }

  //#!#### Destructors

  //#!    sys.gui.destroy([num]id);
  //#!Destroys element with given id.
  //#!Return: none
  if (sysFuncMatch(argS->callId, "destroy", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    gr2_destroy((uint16_t) argS->arg[1].val_s, &sda_app_con);
    return 1;
  }

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

  return 0;
}

