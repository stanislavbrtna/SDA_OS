/*
Copyright (c) 2018 Stanislav Brtna

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

#include "sda_wrapper.h"

uint8_t sda_overlay_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
	uint8_t argType[11];

	//#!#### Overlay API

	//#!##### Set overlay screen
	//#!    sys pSetOvrScr([num]screen_id);
	//#!Sets overlay screen, returns overlay id.
	//#!Return: [num] Overlay id
	if (sysFuncMatch(argS->callId, "pSetOvrScr", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
	  result->value.val_s = setOverlayScreen((uint16_t) argS->arg[1].val_s, &sda_app_con);
		result->type = 0;
	  return 1;
	}

	//#!##### Get overlay id
	//#!    sys pGetOvrId();
	//#!Gets id of current overlay.
	//#!Return: [num] OverlayId
	if (sysFuncMatch(argS->callId, "pGetOvrId", s)) {
		if(sysExecTypeCheck(argS, argType, 0, s)) {
		  return 0;
		}
	  result->value.val_s =  getOverlayId();
		result->type = 0;
	  return 1;
	}

	//#!##### Destroy overlay
	//#!    sys pOvrDestroy();
	//#!Destroys current overlay, also destroys its screen.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pOvrDestroy", s)) {
		if(sysExecTypeCheck(argS, argType, 0, s)) {
		  return 0;
		}
	  destroyOverlay();
	  setRedrawFlag();
		result->type = 0;
	  return 1;
	}

	//#!##### Set position and size of overlay
	//#!    sys pOvrSetXYXY([num]x1, [num]y1, [num]x2, [num]y2);
	//#!Sets position and size of current overlay.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pOvrSetXYXY", s)) {
		argType[1] = 0;
		argType[2] = 0;
		argType[3] = 0;
		argType[4] = 0;
		if(sysExecTypeCheck(argS, argType, 4, s)) {
		  return 0;
		}
	  setOverlayX1(argS->arg[1].val_s);
	  setOverlayY1(argS->arg[2].val_s);
	  setOverlayX2(argS->arg[3].val_s);
	  setOverlayY2(argS->arg[4].val_s);
		result->type = 0;
	  return 1;
	}


	//#!##### Set overlay height
	//#!    sys pSetOvrY( val); set Overlay Y2
	//#!Sets lower coordinates of current overlay.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "pSetOvrY", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  setOverlayY2((uint16_t) argS->arg[1].val_s );
	  return 1;
	}

	//#!
	//#!#### Date overlay
	//#!

	//#!##### Create date overlay
	//#!    sys oDateAdd([num]year, [num]month, [num]day); #return overlay id
	//#!Creates date overlay id, returns id
	//#!Return: [num]Date overlay id
	if (sysFuncMatch(argS->callId, "oDateAdd", s)) {
		argType[1] = 0;
		argType[2] = 0;
		argType[3] = 0;

		if(sysExecTypeCheck(argS, argType, 3, s)) {
		  return 0;
		}

		result->value.val_s
			= date_overlay_init(
				(uint16_t)argS->arg[1].val_s,
				(uint16_t)argS->arg[2].val_s,
				(uint16_t)argS->arg[3].val_s
			);
		result->type = 0;
	  return 1;
	}

	//#!##### Update date overlay
	//#!    sys oDateUpd([num]id);
	//#!Updates date overlay
	//#!Return: None
	if (sysFuncMatch(argS->callId, "oDateUpd", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
		date_overlay_update((uint16_t)argS->arg[1].val_s);
		result->type = 0;
	  return 1;
	}

	//#!##### Get ok from overlay
	//#!    sys oDateGetOk([num]id); #return if ok
	//#!Gets if overlay ok button was pressed.
	//#!Return: 1 if overlay ok button was pressed
	if (sysFuncMatch(argS->callId, "oDateGetOk", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
		result->value.val_s = date_overlay_get_ok((uint16_t) argS->arg[1].val_s);
		result->type=0;
	  return 1;
	}

	//#!##### Clear ok from overlay
	//#!    sys oDateClrOk([num]id);
	//#!Clears ok flag from overlay
	//#!Return: None
	if (sysFuncMatch(argS->callId, "oDateClrOk", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
		date_overlay_clear_ok((uint16_t) argS->arg[1].val_s);
		result->type = 0;
	  return 1;
	}

	//#!##### Get year form overlay
	//#!    sys oDateGetYr([num]id); #return year
	//#!Gets year from overlay with given id.
	//#!Return: [num]Year
	if (sysFuncMatch(argS->callId, "oDateGetYr", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
		result->value.val_s = date_overlay_get_year((uint16_t)argS->arg[1].val_s);
		result->type = 0;
	  return 1;
	}

	//#!##### Get day from overlay
	//#!    sys oDateGetDay([num]id);
	//#!Gets day from overlay with given id.
	//#!Return: [num]Day
	if (sysFuncMatch(argS->callId, "oDateGetDay", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
		result->value.val_s = date_overlay_get_day((uint16_t)argS->arg[1].val_s);
		result->type = 0;
	  return 1;
	}

	//#!##### Get month from overlay
	//#!    sys oDateGetMon([num]id);
	//#!Gets month from overlay with given id.
	//#!Return: [num]Month
	if (sysFuncMatch(argS->callId, "oDateGetMon", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
		result->value.val_s = date_overlay_get_month((uint16_t)argS->arg[1].val_s);
		result->type = 0;
	  return 1;
	}

	//#!
	//#!#### Time overlay
	//#!

	//#!##### Create time overlay
	//#!    sys oTimAdd();
	//#!Adds a time overlay, returns it's id
	//#!Return: [num]overlay id
	if (sysFuncMatch(argS->callId, "oTimAdd", s)) {

		if(sysExecTypeCheck(argS, argType, 0, s)) {
		  return 0;
		}

		result->value.val_s = time_overlay_init();
		result->type = 0;
	  return 1;
	}

	//#!##### Set time overlay time
	//#!    sys oTimSet([num]overlay_id, [num]hr, [num]min);
	//#!Sets time in overlay with given id.
	//#!Return: None
	if (sysFuncMatch(argS->callId, "oTimSet", s)) {
		argType[1] = 0;
		argType[2] = 0;
		argType[3] = 0;
		if(sysExecTypeCheck(argS, argType, 3, s)) {
		  return 0;
		}
		time_overlay_set_time(
			(uint16_t)argS->arg[1].val_s,
			(uint16_t)argS->arg[2].val_s,
			(uint16_t)argS->arg[3].val_s
		);
	  return 1;
	}

	//#!##### Update time overlay
	//#!    sys oTimUpd([num]overlay_id);
	//#!Updates time overlay.
	//#!Return: None
	if (sysFuncMatch(argS->callId,"oTimUpd",s)){
		argType[1]=0;
		if(sysExecTypeCheck(argS, argType, 1,s)){
		  return 0;
		}

	  time_overlay_update((uint16_t) argS->arg[1].val_s );
	  return 1;
	}

	//#!##### Get time overlay ok
	//#!    sys oTimGetOk([num]overlay_id);
	//#!Gets ok from time overlay.
	//#!Return: 1 if ok was pressed
	if (sysFuncMatch(argS->callId, "oTimGetOk", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  result->value.val_s = time_overlay_get_ok((uint16_t)argS->arg[1].val_s);
	  result->type = 0;
	  return 1;
	}

	//#!##### Get time overlay minutes
	//#!    sys oTimGetMin([num]overlay_id);
	//#!Returns minutes from overlay.
	//#!Return: [num]Minutes
	if (sysFuncMatch(argS->callId, "oTimGetMin", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}
	  result->value.val_s = time_overlay_get_minutes((uint16_t)argS->arg[1].val_s);
	  result->type = 0;
	  return 1;
	}

	//#!##### Get time overlay hours
	//#!    sys oTimGetHr([num]overlay_id);
	//#!Returns hours from overlay.
	//#!Return: [num]Hours
	if (sysFuncMatch(argS->callId, "oTimGetHr", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  result->value.val_s = time_overlay_get_hours((uint16_t)argS->arg[1].val_s);
	  result->type = 0;
	  return 1;
	}

	//#!##### Clear ok
	//#!    sys oTimClrOk([num]overlay_id);
	//#!Clears ok from time overlay
	//#!Return: None
	if (sysFuncMatch(argS->callId, "oTimClrOk", s)) {
		argType[1] = 0;
		if(sysExecTypeCheck(argS, argType, 1, s)) {
		  return 0;
		}

	  time_overlay_clear_ok((uint16_t)argS->arg[1].val_s);
	  return 1;
	}

	return 0;
}
