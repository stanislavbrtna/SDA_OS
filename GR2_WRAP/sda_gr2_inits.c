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

uint8_t sda_gr2_inits_subwrap(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!
  //#!#### Element constructors
  //#!

  //#!##### New screen
  //#!    sys.gui.addScreen();
  //#!or also
  //#!    sys.gui.addScreen([num]x1, [num]y1, [num]x2, [num]y2, [num]scrId);
  //#!Creates new screen.
  //#!
  //#!Return: [num]scrId
  if (sysFuncMatch(argS->callId, "addScreen", s)) {
    if (argS->usedup == 0) {
      if(sysExecTypeCheck(argS, argType, 0, s)) {
        return 0;
      }
      result->value.val_s = gr2_add_screen(&sda_app_con);
      result->type = 0;
      return 1;
    } else {
      argType[1] = 0; //x1
      argType[2] = 0; //y1
      argType[3] = 0; //x2
      argType[4] = 0; //y2
      argType[5] = 0; //scrId

      if(sysExecTypeCheck(argS, argType, 5, s)) {
        return 0;
      }
      result->value.val_s = gr2_add_screen_ext(
      argS->arg[1].val_s,
      argS->arg[2].val_s,
      argS->arg[3].val_s,
      argS->arg[4].val_s,
      argS->arg[5].val_s,
      &sda_app_con);
      result->type = 0;
      return 1;
    }
  }

  //#!##### New frame
  //#!    sys.gui.addFrame([num]x1, [num]y1, [num]x2, [num]y2, [num]value, [num]scrId);
  //#!Creates new pscg frame. Value contains id of screen inside frame.
  //#!
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
  //#!
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
  //#!
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
  //#!
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
  //#!
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
  //#!Adds icon. Image must be a file in current working directory.
  //#!Optimal resolution is 64x64px. Icon will try to accomodate given space by scaling itself.
  //#!use sys.gui.setTxtAlign to change position of the description box.
  //#!When parameter of icon element is not zero, color of value param - 1 (16bit RGB565) is drawn as transparent.
  //#!
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
  //#!
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
  //#!Creates new image (ppm or p16) container. Name of image is stored in str_value
  //#!Size attribute is stored in value, (one by default)
  //#!Color of value param - 1 (16bit RGB565) is drawn as transparent.
  //#!
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
  //#!
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
  //#!
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
  //#!Adds progress bar. Orientation depends on its dimensions.
  //#!
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
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "destroy", s)) {
    argType[1] = 0;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    gr2_destroy((uint16_t) argS->arg[1].val_s, &sda_app_con);
    return 1;
  }

  return 2;
}