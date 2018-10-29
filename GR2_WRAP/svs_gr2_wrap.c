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

//#!	Automatically generated documentation for GR2 SVS wrapper, follows markdown syntax.
//#!
//#!#### Constants
//#!| Constant | Value | Meaning |
//#!|---|---|---|
//#!|EV_PRESSED|1| Event: pressed|
//#!|EV_RELEASED|3| Event: released|
//#!|EV_HOLD|2| Event: hold|
//#!|EV_NONE|0| Event: none|

//#!|COL_BORDER|1| Event: none|
//#!|COL_TEXT|2| Event: none|
//#!|COL_BACKGROUND|3| Event: none|
//#!|COL_FILL|4| Event: none|
//#!|COL_ACTIVE|5| Event: none|

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

  {"end", 0}
};


uint8_t svsGr2Wrap(varRetVal *result, argStruct *argS, svsVM *s);

void svsGr2WrapInit(){
  addSysWrapper(svsGr2Wrap);
  addSysConsts(gr2WrapConsts);
}

uint8_t svsGr2Wrap(varRetVal *result, argStruct *argS, svsVM *s) {
	uint8_t argType[11];
	//#!
	//#!#### Element constructors
	//#!

	//#!##### New screen
	//#!    pAddScreen();
	//#!Creates new screen.
	//#!Return: [num]scrId
	if (sysFuncMatch(argS->callId, "pAddScreen", s)) {
		if(sysExecTypeCheck(argS, argType, 0, s)) {
		  return 0;
		}
	  result->value.val_s = pscg_add_screen(&sda_app_con);
	  result->type = 0;
	  return 1;
	}

	//#!##### New frame
	//#!    pAddFrame([num]x1, [num]y1, [num]x2, [num]y2, [num]value, [num]scrId);
	//#!Creates new pscg frame. Value contains id of screen inside frame.
	//#!Return: [num]elementId
	if (sysFuncMatch(argS->callId,"pAddFrame",s)) {
		argType[1] = 0; //x1
		argType[2] = 0; //y1
		argType[3] = 0; //x2
		argType[4] = 0; //y2
		argType[5] = 0; //val
		argType[6] = 0; //scrId

		if(sysExecTypeCheck(argS, argType, 6, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_add_frame(
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
	//#!    pAddText([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
	//#!Adds a new text field.
	//#!Return: [num]id
	if (sysFuncMatch(argS->callId, "pAddText", s)) {
		argType[1] = 0; //x1
		argType[2] = 0; //y1
		argType[3] = 0; //x2
		argType[4] = 0; //y2
		argType[5] = 1; //str
		argType[6] = 0; //scrId

		if(sysExecTypeCheck(argS, argType, 6, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_add_text(
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
	//#!    pAddButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
	//#!Creates new button.
	//#!Return: [num]id
	if (sysFuncMatch(argS->callId, "pAddButton", s)) {
		argType[1] = 0; //x1
		argType[2] = 0; //y1
		argType[3] = 0; //x2
		argType[4] = 0; //y2
		argType[5] = 1; //str
		argType[6] = 0; //scrId

		if(sysExecTypeCheck(argS, argType, 6, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_add_button( \
	  												argS->arg[1].val_s, \
	  												argS->arg[2].val_s, \
	  												argS->arg[3].val_s, \
	  												argS->arg[4].val_s, \
	  												s->stringField + argS->arg[5].val_str, \
	  												argS->arg[6].val_s, \
													&sda_app_con
	  											);
	  result->type = 0;
	  return 1;
	}

	//#!##### New color button
	//#!    pAddCButton([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
	//#!Adds color button, color is stored in its value.
	//#!Return: [num]id
	if (sysFuncMatch(argS->callId, "pAddCButton", s)) {
		argType[1] = 0; //x1
		argType[2] = 0; //y1
		argType[3] = 0; //x2
		argType[4] = 0; //y2
		argType[5] = 1; //str
		argType[6] = 0; //scrId

		if(sysExecTypeCheck(argS, argType, 6, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_add_cbutton( \
	  																				argS->arg[1].val_s, \
	  																				argS->arg[2].val_s, \
	  																				argS->arg[3].val_s, \
	  																				argS->arg[4].val_s, \
	  																				s->stringField + argS->arg[5].val_str, \
	  																				argS->arg[6].val_s, \
																					&sda_app_con
	  																			);
	  result->type = 0;
	  return 1;
	}

	//#!##### New check box
	//#!    pAddCheckBox([num]x1, [num]y1, [num]x2, [num]y2, [str]str, [num]scrId);
	//#!Creates new checkbox. Checkbox state is stored in its value.
	//#!Return: [num]id
	if (sysFuncMatch(argS->callId, "pAddCheckBox", s)) {
		argType[1] = 0; //x1
		argType[2] = 0; //y1
		argType[3] = 0; //x2
		argType[4] = 0; //y2
		argType[5] = 1; //str
		argType[6] = 0; //scrId

		if(sysExecTypeCheck(argS, argType, 6, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_add_checkbox( \
	  												argS->arg[1].val_s, \
	  												argS->arg[2].val_s, \
	  												argS->arg[3].val_s, \
	  												argS->arg[4].val_s, \
	  												s->stringField + argS->arg[5].val_str, \
	  												argS->arg[6].val_s, \
													&sda_app_con
	  											);
	  result->type = 0;
	  return 1;
	}

	//#!##### New image
	//#!    pAddImage([num]x1, [num]y1, [num]x2, [num]y2, [str]fname, [num]scrId);
	//#!Creates new .ppm image container. Name of image is stored in str_value
	//#!Size attribute is stored in value, (one by default)
	//#!Return: [num]id
	if (sysFuncMatch(argS->callId, "pAddImage", s)) {
		argType[1] = 0; //x1
		argType[2] = 0; //y1
		argType[3] = 0; //x2
		argType[4] = 0; //y2
		argType[5] = 1; //str
		argType[6] = 0; //scrId

		if(sysExecTypeCheck(argS, argType, 6, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_add_image( \
	  												argS->arg[1].val_s, \
	  												argS->arg[2].val_s, \
	  												argS->arg[3].val_s, \
	  												argS->arg[4].val_s, \
	  												s->stringField + argS->arg[5].val_str, \
	  												argS->arg[6].val_s, \
													&sda_app_con
	  											);
	  result->type = 0;
	  return 1;
	}

	//#!##### New vertical slider
	//#!    pAddSliderV([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
	//#!Adds a new vertical slider. (pAddSlider is also accepted)
	//#!Return: [num]id
	if (sysFuncMatch(argS->callId, "pAddSlider", s) || sysFuncMatch(argS->callId, "pAddSliderV", s)) {
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

	  result->value.val_s = pscg_add_slider_v(
	  																				argS->arg[1].val_s, \
	  																				argS->arg[2].val_s, \
	  																				argS->arg[3].val_s, \
	  																				argS->arg[4].val_s, \
	  																				argS->arg[5].val_s, \
	  																				argS->arg[6].val_s, \
	  																				argS->arg[7].val_s, \
																					&sda_app_con
	  																			);
	  result->type = 0;
	  return 1;
	}

	//#!##### New horizontal slider
	//#!    pAddSliderH([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
	//#!Adds a new horizontal slider.
	//#!Return: [num]id
	if (sysFuncMatch(argS->callId, "pAddSliderH", s)) {
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

	  result->value.val_s = pscg_add_slider_h( \
	  																				argS->arg[1].val_s, \
	  																				argS->arg[2].val_s, \
	  																				argS->arg[3].val_s, \
	  																				argS->arg[4].val_s, \
	  																				argS->arg[5].val_s, \
	  																				argS->arg[6].val_s, \
	  																				argS->arg[7].val_s, \
																					&sda_app_con
	  																			);
	  result->type = 0;
	  return 1;
	}

	//#!##### New progress bar
	//#!    pAddBar([num]x1, [num]y1, [num]x2, [num]y2, [num]howMuchOverall, [num]howMuch, [num]scrId);
	//#!Adds progress bar. Orientation depends on bar dimensions.
	//#!Return: [num]id
	if (sysFuncMatch(argS->callId, "pAddVBar", s) || sysFuncMatch(argS->callId, "pAddBar", s)) {
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

	  result->value.val_s = pscg_add_progbar_v( \
	  																					argS->arg[1].val_s, \
	  																					argS->arg[2].val_s, \
	  																					argS->arg[3].val_s, \
	  																					argS->arg[4].val_s, \
	  																					argS->arg[5].val_s, \
	  																					argS->arg[6].val_s, \
	  																					argS->arg[7].val_s, \
																						&sda_app_con
	  																				);
	  result->type = 0;
	  return 1;
	}

	//#!#### Destructors

	//#!    pDestroy([num]id);
	//#!Destroys element with given id.
	//#!Return: none
	if (sysFuncMatch(argS->callId, "pDestroy", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
	  pscg_destroy((uint16_t) argS->arg[1].val_s, &sda_app_con);
	  return 1;
	}

	//#!
	//#!    pDestroyScr([num]scrId);
	//#!Destroy screen with given id (with all its sub-screens). To be removed.
	//#!Return: None
	if (sysFuncMatch(argS->callId,"pDestroyScr",s)){
		argType[1]=0;
		if(sysExecTypeCheck(argS, argType, 1,s)){
		  return 0;
		}
	  pscg_destroy_screen((uint16_t) argS->arg[1].val_s, &sda_app_con);
	  return 1;
	}


	//#!#### Getters & setters

	//#!##### Value
	//#!    pGetValue([num]Id);
	//#!Gets value of pscg item.
	//#!Return: [num]value
	if (sysFuncMatch(argS->callId,"pGetValue",s)){
		argType[1] = 0; //id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_get_value(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetValue([num]Id, [num]value);
	//#!Sets value of pscg item.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetValue", s)) {
		argType[1] = 0; //id
		argType[2] = 0; //val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_value(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!##### Modified flag
	//#!    pSetModif([num]Id);
	//#!Sets modified flag of an element.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetModif", s)) {
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  pscg_set_modified(argS->arg[1].val_s, &sda_app_con);
	  return 1;
	}

	//#!##### Parameter
	//#!    pGetParam([num]Id);
	//#!Gets elements parameter value.
	//#!Return: [num]param
	if (sysFuncMatch(argS->callId, "pGetParam", s)) {
		argType[1] = 0; //id
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
	  result->value.val_s = pscg_get_param(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetParam([num]Id, [num]value);
	//#!Sets elements parameter value.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetParam", s)) {
		argType[1] = 0; //id
		argType[2] = 0; //val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_param(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

	  return 1;
	}

	//#!##### Events
	//#!    pGetEvent([num]Id);
	//#!Gets event from an element.
	//#!Return: [num] event value, one of EV_ defines.
	if (sysFuncMatch(argS->callId, "pGetEvent", s)) {
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
	  uint16_t ret = 0;
	  if (pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_PRESSED) {
		  ret = 1;
	  } else if(pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_HOLD) {
		  ret = 2;
	  } else if(pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_RELEASED) {
		  ret = 3;
	  } else if(pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_DRAGOUT) {
		  ret = 4;
	  }
	  result->value.val_s = ret;
	  result->type = 0;
	  return 1;
	}

	//#!
	//#!    pGetEventC([num]Id);
	//#!Gets event from an element and clears the event.
	//#!Return: [num] event value, one of EV_ defines.
	if (sysFuncMatch(argS->callId, "pGetEventC", s)) {
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
	  uint16_t ret = 0;
	  if (pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_PRESSED) {
		  ret = 1;
	  } else if(pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_HOLD) {
		  ret = 2;
	  } else if(pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_RELEASED) {
		  ret = 3;
	  } else if(pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_DRAGOUT) {
		  ret = 4;
	  }
	  result->value.val_s = ret;
	  result->type = 0;

	  pscg_set_event(argS->arg[1].val_s, EV_NONE, &sda_app_con);

	  return 1;
	}

	//#!
	//#!    pSetEvent([num]Id, [num] event);
	//#!Sets event of an element.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetEvent", s)) {
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

	  pscg_set_event(argS->arg[1].val_s, ev, &sda_app_con);
	  return 1;
	}

	//#!
	//#!    pClrScrEv([num]Id);
	//#!Clears event for whole screen and its sub-screens.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pClrScrEv", s)) {
	  argType[1] = 0; //id

	  if(sysExecTypeCheck(argS, argType, 1, s)) {
	    return 0;
	  }
	  pscg_clear_screen_ev((uint16_t)argS->arg[1].val_s, &sda_app_con);
	  return 1;
	}

	//#!##### Screen
	//#!    pSetScreen([num]Id, [num]screenId);
	//#!Sets element screen parameter.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetScreen", s)) {
		argType[1] = 0; //id
		argType[2] = 0; //screenId

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_screen(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!##### Grayout
	//#!    pGetGrayout([num]Id);
	//#!Gets element grayout.
	//#!Return: [num] grayout, 1 if element is grey, 0 if it is normal
	if (sysFuncMatch(argS->callId, "pGetGrayout", s)) {
		argType[1] = 0; //id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_get_grayout(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetGrayout([num]Id, [num]grayout);
	//#!Sets element grayout.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetGrayout", s)) {
		argType[1] = 0; //id
		argType[2] = 0; //val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}
	  pscg_set_grayout(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!##### Visibility
	//#!    pGetVisible([num]Id);
	//#!Gets element visibility.
	//#!Return: [num] visibility
	if (sysFuncMatch(argS->callId, "pGetVisible", s)) {
		argType[1] = 0; //id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

		result->value.val_s = pscg_get_visible(argS->arg[1].val_s, &sda_app_con);
		result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetVisible([num]Id, [num]visibility);
	//#!Sets element visibility.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetVisible", s)) {
		argType[1] = 0; //id
		argType[2] = 0; //val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_visible(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!##### Ghost buttons
	//#!    pGetGhost([num]Id);
	//#!Gets element ghost parameter.
	//#!Return: [num] isGhost
	if (sysFuncMatch(argS->callId, "pGetGhost", s)) {
		argType[1] = 0; //id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

		result->value.val_s = pscg_get_ghost(argS->arg[1].val_s, &sda_app_con);
		result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetGhost([num]Id, [num]ghost);
	//#!Sets element ghost parameter.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetGhost", s)) {
		argType[1] = 0; //id
		argType[2] = 0; //val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_ghost(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!##### Selected buttons
	//#!    pGetSelect([num]Id);
	//#!Gets element selected parameter.
	//#!Return: [num] isSelected
	if (sysFuncMatch(argS->callId, "pGetSelect", s)) {
		argType[1] = 0; //id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

		result->value.val_s = pscg_get_select(argS->arg[1].val_s, &sda_app_con);
		result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetSelect([num]Id, [num]select);
	//#!Sets element select parameter.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetSelect", s)) {
		argType[1] = 0; //id
		argType[2] = 0; //val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_select(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!##### String parameter
	//#!    pGetStr([num]Id);
	//#!Gets element value_str parameter.
	//#!Return: [str]str
	if (sysFuncMatch(argS->callId, "pGetStr", s)) {
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  result->value.val_str = strNew(pscg_get_str(argS->arg[1].val_s, &sda_app_con), s);
	  result->type = 1;
	  return 1;
	}

	//#!
	//#!    pSetStr([num]Id, [str]str);
	//#!Sets element value_str parameter.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetStr", s)) {
		argType[1] = 0; // id
		argType[2] = 1; // str

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_str(argS->arg[1].val_s, s->stringField + argS->arg[2].val_str, &sda_app_con);

	  return 1;
	}

	//#!
	//#!    pSetTxtSize([num]Id, [num]val);
	//#!Sets size of text inside buttons or text fields.
	//#!Possible values are those used by LCD_Set_Sys_Font
	//#!By default they are: 18, 32, 70, 87
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetTxtSize", s)) {
		argType[1] = 0; // id
		argType[2] = 0; // val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}
	  pscg_text_set_size(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

	  return 1;
	}

	//#!##### Size and placement
	//#!
	//#!    pSetRelInit([num]val);
	//#!Sets aplication gr2 context to relative init mode.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetRelInit", s)) {
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  pscg_set_relative_init(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetXYXY([num]Id, [num] x1, [num] y1, [num] x2, [num] y2);
	//#!Sets position of element inside screen.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetXYXY", s)) {
		argType[1] = 0; // id
		argType[2] = 0; // x1
		argType[3] = 0; // y1
		argType[4] = 0; // x2
		argType[5] = 0; // y2

		if(sysExecTypeCheck(argS, argType, 5, s)) {
		  return 0;
		}
	  pscg_set_x1y1x2y2( \
	  									argS->arg[1].val_s, \
	  									argS->arg[2].val_s, \
	  									argS->arg[3].val_s, \
	  									argS->arg[4].val_s, \
	  									argS->arg[5].val_s,
										&sda_app_con
	  									);
	  return 1;
	}

	//#!
	//#!    pSetSpacing([num]Id, [num] left, [num] right, [num] top, [num] bottom);
	//#!Sets element spacing atributes of given screen.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetSpacing", s)) {
		argType[1] = 0; // id
		argType[2] = 0; // left
		argType[3] = 0; // right
		argType[4] = 0; // top
		argType[5] = 0; // bottom

		if(sysExecTypeCheck(argS, argType, 5, s)) {
		  return 0;
		}
		pscg_set_cell_space_left(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  pscg_set_cell_space_right(argS->arg[1].val_s, argS->arg[3].val_s, &sda_app_con);
	  pscg_set_cell_space_top(argS->arg[1].val_s, argS->arg[4].val_s, &sda_app_con);
	  pscg_set_cell_space_bottom(argS->arg[1].val_s, argS->arg[5].val_s, &sda_app_con);
	  return 1;
	}

	//#!
	//#!    pGetXcell([num]screenId);
	//#!Gets screen Xcell parameter.
	//#!Return: [num] Xcell
	if (sysFuncMatch(argS->callId, "pGetXcell", s)) {
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_get_x_cell(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetXcell([num]screenId, [num] Xcell);
	//#!Sets screen Xcell parameter. (32 by default)
	//#!Return: None
	if (sysFuncMatch(argS->callId,"pSetXcell",s)) {
		argType[1] = 0; //id
		argType[2] = 0; //val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_x_cell(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

	  return 1;
	}

	//#!
	//#!    pGetYcell([num]screenId);
	//#!Gets screen Ycell parameter.
	//#!Return: [num] Ycell
	if (sysFuncMatch(argS->callId, "pGetYcell", s)){
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_get_y_cell(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetYcell([num]screenId, [num] Ycell);
	//#!Sets screen Ycell parameter. (32 by default)
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetYcell", s)) {
		argType[1] = 0; // id
		argType[2] = 0; // val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_y_cell(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!
	//#!    pGetXscroll([num]screenId);
	//#!Gets screen Xscroll.
	//#!Return: [num] Xscroll
	if (sysFuncMatch(argS->callId, "pGetXscroll", s)) {
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_get_xscroll(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetXscroll([num]screenId, [num]Xscroll);
	//#!Sets Xscroll parameter.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetXscroll", s)) {
		argType[1] = 0; //id
		argType[2] = 0; //val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_xscroll(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

	  return 1;
	}

	//#!
	//#!    pGetYscroll([num]screenId);
	//#!Gets Yscroll parameter.
	//#!Return: [num] Yscroll
	if (sysFuncMatch(argS->callId, "pGetYscroll", s)) {
		argType[1] = 0; //id

		if(sysExecTypeCheck(argS, argType, 1,s)){
		  return 0;
		}

	  result->value.val_s = pscg_get_yscroll(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;
	  return 1;
	}

	//#!
	//#!    pSetYscroll([num]screenId, [num]Yscroll);
	//#!Sets Yscroll parameter.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetYscroll", s)) {
		argType[1] = 0; // id
		argType[2] = 0; // val

		if(sysExecTypeCheck(argS, argType, 2,s)) {
		  return 0;
		}

	  pscg_set_yscroll(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!##### Fonts & texts
	//#!    pSetDFont([num]screenId, [num] val);
	//#!Sets defalt screen font.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetDFont", s)) {
		argType[1] = 0; // id
		argType[2] = 0; // val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_set_default_font(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!
	//#!    pGetTexAct([num]Id);
	//#!Gets if given editable text field is currently active.
	//#!Return: [num]isActive
	if (sysFuncMatch(argS->callId, "pGetTexAct", s)) {
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  result->value.val_s = pscg_get_text_active(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;

	  return 1;
	}

	//#!
	//#!    pSetTexAct([num]Id);
	//#!Sets given editable text field as currently active.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetTexAct", s)) {
		argType[1] = 0; // id

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  pscg_activate_text(argS->arg[1].val_s, &sda_app_con);
	  result->type = 0;

	  return 1;
	}

	//#!
	//#!    pSetTexFit([num]Id, [num]val);
	//#!Sets automatic line-breaking on.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetTexFit", s)) {
		argType[1] = 0; // id
		argType[2] = 0; // val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_text_set_fit(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);
	  return 1;
	}

	//#!
	//#!    pSetTexEd([num]Id, [num]val);
	//#!Sets text fiels as editable.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetTexEd", s)) {
		argType[1] = 0; // id
		argType[2] = 0; // val

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

	  pscg_text_set_editable(argS->arg[1].val_s, argS->arg[2].val_s, &sda_app_con);

	  return 1;
	}

	//#!##### Colours
	//#!
	//#!    pSetColor([num]Col, [num]val);
	//#!Sets given color to given value.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetColor", s)) {
		argType[1] = 0;
		argType[2] = 0;

		if(sysExecTypeCheck(argS, argType, 2, s)) {
		  return 0;
		}

		if (argS->arg[1].val_s == 1) {
			pscg_set_border_color(argS->arg[2].val_s, &sda_app_con);
		} else if (argS->arg[1].val_s == 2) {
			pscg_set_text_color(argS->arg[2].val_s, &sda_app_con);
		} else if (argS->arg[1].val_s == 3) {
			pscg_set_background_color(argS->arg[2].val_s, &sda_app_con);
		} else if (argS->arg[1].val_s == 4) {
			pscg_set_fill_color(argS->arg[2].val_s, &sda_app_con);
		} else if (argS->arg[1].val_s == 5) {
			pscg_set_active_color(argS->arg[2].val_s, &sda_app_con);
		}

	  return 1;
	}

	//#!
	//#!    pGetColor([num]Col);
	//#!Gets value of given color.
	//#!Return: [num]val
	if (sysFuncMatch(argS->callId, "pGetColor", s)) {
		argType[1] = 0;

		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

		if (argS->arg[1].val_s == 1) {
			result->value.val_s = pscg_get_border_color(&sda_app_con);
		} else if (argS->arg[1].val_s == 2) {
			result->value.val_s = pscg_get_text_color(&sda_app_con);
		} else if (argS->arg[1].val_s == 3) {
			result->value.val_s = pscg_get_background_color(&sda_app_con);
		} else if (argS->arg[1].val_s == 4) {
			result->value.val_s = pscg_get_fill_color(&sda_app_con);
		} else if (argS->arg[1].val_s == 5) {
			result->value.val_s = pscg_get_active_color(&sda_app_con);
		}
		result->type = 0;
	  return 1;
	}

	return 0;
}

