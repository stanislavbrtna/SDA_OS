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

#include "sda_wrapper.h"

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

//#!  Automatically generated documentation on wrap_umc_svp.c

//#!#### SVP API Level history
//#!
//#!Api level changes:
//#!101 - Fexists, svp lang, led API, BTN defines
//#!102 - pscg_image added to GR2 wrap
//#!103 - pGetEventC added to gr2 wrapper
//#!104 - Timestamp API
//#!105 - cKeyRemove
//#!106 - pscg: pGetTexAct
//#!107 - error
//#!108 - pGetVisible
//#!109 - pSetTexAct,  setSingular(), notification api, pGetMainScr
//#!
//#!- API level overhaul -
//#!660 - SDA version 0.6.6 with all its features
//#! ...
//#!720 - SDA version 0.7.2 with all its features

//#!#### Constants
//#!| Constant | Description |
//#!|   ---    |    ---      |
//#!| SVP_LANG_CZ | Czech language |
//#!| SVP_LANG_ENG | English language |
//#!


svsConstType svsWrapConsts[] = {
  {"SVP_LANG_CZ", 0},
  {"SVP_LANG_ENG", 1},
  {"LED_ON", 1},
  {"LED_OFF", 0},
  {"LED_BLINK", 2},
  {"LED_SHORTBLINK", 3},
  {"LED_ALARM", 4},

  {"BTN_A", BUTTON_A},
  {"BTN_LEFT", BUTTON_LEFT},
  {"BTN_UP", BUTTON_UP},
  {"BTN_DOWN", BUTTON_DOWN},
  {"BTN_RIGHT", BUTTON_RIGHT},
  {"BTN_B", BUTTON_B},

  {"PIN_IN", SDA_BASE_PIN_IN},
  {"PIN_OUT", SDA_BASE_PIN_OUT},
  {"PIN_ALT", SDA_BASE_PIN_ALT},

  {"PIN_NOPULL", SDA_BASE_PIN_NOPULL},
  {"PIN_PULLDOWN", SDA_BASE_PIN_PULLDOWN},
  {"PIN_PULLUP", SDA_BASE_PIN_PULLUP},

  {"end", 0}
};

uint8_t svsSVPWrap(varRetVal *result, argStruct *argS, svsVM *s);

void svsSVPWrapInit() {
  addSysConsts(svsWrapConsts);
  addSysWrapper(svsSVPWrap, (uint8_t *)"os");
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
  addSysWrapper(sda_time_alarm_wrapper, (uint8_t *)"alarm");
  addSysWrapper(sda_os_gui_wrapper, (uint8_t *)"os.gui");
  addSysWrapper(sda_os_cal_widget_wrapper, (uint8_t *)"w.cal");
}


