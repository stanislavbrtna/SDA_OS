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
//#!| LED_ON | Nonification led on |
//#!| LED_OFF | Nonification led off |
//#!| LED_BLINK | Nonification led pattern |
//#!| LED_SHORTBLINK | Nonification led pattern |
//#!| LED_ALARM | Nonification led pattern |
//#!| BTN_A | Button define |
//#!| BTN_LEFT | Button define |
//#!| BTN_UP | Button define |
//#!| BTN_DOWN | Button define |
//#!| BTN_RIGHT | Button define |
//#!| BTN_B | Button define |
//#!| PIN_IN | Expansion pin setup define |
//#!| PIN_OUT | Expansion pin setup define |
//#!| PIN_ALT | Expansion pin setup define |
//#!| PIN_NOPULL | Expansion pin setup define |
//#!| PIN_PULLUP | Expansion pin setup define |
//#!| PIN_PULLDOWN | Expansion pin setup define |
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

void svsSVPWrapInit(){
  addSysConsts(svsWrapConsts);
  addSysWrapper(svsSVPWrap);
}

static uint8_t wrap_lcdOffButtons;

void wrap_set_lcdOffButtons(uint8_t val) {
  wrap_lcdOffButtons = val;
}

uint8_t wrap_get_lcdOffButtons() {
  return wrap_lcdOffButtons;
}

uint8_t sda_files_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_time_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_overlay_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s);

