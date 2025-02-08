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

//#!  Automatically generated documentation of SDA OS system functions

//#!
//#! - [OS Gui](sda_os_gui.md)
//#! - [Sound](sda_os_sound.md)
//#! - [Callendar widget](sda_os_widgets.md)
//#! - [Text obfuscation](sda_os_crypto.md)
//#!

//#!### Basic Application structure
//#!
//#!    function init {
//#!      screen = sys.gui.addScreen();
//#!      sys.gui.addText(1, 1, 2, 6, "Hello, world!", screen);
//#!      sys.os.gui.setMainScr(screen);
//#!    }
//#!    
//#!    function update {
//#!      # empty in this example
//#!    }
//#!
//#!#### Required functions
//#!Each application must implement all required functions.
//#!##### Init function
//#!    function init {}
//#!Init is called once when the app is loaded.
//#!arg0 - arg2: call arguments passed from sys.os.subProcess
//#!
//#!##### Update function
//#!    function update
//#!Update function is called each update cycle, when the app
//#!is active and in the foreground.
//#!
//#!#### Optional functions
//#!User might also implement optional functions. OS will call these functions under circumstances defined below.
//#!
//#!##### Exit function
//#!    function exit {}
//#!Exit function is called upon exiting the app.  
//#!
//#!##### Suspend function
//#!    function suspend {}
//#!Suspend function is called when the app is suspended.
//#!
//#!##### Wakeup function
//#!    function wakeup {}
//#!Wakeup is called when the app is woken from suspend state.
//#!If the application process was set as a singular, and the app was woken from sys.os.subProcess call,
//#!init arguments are passed to the wakeup function.
//#!

//#!#### SDA API Level history
//#!
//#!API level given by *sys.os.getVer* etc. works like this: 
//#!10510 - SDA version 1.5.1 with all its features
//#!
//#!Note: before 1.4 sub-version used only one digit
//#!1120 - SDA version 1.1.2 with all its features
//#!

//#!#### Constants
//#!| Constant     | Description      |
//#!|   ---        |    ---           |
//#!| SVP_LANG_CZ  | Czech language   |
//#!| SVP_LANG_ENG | English language |
//#!

#include "sda_os_wrapper.h"

uint8_t sda_os_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_sound_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_hw_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_counter_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_crypto_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_time_alarm_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_time_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_overlay_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_overlay_time_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_overlay_date_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_gui_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_cal_widget_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_hw_buttons_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_os_hw_com_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_settings_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_overlay_color_wrapper(varRetVal *result, argStruct *argS, svsVM *s);


svsConstType svsWrapConsts[] = {
  {"SVP_LANG_CZ",    0},
  {"SVP_LANG_ENG",   1},
  {"LED_ON",         1},
  {"LED_OFF",        0},
  {"LED_BLINK",      2},
  {"LED_SHORTBLINK", 3},
  {"LED_ALARM",      4},

  {"BTN_A",     BUTTON_A},
  {"BTN_LEFT",  BUTTON_LEFT},
  {"BTN_UP",    BUTTON_UP},
  {"BTN_DOWN",  BUTTON_DOWN},
  {"BTN_RIGHT", BUTTON_RIGHT},
  {"BTN_B",     BUTTON_B},

  {"PIN_IN",  SDA_BASE_PIN_IN},
  {"PIN_OUT", SDA_BASE_PIN_OUT},
  {"PIN_ALT", SDA_BASE_PIN_ALT},

  {"PIN_NOPULL",   SDA_BASE_PIN_NOPULL},
  {"PIN_PULLDOWN", SDA_BASE_PIN_PULLDOWN},
  {"PIN_PULLUP",   SDA_BASE_PIN_PULLUP},

  {"EXT_EXP_PORT", EXTERNAL_EXPANSION_PORT},
  {"INT_EXP_PORT", INTERNAL_EXPANSION_PORT},
  {"SERIAL_PORT",  SERIAL_PORT},
  {"USB_PORT",     USB_PORT},

  {"ICON_NONE", SDA_ICON_NONE},
  {"ICON_BACK", SDA_ICON_BACK},
  {"ICON_FORWARD", SDA_ICON_FORWARD},

  {"end", 0}
};