uint8_t svsSVPWrap(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];
  result->value.val_u = 0;
  result->type = SVS_TYPE_NUM;

  //#!#### Main OS functions

  //#!##### Get redraw flag
  //#!    sys.os.getRedraw();
  //#!Gets redraw flag. *getRedraw* also works.
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
  //#!If called from timer callback, the app is promoted to the foreground.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "arise", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sdaSvmSetTimerWkup();
    return 1;
  }

  //#!##### Show Error
  //#!    sys.os.error([str]errorText);
  //#!Throws error message
  //#!Return: None
  if (sysFuncMatch(argS->callId, "error", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    sda_show_error_message(s->stringField + argS->arg[1].val_str);
    return 1;
  }

  //#!##### Get path to executable
  //#!    sys.os.getAppPath();
  //#!Get path to the executable
  //#!Return: [str] Path
  if (sysFuncMatch(argS->callId, "getAppPath", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = strNew(sdaSvmGetName(), s);
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!#### Keyboard

  //#!##### Hide keyboard
  //#!    sys.os.hideKbd();
  //#!Hides system keyboard.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "hideKbd", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sda_keyboard_hide();
    return 1;
  }

  //#!##### Show keyboard
  //#!    sys.os.showKbd();
  //#!Shows system keyboard
  //#!Return: None
  if (sysFuncMatch(argS->callId, "showKbd", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sda_keyboard_show();
    return 1;
  }

  //#!
  //#!#### Misc
  //#!

  //#!##### Get random number
  //#!    sys.os.rnd();
  //#!Returns random number
  //#!Return: [num]RandomValue
  if (sysFuncMatch(argS->callId, "rnd", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = svp_random();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Quit program
  //#!    sys.os.exit();
  //#!Stops program execution after exiting *update* function and performing *exit* function.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "exit", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svpSGlobal.systemXBtnClick = 1;
    return 1;
  }

  //#!##### Check API level
  //#!    sys.os.checkVer([num] API_Level);
  //#!Checks for API Lvl support.
  //#!If host level is below given API_Level, error is thrown and app is terminated.
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
  //#!Returns SDA_OS language.
  //#!Return: 0 if czech, 1 if english, also defines SVP_LANG_CZ SVP_LANG_ENG
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
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setSingular", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svmSetSingular(sdaSvmGetId());
    return 1;
  }

  //#!##### Launch subprocess
  //#!    sys.os.subProcess([str]fileName, [str/ref] callback, [str] arg0, [str] arg1, [str] arg2);
  //#!Runs child process
  //#!Return: None
  if (sysFuncMatch(argS->callId, "subProcess", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    argType[3] = 3;
    argType[4] = 3;
    argType[5] = 3;
    if(sysExecTypeCheck(argS, argType, 5, s)) {
      return 0;
    }
    gr2_text_deactivate(&sda_app_con);
    sdaSvmCall(
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
  //#!val: 0 - APPS folder,1 - cwd
  //#!Return: None
  if (sysFuncMatch(argS->callId, "subProcCWD", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svmSetLaunchCWDflag(argS->arg[1].val_s);
    return 1;
  }

  //#!##### Return data to parent process
  //#!    sys.os.subRetval([str] arg0, [str] arg1, [str] arg2);
  //#!Sets values that will be returned to parent process
  //#!Return: None
  if (sysFuncMatch(argS->callId, "subRetval", s)) {
    argType[1] = 3;
    argType[2] = 3;
    argType[3] = 3;
    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }
    sdaSvmRetval(
        argS->arg[1], argS->argType[1],
        argS->arg[2], argS->argType[2],
        argS->arg[3], argS->argType[3]
    );
    return 1;
  }

  return 0;
}


uint8_t sda_os_sound_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Sound

  //#!##### Beep the speaker
  //#!    sys.snd.beep();
  //#!Initiates system beep.
  //#!Return: None
   if (sysFuncMatch(argS->callId, "beep", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_beep();
    return 1;
  }

  //#!##### Set beep param to default
  //#!    sys.snd.beepDef();
  //#!Sets beep to its default values.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "beepDef", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_beep_set_def();
    return 1;
  }

  //#!##### Set time of beep
  //#!    sys.snd.beepTime([num]time (~ms));
  //#!Sets lenght of beep.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "beepTime", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_beep_set_t(argS->arg[1].val_u);
    return 1;
  }

  //#!##### Set period of beep
  //#!    sys.snd.beepFreq([num]frequency (Hz));
  //#!Sets frequency of the beep in Hz in range from 27 to 20000.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "beepFreq", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_beep_set_pf(argS->arg[1].val_u);
    return 1;
  }

  //#!##### Get if system sound is disabled
  //#!    sys.snd.getMute();
  //#!Returns system mute
  //#!Return: [num]1 if system is on mute.
  if (sysFuncMatch(argS->callId, "getMute", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = svpSGlobal.mute;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
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
     sdaSvmSetMainScreen((uint16_t) argS->arg[1].val_s );
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
     result->value.val_u = sdaSvmGetMainScreen();
     result->type = SVS_TYPE_NUM;
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
      // getting the cursor position
      if (((gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_PRESSED) \
          || (gr2_get_event(argS->arg[1].val_s, &sda_app_con) == EV_HOLD))
          && (gr2_text_get_pwd(argS->arg[1].val_s, &sda_app_con) == 0)) {
        uint16_t temp;
        uint8_t curr_font;
        curr_font = LCD_Get_Font_Size();
        LCD_Set_Sys_Font(gr2_get_param2(argS->arg[1].val_s, &sda_app_con));
  
        temp = LCD_Text_Get_Cursor_Pos(s->stringField + argS->arg[2].val_str, gr2_get_tmx(&sda_app_con), gr2_get_tmy(&sda_app_con));
        
        LCD_Set_Sys_Font(curr_font);

        if (temp == 0) {
          gr2_set_param(argS->arg[1].val_s, 0, &sda_app_con);
        } else {
          gr2_set_param(argS->arg[1].val_s, temp, &sda_app_con);
        }
      }
      gr2_set_event(argS->arg[1].val_s, EV_NONE, &sda_app_con);

      //čtení z klávesnice a zápis do řetězce
      if (sda_get_keyboard_key_flag()) {
        if ((svpSGlobal.kbdKeyStr[0]) != 2) {
          result->value.val_str
            = strInsert(
                        (uint16_t)argS->arg[2].val_str,
                        strNew(svpSGlobal.kbdKeyStr, s),
                        gr2_get_param(argS->arg[1].val_s, &sda_app_con),
                        s
              ); //přičtem char
          result->type = 1;

          //divnost... pokud je to odkomentovaný, tak to prvně nastaví text s indexem +2
          //gr2_set_str(argS->arg[1].val_s,s->stringField+result->value.val_str); //nastavíme správněj text

          if (svpSGlobal.kbdKeyStr[1] != 0) {
            gr2_set_param(argS->arg[1].val_s, gr2_get_param(argS->arg[1].val_s, &sda_app_con) + 2, &sda_app_con); //posunem kurzor
          } else {
            gr2_set_param(argS->arg[1].val_s, gr2_get_param(argS->arg[1].val_s, &sda_app_con) + 1, &sda_app_con);
          }
          return 1;
        } else {
          uint16_t len = 0;

          while(*(s->stringField + argS->arg[2].val_str + len) != 0) {
            len++;
          }

          if (len > 0) {
            uint16_t prac;
            result->value.val_str = strNew(s->stringField + argS->arg[2].val_str, s); //vyrobíme novej
            uint16_t charIndex;
            uint8_t czFlag = 0;

            charIndex = result->value.val_str + gr2_get_param(argS->arg[1].val_s, &sda_app_con);

            if (len >= 2
                 && (s->stringField[charIndex - 2] >= 0xC3)
                 && (s->stringField[charIndex - 2] <= 0xC5)) {
              gr2_set_param(argS->arg[1].val_s, gr2_get_param(argS->arg[1].val_s, &sda_app_con) - 2, &sda_app_con); //posune
              czFlag = 1;
              //printf("removing cz char %c%c \n", s->stringField[charIndex - 2], s->stringField[charIndex - 1]);
            } else {
              gr2_set_param(argS->arg[1].val_s, gr2_get_param(argS->arg[1].val_s, &sda_app_con) - 1, &sda_app_con); //posune
              //printf("removing normal char %c \n", s->stringField[charIndex - 1]);
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
    }
    if (gr2_get_str(argS->arg[1].val_s, &sda_app_con) != s->stringField + argS->arg[2].val_str) {
      gr2_set_str(argS->arg[1].val_s, s->stringField + argS->arg[2].val_str, &sda_app_con); //stejnak nastavíme
    }
    result->value = argS->arg[2];
    result->type = SVS_TYPE_STR;
    return 1;
  }

  return 0;
}


uint8_t sda_os_crypto_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Text obfuscation


  //#!##### Unlock overlay init
  //#!    sys.cr.unLockInit();
  //#!Creates unlock overlay
  //#!Return: [num] overlay ID, 0 when error
  if (sysFuncMatch(argS->callId, "unLockInit", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (svp_crypto_get_lock() == 0) {
      result->value.val_u = password_overlay_init();
    } else {
      result->value.val_u = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Unlock overlay update
  //#!    sys.cr.update([num] ovId);
  //#!Updates unlock overlay
  //#!Return: None
  if (sysFuncMatch(argS->callId, "update", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    password_overlay_update(argS->arg[1].val_u);
    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Unlock overlay get ok
  //#!    sys.cr.getOk([num] ovId);
  //#!Gets if unlock was successfull
  //#!Return: [num] 1 - unlock success, 2 - unlock canceled
  if (sysFuncMatch(argS->callId, "getOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_u = password_overlay_get_ok(argS->arg[1].val_u);
    if (result->value.val_u == 1) {
      sdaSvmSetCryptoUnlock(1);
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Unlock overlay clear ok
  //#!    sys.cr.clrOk([num] ovId);
  //#!Creates unlock overlay
  //#!Return: None
  if (sysFuncMatch(argS->callId, "clrOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    password_overlay_clear_ok(argS->arg[1].val_u);
    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get if is locked
  //#!    sys.cr.getLock();
  //#!Gets if crypto is unlocked
  //#!Return: [num] 1 - crypto unlocked, 0 - crypto locked
  if (sysFuncMatch(argS->callId, "getLock", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = sdaSvmGetCryptoUnlock() && svp_crypto_get_lock();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Loads password as a key
  //#!    sys.cr.loadPass();
  //#!Loads OS password as a key
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "loadPass", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (sdaSvmGetCryptoUnlock()) {
      svp_crypto_set_pass_as_key();
      result->value.val_u = 0;
    } else {
      result->value.val_u = 1;
    }
    
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Load custom key string
  //#!    sys.cr.loadStr([str]key);
  //#!Loads custom string as a crypto key
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "loadStr", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (sdaSvmGetCryptoUnlock()) {
      result->value.val_u = svp_crypto_set_key(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Load custom keyfile
  //#!    sys.cr.loadKey([str]keyfile);
  //#!Loads custom keyfile as a crypto key
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "loadKey", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (sdaSvmGetCryptoUnlock()) {
      result->value.val_u = svp_crypto_load_keyfile(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Load OS keyfile
  //#!    sys.cr.loadOSKey();
  //#!Loads OS keyfile as a crypto key
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "loadOSKey", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (sdaSvmGetCryptoUnlock()) {
      result->value.val_u = svp_crypto_load_os_keyfile();
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Generate keyfile
  //#!    sys.cr.genKey([str]keyfile);
  //#!Generates custom keyfile.
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "genKey", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (sdaSvmGetCryptoUnlock()) {
      result->value.val_u = svp_crypto_generate_keyfile(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Lock
  //#!    sys.cr.lock();
  //#!Locks sda encryption
  //#!Return: None
  if (sysFuncMatch(argS->callId, "lock", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (sdaSvmGetCryptoUnlock()) {
      sdaSvmSetCryptoUnlock(0);
      svp_crypto_lock();
    }
    return 1;
  }

  //#!##### Encrypt file
  //#!    sys.cr.encrypt([str]fname);
  //#!Encrypts file.
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "encrypt", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (sdaSvmGetCryptoUnlock()) {
      result->value.val_u = svp_encrypt(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Decrypt file
  //#!    sys.cr.decrypt([str]fname);
  //#!Encrypts file.
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "decrypt", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (sdaSvmGetCryptoUnlock()) {
      result->value.val_u = svp_decrypt(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}

uint8_t sda_os_cal_widget_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];
  static dateSelectorWidgetType date;

  //#!
  //#!#### Date selector widget
  //#!

  //#!##### Init calendar widget
  //#!    sys.w.cal.init([num]year, [num]month, [num]day) #return: cal screen ID
  //#!Creates callendar widget screen.
  //#!With given year, month and day.
  //#!Return: [num]Callendar widget screen id.
  if (sysFuncMatch(argS->callId, "init", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }
    result->value.val_u = date_select_widget_init(&date, argS->arg[1].val_s, argS->arg[2].val_s, argS->arg[3].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Select date
  //#!    sys.w.cal.select([num]year, [num]month, [num]day);
  //#!Sets year, month and day to callendar widget.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "select", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }
    date_select_widget_set_date(&date, argS->arg[1].val_s, argS->arg[2].val_s, argS->arg[3].val_s);
    return 1;
  }

  //#!##### Update
  //#!    sys.w.cal.update();
  //#!Updates callendar widget.
  //#!Return: [num]1 when callendar is clicked.
  if (sysFuncMatch(argS->callId, "update", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = date_select_widget_update(&date);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Mark day
  //#!    sys.w.cal.mark([num]day);
  //#!Marks day in callendar widget.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "mark", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    date_select_highlight(&date, argS->arg[1].val_s);
    return 1;
  }

  //#!##### Set highlighting
  //#!    sys.w.cal.highlight([num]val);
  //#!Enable that all buttons except marked are rendered as ghost buttons.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "highlight", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    date_select_set_highlight(&date, (uint8_t) argS->arg[1].val_s);
    return 1;
  }

  //#!##### Get selected day
  //#!    sys.w.cal.getDay();
  //#!Returns selected day.
  //#!Return: [num]day
  if (sysFuncMatch(argS->callId, "getDay", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = date_select_widget_get_day(&date);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}

uint8_t sda_counter_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Counters

  //#!##### Set counter
  //#!    sys.cnt.set([num] ms);
  //#!Sets system timer, it counts down and stops at zero.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "set", s)) {
    argType[1] = SVS_TYPE_NUM; //ms
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    set_sda_counter(argS->arg[1].val_s);
    return 1;
  }

  //#!##### Gets counter
  //#!    sys.cnt.get();
  //#!Gets system timer value
  //#!Return: value of system timer
  if (sysFuncMatch(argS->callId, "get", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = get_sda_counter();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}