uint8_t svsSVPWrap(varRetVal *result, argStruct *argS, svsVM *s) {
  uint16_t x;
  uint8_t argType[11];

  uint8_t subWrapperRetVal = 0;

  static dateSelectorWidgetType date;

  result->value.val_u = 0;
   result->type = SVS_TYPE_NUM;

  // call the sda sub-wrappers
  subWrapperRetVal = sda_files_sub_wrapper(result, argS, s);
  if ((subWrapperRetVal == 1)) {
    return 1;
  } else if (errCheck(s)) {
    return 0;
  }

  subWrapperRetVal = sda_time_sub_wrapper(result, argS, s);
  if ((subWrapperRetVal == 1)) {
    return 1;
  } else if (errCheck(s)) {
    return 0;
  }

  subWrapperRetVal = sda_overlay_sub_wrapper(result, argS, s);
  if ((subWrapperRetVal == 1)) {
    return 1;
  } else if (errCheck(s)) {
    return 0;
  }

  //#!#### Gui

  //#!##### Set main application screen
  //#!    sys.pSetMainScr([num]id);
  //#!Sets main screen to id
  //#!Return: None
  if (sysFuncMatch(argS->callId, "pSetMainScr", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }
     sdaSvmSetMainScreen((uint16_t) argS->arg[1].val_s );
    return 1;
  }

  //#!##### Get main application screen
  //#!    sys.pGetMainScr();
  //#!Gets main screen id
  //#!Return: [num]id
  if (sysFuncMatch(argS->callId, "pGetMainScr", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)){
      return 0;
    }
     result->value.val_u = sdaSvmGetMainScreen();
     result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get redraw flag
  //#!    sys.pGetRedraw();
  //#!Gets redraw flag. *getRedraw* also works.
  //#!Return: [num] 1 if redraw flag is set, otherwise 0
  if (sysFuncMatch(argS->callId, "getRadraw", s)
       || sysFuncMatch(argS->callId, "getRedraw", s)
       || sysFuncMatch(argS->callId, "pGetRedraw", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = sdaGetRedrawDetect();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set redraw
  //#!    sys.pSetRedraw();
  //#!Sets redraw flag
  //#!Return: None
  if (sysFuncMatch(argS->callId, "pSetRedraw", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    setRedrawFlag();
    return 1;
  }

  //#!##### Show Error
  //#!    sys.error([str]errorText);
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

  //#!#### Counters

  //#!##### Set counter
  //#!    sys.cntSet([num] ms);
  //#!Sets system timer, it counts down and stops at zero.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "cntSet", s)) {
    argType[1] = SVS_TYPE_NUM; //ms
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    set_svp_counter(argS->arg[1].val_s);
    return 1;
  }

  //#!##### Gets counter
  //#!    sys.cntGet();
  //#!Gets system timer value
  //#!Return: value of system timer
  if (sysFuncMatch(argS->callId, "cntGet", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_s = get_svp_counter();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!#### Buttons
  //#!##### Get button event
  //#!    sys.btnGetEv([num] btn)
  //#!Return last button event
  //#!Return: 0 - none, 1-pressed, 2 - hold, 3 - released
  if (sysFuncMatch(argS->callId, "btnGetEv", s)) {
    gr2EventType ev;
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (argS->arg[1].val_u < 6) {
      ev = sda_wrap_get_button(argS->arg[1].val_u);
    } else {
      ev = EV_NONE;
    }

    if (svpSGlobal.lcdState == LCD_ON || wrap_lcdOffButtons) {
      if (ev == EV_NONE) {
        result->value.val_s = 0;
      }

      if (ev == EV_PRESSED) {
        result->value.val_s = 1;
      }

      if (ev == EV_HOLD) {
        result->value.val_s = 2;
      }

      if (ev == EV_RELEASED) {
        result->value.val_s = 3;
      }
    } else {
      result->value.val_s = 0;
    }

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Clears button events
  //#!    sys.btnClrEv([num]btn)
  //#!Sets button event to EV_NONE
  //#!Return: None
  if (sysFuncMatch(argS->callId, "btnClrEv", s)) {
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (argS->arg[1].val_u < 6) {
      sda_wrap_clear_button(argS->arg[1].val_u);
    }

    result->value.val_s = 0;

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Enable button events with LCD off
  //#!    sys.btnStdbyEn([num]val)
  //#!Enables button readout with LCD off
  //#!Return: None
  if (sysFuncMatch(argS->callId, "btnStdbyEn", s)) {
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (argS->arg[1].val_u < 6) {
      wrap_lcdOffButtons = argS->arg[1].val_u;
    }

    result->value.val_s = 0;

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!#### Text field handling

  //#!##### Handle text input
  //#!    sys.pHandleText([num]id, [str]text);
  //#!Handles text input fields. Id is field id. Text is default text value.
  //#!Return: [str] New modified text value
  if (sysFuncMatch(argS->callId, "pHandleText", s)) {
    argType[1] = SVS_TYPE_NUM; //id
    argType[2] = SVS_TYPE_STR; //textval

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    x = 0;
    while(*(s->stringField + argS->arg[2].val_str + x) != 0) {
      x++;
    }

    if (pscg_get_value(argS->arg[1].val_s, &sda_app_con)) {
      //nastavování kurzoru
      if ((pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_PRESSED) \
          || (pscg_get_event(argS->arg[1].val_s, &sda_app_con) == EV_HOLD)) {
        uint16_t temp;
        temp = LCD_Text_Get_Cursor_Pos(s->stringField + argS->arg[2].val_str, pscg_get_tmx(&sda_app_con), pscg_get_tmy(&sda_app_con));
        if (temp == 0) {
          pscg_set_param(argS->arg[1].val_s, 0, &sda_app_con);
        } else {
          pscg_set_param(argS->arg[1].val_s, temp, &sda_app_con);
        }
      }
      pscg_set_event(argS->arg[1].val_s, EV_NONE, &sda_app_con);

      //čtení z klávesnice a zápis do řetězce
      if (getKbdKey()) {
        if ((svpSGlobal.kbdKeyStr[0]) != 2) {
          result->value.val_str
            = strInsert(
                        (uint16_t)argS->arg[2].val_str,
                        strNew(svpSGlobal.kbdKeyStr, s),
                        pscg_get_param(argS->arg[1].val_s, &sda_app_con),
                        s
              ); //přičtem char
          result->type = 1;

          //divnost... pokud je to odkomentovaný, tak to prvně nastaví text s indexem +2
          //pscg_set_str(argS->arg[1].val_s,s->stringField+result->value.val_str); //nastavíme správněj text

          if (svpSGlobal.kbdKeyStr[1] != 0) {
            pscg_set_param(argS->arg[1].val_s, pscg_get_param(argS->arg[1].val_s, &sda_app_con) + 2, &sda_app_con); //posunem kurzor
          } else {
            pscg_set_param(argS->arg[1].val_s, pscg_get_param(argS->arg[1].val_s, &sda_app_con) + 1, &sda_app_con);
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

            charIndex = result->value.val_str + pscg_get_param(argS->arg[1].val_s, &sda_app_con);

            if (len >= 2
                 && (s->stringField[charIndex - 2] >= 0xC3)
                 && (s->stringField[charIndex - 2] <= 0xC5)) {
              pscg_set_param(argS->arg[1].val_s, pscg_get_param(argS->arg[1].val_s, &sda_app_con) - 2, &sda_app_con); //posune
              czFlag = 1;
              //printf("removing cz char %c%c \n", s->stringField[charIndex - 2], s->stringField[charIndex - 1]);
            } else {
              pscg_set_param(argS->arg[1].val_s, pscg_get_param(argS->arg[1].val_s, &sda_app_con) - 1, &sda_app_con); //posune
              //printf("removing normal char %c \n", s->stringField[charIndex - 1]);
            }

            prac = pscg_get_param(argS->arg[1].val_s, &sda_app_con);
            x = 0;

            while(s->stringField[result->value.val_str + prac + x] != 0) {
              s->stringField[result->value.val_str + prac + x]
                = s->stringField[result->value.val_str + prac + x + 1 + czFlag];
              x++;
            }

            *((uint8_t*)(s->stringField + result->value.val_str + len - 1)) = 0; //zkrátíme pro jistotu
            pscg_set_str(argS->arg[1].val_s,s->stringField + result->value.val_str, &sda_app_con); //nastavíme text/updatujeme
            result->type = SVS_TYPE_STR;
            return 1;
          }

        }

      }
    }
    if (pscg_get_str(argS->arg[1].val_s, &sda_app_con) != s->stringField + argS->arg[2].val_str) {
      pscg_set_str(argS->arg[1].val_s, s->stringField + argS->arg[2].val_str, &sda_app_con); //stejnak nastavíme
    }
    result->value = argS->arg[2];
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!#### Keyboard

  //#!##### Hide keyboard
  //#!    sys.keybHide();
  //#!Hides system keyboard.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "keybHide", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    hideKeyboard();
    return 1;
  }

  //#!##### Show keyboard
  //#!    sys.keybShow();
  //#!Shows system keyboard
  //#!Return: None
  if (sysFuncMatch(argS->callId, "keybShow", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    showKeyboard();
    return 1;
  }

  //#!#### Misc

  //#!##### Get random number
  //#!    rnd();
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

  //#!##### Get if system sound is disabled
  //#!    sys.getMute();
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

  //#!
  //#!#### Date selector widget
  //#!

  //#!##### Init calendar widget
  //#!    sys.wCalInit([num]year, [num]month, [num]day) #return: cal screen ID
  //#!Creates callendar widget screen.
  //#!With given year, month and day.
  //#!Return: [num]Callendar widget screen id.
  if (sysFuncMatch(argS->callId, "wCalInit", s)) {
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
  //#!    sys.wCalSelect([num]year, [num]month, [num]day);
  //#!Sets year, month and day to callendar widget.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "wCalSelect", s)) {
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
  //#!    sys.wCalUpdate();
  //#!Updates callendar widget.
  //#!Return: [num]1 when callendar is clicked.
  if (sysFuncMatch(argS->callId, "wCalUpdate", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = date_select_widget_update(&date);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Mark day
  //#!    sys.wCalMark([num]day);
  //#!Marks day in callendar widget.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "wCalMark", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    date_select_highlight(&date, argS->arg[1].val_s);
    return 1;
  }

  //#!##### Get selected day
  //#!    sys.wCalGetDay();
  //#!Returns selected day.
  //#!Return: [num]day
  if (sysFuncMatch(argS->callId, "wCalGetDay", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = date_select_widget_get_day(&date);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!
  //#!#### Misc
  //#!

  //#!##### Lock LCD sleep
  //#!    sys.sleepLock([num]val);
  //#!Sets sleep lock value. On 1 system wont go to sleep.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "sleepLock", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_set_sleep_lock(argS->arg[1].val_s);
    return 1;
  }

  //#!##### Turn on the LCD
  //#!    sys.lcdWake();
  //#!Wakes LCD screen.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "lcdWake", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_set_lcd_state(LCD_ON);
    return 1;
  }

  //#!##### Get LCD state
  //#!    sys.getLcdState();
  //#!Gets state of lcd.
  //#!Return: 1 if lcd is on, otherwise 0
  if (sysFuncMatch(argS->callId, "getLcdState", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (svpSGlobal.lcdState == LCD_OFF) {
      result->value.val_u = 0;
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }


  //#!##### Wake the SDA from sleep
  //#!    sys.sdaWake();
  //#!Wakes SDA without turning the screen on.
  //#!SDA will wake in the low power mode and will sleep again after the lcd shutdown time.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "sdaWake", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    sda_interrupt_sleep();
    return 1;
  }


  //#!##### Quit program
  //#!    sys.exit();
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
  //#!    sys.checkSVSVer([num] API_Level);
  //#!Checks for API Lvl support.
  //#!If host level is below given API_Level, error is thrown and app is terminated.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "checkSVSVer", s)) {
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
  //#!    sys.getSVSVer();
  //#!Checks for API Lvl support.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "getSVSVer", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_u = (uint16_t) SDA_OS_VERSION_NUM;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get system language
  //#!    sys.sdaGetLang();
  //#!Returns SDA_OS language.
  //#!Return: 0 if czech, 1 if english, also defines SVP_LANG_CZ SVP_LANG_ENG
  if (sysFuncMatch(argS->callId, "sdaGetLang", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_u = (uint16_t) LANG_VAL;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!#### Text obfuscation

  //#!##### Unlock
  //#!    sys.crUnLock([str]password);
  //#!Unlocks svp encryption
  //#!Return: [num] 0 if success, 2 if error, 3 if locked
  if (sysFuncMatch(argS->callId, "crUnLock", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_u = svp_crypto_unlock(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Lock
  //#!    sys.crLock();
  //#!Locks svp encryption
  //#!Return: None
  if (sysFuncMatch(argS->callId, "crLock", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_crypto_lock();
    return 1;
  }

  //#!##### Encrypt file
  //#!    sys.crEncrypt([str]fname);
  //#!Encrypts file.
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "crEncrypt", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_u = svp_encrypt(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Decrypt file
  //#!    sys.crDecrypt([str]fname);
  //#!Encrypts file.
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "crDecrypt", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_u = svp_decrypt(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!#### Subprocess


  //#!##### Set process as singular
  //#!    sys.setSingular();
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
  //#!    sys.subProcess([str]fileName, [str/ref] callback, [str] arg0, [str] arg1, [str] arg2);
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
  //#!    sys.subProcCWD([num] val);
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
  //#!    sys.subRetval([str] arg0, [str] arg1, [str] arg2);
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

  //#!#### Sound and notification LED

  //#!##### Beep the speaker
  //#!    sys.beep();
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
  //#!    sys.beepDef();
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
  //#!    sys.beepT([num]time (~ms));
  //#!Sets lenght of beep.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "beepT", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_beep_set_t(argS->arg[1].val_u);
    return 1;
  }

  //#!##### Set period of beep
  //#!    sys.beepF([num]period of ~20kHz pulses to one pulse);
  //#!Sets period of ~20kHz pulses to one pulse.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "beepF", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_beep_set_pf(argS->arg[1].val_u);
    return 1;
  }

  //#
  //#!#### Expansion Ports
  //#

  //#!##### Serial expansion transmit
  //#!    sys.serialTrs([str]data);
  //#!Sends given string to serial port on internal or external expansion connector.
  //#!Depends on what is initialized.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "serialTrs", s)) {
    int32_t i = 0;
    uint8_t * c;

    c = s->stringField + argS->arg[1].val_str;

    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    while(c[i] != 0) {
      i++;
    }

    if (i > 0) {
      sda_serial_transmit(c, i);
      result->value.val_s = 1;
    } else {
      result->value.val_s = 0;
    }

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  static uint8_t serialQueue[32];
  static uint8_t serialLen;

  //#!##### Serial expansion transmit queue
  //#!    sys.srlTrsQAdd([num]data);
  //#!Queues given hex value to transmit buffer.
  //#!Max 32 bytes.
  //#!Return: [num] 1-ok, 0-error
  if (sysFuncMatch(argS->callId, "srlTrsQAdd", s)) {

    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (serialLen < 32) {
      serialQueue[serialLen] = (uint8_t) argS->arg[1].val_s;
      serialLen++;
      result->value.val_s = 1;
    } else {
      result->value.val_s = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Serial expansion transmit queue clear
  //#!    sys.srlTrsQClr();
  //#!Clears transmit buffer.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "srlTrsQClr", s)) {

    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    serialLen = 0;
    return 1;
  }

  //#!##### Serial expansion transmit queue
  //#!    sys.srlTrsQ();
  //#!Sends previously stored queue to the initialized serial port.
  //#!Max 32 bytes.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "srlTrsQ", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (serialLen != 0) {
      sda_serial_transmit(serialQueue, serialLen);
    }
    serialLen = 0;
    return 1;
  }

  //#!##### Serial expansion recieve
  //#!    sys.serialRcv([num]timeout);
  //#!Gets string (max 512 bytes) from currently initialized serial port.
  //#!If nothing is sent during timeout (in ms), empty string is returned.
  //#!Return: [str] data
  if (sysFuncMatch(argS->callId, "serialRcv", s)) {
    uint8_t c[512];

    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (sda_serial_recieve(c, sizeof(c), argS->arg[1].val_s)){
      c[511] = 0;
      result->value.val_u = strNew(c, s);
    } else {
      result->value.val_u = strNew((uint8_t *)"", s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Set notification led pattern
  //#!    sys.sdaSetLed([num]led_type);
  //#!Sets notification led to a given pattern, uses:
  //#!LED_ON, LED_OFF ,LED_BLINK, LED_SHORTBLINK, LED_ALARM
  //#!Return: None
  if (sysFuncMatch(argS->callId, "sdaSetLed", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (argS->arg[1].val_s == 0) {
      led_set_pattern(LED_OFF);
    }

    if (argS->arg[1].val_s == 1) {
      led_set_pattern(LED_ON);
    }

    if (argS->arg[1].val_s == 2) {
      led_set_pattern(LED_BLINK);
    }

    if (argS->arg[1].val_s == 3) {
      led_set_pattern(LED_SHORTBLINK);
    }

    if (argS->arg[1].val_s == 4) {
      led_set_pattern(LED_ALARM);
    }

    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Define direction of pins on the internal expansion
  //#!    sys.iPinDef([num]Pin, [num]type, [num]pullUp);
  //#!Sets direction of internal expansion pins.
  //#!Uses defines: PIN_IN, PIN_OUT, PIN_ALT, PIN_NOPULL, PIN_PULLUP, PIN_PULDOWN
  //#!Pin number is number of pin on the connector, can be read from schematics.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "iPinDef", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;


    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }

    sda_internal_pin_def((uint8_t) argS->arg[1].val_s, (uint8_t) argS->arg[2].val_s, (uint8_t) argS->arg[3].val_s);
    return 1;
  }

  //#!##### Set state of pins on the internal expansion
  //#!    sys.iPinSet([num]Pin, [num]val);
  //#!Sets state of internal expansion pin.
  //#!Value 1 sets the pin high, value 0 sets it low.
  //#!Pin number is number of pin on the connector, can be read from schematics.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "iPinSet", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    sda_internal_pin_set((uint8_t) argS->arg[1].val_s, (uint8_t) argS->arg[2].val_s);
    return 1;
  }

  //#!##### Get state of pins on the internal expansion
  //#!    sys.iPinGet([num]Pin, [num]val);
  //#!Gets state of internal expansion pin.
  //#!Pin number is number of pin on the connector, can be read from schematics.
  //#!Return: 1 if the pin is high, 0 if it is low.
  if (sysFuncMatch(argS->callId, "iPinGet", s)) {
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_u = sda_internal_pin_get((uint8_t) argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Define direction of pins on the expansion
  //#!    sys.ePinDef([num]Pin, [num]type, [num]pullUp);
  //#!Sets direction of external expansion pins.
  //#!Uses defines: PIN_IN, PIN_OUT, PIN_ALT, PIN_NOPULL, PIN_PULLUP, PIN_PULDOWN
  //#!Pin number is number of pin on the connector, can be read from schematics.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "ePinDef", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    argType[3] = SVS_TYPE_NUM;


    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }

    sda_external_pin_def((uint8_t) argS->arg[1].val_s, (uint8_t) argS->arg[2].val_s, (uint8_t) argS->arg[3].val_s);
    return 1;
  }

  //#!##### Set state of pins on the expansion
  //#!    sys.ePinSet([num]Pin, [num]val);
  //#!Sets state of external expansion pin.
  //#!Value 1 sets the pin high, value 0 sets it low.
  //#!Pin number is number of pin on the connector, can be read from schematics.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "ePinSet", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    sda_external_pin_set((uint8_t) argS->arg[1].val_s, (uint8_t) argS->arg[2].val_s);
    return 1;
  }

  //#!##### Get state of pins on the expansion
  //#!    sys.ePinGet([num]Pin, [num]val);
  //#!Gets state of external expansion pin.
  //#!Pin number is number of pin on the connector, can be read from schematics.
  //#!Return: 1 if the pin is high, 0 if it is low.
  if (sysFuncMatch(argS->callId, "ePinGet", s)) {
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_u = sda_external_pin_get((uint8_t) argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get ADC readout
  //#!    sys.eADCRead();
  //#!Gets state of external expansion pin.
  //#!Pin number is number of pin on the connector, can be read from schematics.
  //#!Return: [float] measured voltage in volts.
  if (sysFuncMatch(argS->callId, "eADCRead", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_f = sda_external_ADC_get();
    result->type = SVS_TYPE_FLT;
    return 1;
  }

  return 0;
}