void sda_svs_wrapper_init() {
  addSysConsts(svsWrapConsts);
  addSysWrapper(sda_os_wrapper, (uint8_t *)"os");
  addSysWrapper(sda_os_sound_wrapper, (uint8_t *)"snd");
  addSysWrapper(sda_os_crypto_wrapper, (uint8_t *)"cr");
  addSysWrapper(sda_os_hw_wrapper, (uint8_t *)"hw");
  addSysWrapper(sda_os_hw_buttons_wrapper, (uint8_t *)"hw.btn");
  addSysWrapper(sda_os_hw_com_wrapper, (uint8_t *)"com");
  addSysWrapper(sda_counter_wrapper, (uint8_t *)"cnt");
  addSysWrapper(sda_time_sub_wrapper, (uint8_t *)"time");
  addSysWrapper(sda_overlay_sub_wrapper, (uint8_t *)"o");
  addSysWrapper(sda_overlay_time_wrapper, (uint8_t *)"o.time");
  addSysWrapper(sda_overlay_date_wrapper, (uint8_t *)"o.date");
  addSysWrapper(sda_overlay_color_wrapper, (uint8_t *)"o.color");
  addSysWrapper(sda_time_alarm_wrapper, (uint8_t *)"alarm");
  addSysWrapper(sda_os_gui_wrapper, (uint8_t *)"os.gui");
  addSysWrapper(sda_os_cal_widget_wrapper, (uint8_t *)"w.cal");
  addSysWrapper(sda_settings_wrapper, (uint8_t *)"os.settings");
}


uint8_t sda_os_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];
  result->value.val_u = 0;
  result->type = SVS_TYPE_NUM;

  //#!#### Main OS functions

  //#!##### Get redraw flag
  //#!    sys.os.getRedraw();
  //#!Gets redraw flag. *getRedraw* also works.
  //#!
  //#!Return: [num] 1 if redraw flag is set, otherwise 0
  if (sysFuncMatch(argS->callId, "getRedraw", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = sdaGetRedrawDetect();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set redraw
  //#!    sys.os.setRedraw();
  //#!Sets redraw flag
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setRedraw", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    setRedrawFlag();
    return 1;
  }

  //#!##### Wake the SDA from sleep
  //#!    sys.os.wake();
  //#!Wakes SDA without turning the screen on.
  //#!SDA will wake in the low power mode and will sleep again after the lcd shutdown time.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "wake", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sda_interrupt_sleep();
    return 1;
  }

  //#!##### Pushes app to foreground
  //#!    sys.os.arise();
  //#!If called from timer or uart callback, the app is promoted to the foreground.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "arise", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sdaSvmSetAriseFlag();
    return 1;
  }

  //#!##### Disable app close
  //#!    sys.os.noClose([num] enabled);
  //#!If enabled, running application is only suspended
  //#!when close button is pressed.
  //#!Application can still be closed from task manager.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "noClose", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    svmSetSuspendOnClose(argS->arg[1].val_s);
      
    return 1;
  }

  //#!##### Get if running in simulator
  //#!    sys.os.inSim();
  //#!Gets if app is running in simulator. 1 - Simulator, 0 - Real hardware.
  //#!
  //#!Return: [num] result
  if (sysFuncMatch(argS->callId, "inSim", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    #ifdef PLATFORM_PC
    result->value.val_s = 1;
    #else
    result->value.val_s = 0;
    #endif
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Show Error
  //#!    sys.os.error([str]errorText);
  //#!Throws error message
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "error", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    sda_show_error_message(s->stringField + argS->arg[1].val_str);
    return 1;
  }

  //#!##### Gets app path
  //#!    sys.os.getAppPath();
  //#!Gets diretory path of the currently running svs app
  //#!includung the name of the app. 
  //#!
  //#!Return: [str] Path
  if (sysFuncMatch(argS->callId, "getAppPath", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = strNew(svmGetName(), s);
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!#### Keyboard

  //#!##### Show keyboard
  //#!    sys.os.showKbd();
  //#!Shows system keyboard
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "showKbd", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sda_keyboard_show();
    return 1;
  }

  //#!##### Hide keyboard
  //#!    sys.os.hideKbd();
  //#!Hides system keyboard.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "hideKbd", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sda_keyboard_hide();
    return 1;
  }

  //#!##### Get Keyboard state
  //#!    sys.os.kbdGetState();
  //#!Gets if keyboard is deployed 1 - keyboard shown, 0 - keyboard hidden
  //#!
  //#!Return: [num] state
  if (sysFuncMatch(argS->callId, "kbdGetState", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = (uint32_t) svpSGlobal.kbdVisible;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!
  //#!#### Misc
  //#!

  //#!##### Get random number
  //#!    sys.os.rnd();
  //#!Returns random number
  //#!
  //#!Return: [num]RandomValue
  if (sysFuncMatch(argS->callId, "rnd", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svp_random();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Suspend app
  //#!    sys.os.suspend();
  //#!Suspends (minimizes) currently running app.
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "suspend", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svmSetSuspendFlag(1);
    return 1;
  }

  //#!##### Exit app
  //#!    sys.os.exit();
  //#!    sys.os.exit([undef] arg0, [undef] arg1, [undef] arg2); # optional return values
  //#!Stops program execution after exiting *update* function and performing *exit* function.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "exit", s)) {
    if(argS->usedup == 0) {
      if(sysExecTypeCheck(argS, argType, 0, s)) {
        return 0;
      }
    } else {
      argType[1] = SVS_TYPE_UNDEF;
      argType[2] = SVS_TYPE_UNDEF;
      argType[3] = SVS_TYPE_UNDEF;
      if(sysExecTypeCheck(argS, argType, 3, s)) {
        return 0;
      }

      svmSetSubProcRetval(
        argS->arg[1], argS->argType[1],
        argS->arg[2], argS->argType[2],
        argS->arg[3], argS->argType[3]
      );
    }

    svpSGlobal.systemXBtnClick = 1;
    return 1;
  }

  //#!##### Check API level
  //#!    sys.os.checkVer([num] API_Level);
  //#!Checks for API Lvl support.
  //#!If host level is below given API_Level, error is thrown and app is terminated.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "checkVer", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (argS->arg[1].val_s > SDA_OS_VERSION_NUM) {
      errSoft((uint8_t *)"checkSVSVer: Application needs higher level of SDA_os API.", s);
      errSoftSetParam((uint8_t *)"Needed", (varType)argS->arg[1].val_s, s);
      errSoftSetParam((uint8_t *)"Got", (varType)(uint16_t)SDA_OS_VERSION_NUM, s);
      return 1;
    }
    return 1;
  }

  //#!##### Get API level
  //#!    sys.os.getVer();
  //#!Checks for API Lvl support.
  //#!
  //#!Return: [num] SDA_OS version number
  if (sysFuncMatch(argS->callId, "getVer", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_u = (uint16_t) SDA_OS_VERSION_NUM;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get system language
  //#!    sys.os.getLang();
  //#!Returns language of the running SDA_OS build.
  //#!
  //#!Return: defines SVP_LANG_CZ (0) or SVP_LANG_ENG(1)
  if (sysFuncMatch(argS->callId, "getLang", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_u = (uint16_t) LANG_VAL;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!#### Subprocess

  //#!##### Set process as singular
  //#!    sys.os.setSingular();
  //#!Sets current process as singular.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setSingular", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svmSetSingular(svmGetPid());
    return 1;
  }

  //#!##### Launch subprocess
  //#!    sys.os.subProcess([str]fileName, [str/ref] callback, [undef] arg0, [undef] arg1, [undef] arg2);
  //#!Runs child process with given arguments. 
  //#!*fileName* must contain valid path to .svs file located in APPS directory.
  //#!*callback* stores name of a function that will be called after child process exits.
  //#!Subprocess will be launched after the current application returns from its *update* function.
  //#!When strings are passed as arguments,
  //#!their total size must not exceed APP_ARG_STR_LEN define (2048 by default).
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "subProcess", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    argType[3] = SVS_TYPE_UNDEF;
    argType[4] = SVS_TYPE_UNDEF;
    argType[5] = SVS_TYPE_UNDEF;
    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }
    gr2_text_deactivate(&sda_app_con);
    svmCallSubProc(
        s->stringField + argS->arg[1].val_str,
        s->stringField + argS->arg[2].val_str,
        argS->arg[3], argS->argType[3],
        argS->arg[4], argS->argType[4],
        argS->arg[5], argS->argType[5]
    );
    return 1;
  }

  //#!##### Enable launching subprocess from cwd
  //#!    sys.os.subProcCWD([num] val);
  //#!Sets if subprocesses are launched from cwd or from APPS folder.
  //#!
  //#!val:
  //#! | Value: | Description            |
  //#! | ---    | ---                    |
  //#! |    0   | APPS folder (default)  |
  //#! |    1   | CWD                    |
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "subProcCWD", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svmSetLaunchCWDflag(argS->arg[1].val_s);
    return 1;
  }

  //#!##### Disable caching for launched subprocess
  //#!    sys.os.subProcNC();
  //#!Disables caching for next call of sys.os.subProcess.
  //#!Usefull when running modified content and precaching on launch is enabled
  //#!in settings.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "subProcNC", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svmSetNocacheFlag();
    return 1;
  }

  //#!##### Return data to parent process
  //#!    sys.os.subRetval([undef] arg0, [undef] arg1, [undef] arg2);
  //#!Sets values that will be returned to parent process
  //#!When strings are passed as arguments,
  //#!their total size must not exceed APP_ARG_STR_LEN define (2048 by default).
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "subRetval", s)) {
    argType[1] = SVS_TYPE_UNDEF;
    argType[2] = SVS_TYPE_UNDEF;
    argType[3] = SVS_TYPE_UNDEF;
    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }
    svmSetSubProcRetval(
        argS->arg[1], argS->argType[1],
        argS->arg[2], argS->argType[2],
        argS->arg[3], argS->argType[3]
    );
    return 1;
  }

  //#!##### Sets the clipboard string
  //#!    sys.os.setClipboard([str] string);
  //#!Sets the OS clipboard 256 chars by default
  //#!
  //#!Return: [num] 1 - ok, 0 - string too long
  if (sysFuncMatch(argS->callId, "setClipboard", s)) {
    argType[1] = SVS_TYPE_STR; // new keyboard string

    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }

    if (sda_strlen(s->stringField + argS->arg[1].val_str) < sizeof(svpSGlobal.clipboard)) {
      sda_strcp(s->stringField + argS->arg[1].val_str, svpSGlobal.clipboard, sizeof(svpSGlobal.clipboard));
      result->value.val_u = 1;
    } else {
      result->value.val_u = 0;
    }
    
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Gets the clipboard string
  //#!    sys.os.getClipboard();
  //#!Gets the OS clipboard 256 chars max by default.
  //#!
  //#!Return: [str] clipboard_string
  if (sysFuncMatch(argS->callId, "getClipboard", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }

    result->value.val_s = strNew(svpSGlobal.clipboard, s);
    result->type = SVS_TYPE_STR;
    return 1;
  }

  return 0;
}


uint8_t sda_settings_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];
  result->value.val_u = 0;
  result->type = SVS_TYPE_NUM;

  //#!#### OS settings functions

  //#!##### Reload homescreen settings
  //#!    sys.os.settings.homeRld();
  //#!Reloads homescreen settings stored in homescreen.cfg
  //#!
  //#!Return: none
  if (sysFuncMatch(argS->callId, "homeRld", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sda_homescreen_configure();

    result->value.val_s = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Requests high privileges
  //#!    sys.os.settings.rqAuth();
  //#!Requests authorization form user to change system settings.
  //#!Result can be retrieved with 'sys.os.settings.getAuth();'
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "rqAuth", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    sda_show_auth_overlay_init();

    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Gets if privileges are granted
  //#!    sys.os.settings.getAuth();
  //#!Gets if high privileges are granted.
  //#!
  //#!Return: [num] 1 if authorization is given
  if (sysFuncMatch(argS->callId, "getAuth", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svmGetAuthorized();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Sets time and date
  //#!    sys.os.settings.setTime([num] year, [num] month, [num] day, [num] hour, [num] min);
  //#!Sets values that will be returned to parent process
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setTime", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;
    argType[5] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }

    if (!svmGetAuthorized()) {
      errSoft((uint8_t *)"setTime: Not authorized!", s);
      return 0;
    }

    sda_set_time(
      (uint16_t) argS->arg[1].val_s,
      (uint8_t) argS->arg[3].val_s,
      svpSGlobal.weekday,
      (uint8_t) argS->arg[2].val_s,
      (uint8_t) argS->arg[4].val_s,
      (uint8_t) argS->arg[5].val_s,
      0
    );
    
    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}
