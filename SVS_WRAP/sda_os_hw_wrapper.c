/*
Copyright (c) 2019 Stanislav Brtna

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

static uint8_t wrap_lcdOffButtons;

void wrap_set_lcdOffButtons(uint8_t val) {
  wrap_lcdOffButtons = val;
}

uint8_t wrap_get_lcdOffButtons() {
  return wrap_lcdOffButtons;
}

uint8_t sda_os_hw_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!### SDA OS HW functions

  //#!#### Constants

  //#!##### Indicator LED
  //#!| Constant | Description |
  //#!|   ---    |    ---      |
  //#!| LED_ON | Nonification led on |
  //#!| LED_OFF | Nonification led off |
  //#!| LED_BLINK | Nonification led pattern |
  //#!| LED_SHORTBLINK | Nonification led pattern |
  //#!| LED_ALARM | Nonification led pattern |

  //#!##### Buttons
  //#!| Constant | Description |
  //#!|   ---    |    ---      |
  //#!| BTN_A | Button define |
  //#!| BTN_LEFT | Button define |
  //#!| BTN_UP | Button define |
  //#!| BTN_DOWN | Button define |
  //#!| BTN_RIGHT | Button define |
  //#!| BTN_B | Button define |

  //#! ![Buttons layout](sda_buttons.png)

  //#!##### Expansion pin states
  //#!| Constant | Description |
  //#!|   ---    |    ---      |
  //#!| PIN_IN | Expansion pin setup define |
  //#!| PIN_OUT | Expansion pin setup define |
  //#!| PIN_ALT | Expansion pin setup define |
  //#!| PIN_NOPULL | Expansion pin setup define |
  //#!| PIN_PULLUP | Expansion pin setup define |
  //#!| PIN_PULLDOWN | Expansion pin setup define |
  //#!

  //#!#### LCD Functions

  //#!##### Lock LCD sleep
  //#!    sys.hw.lockSleep([num]val);
  //#!Sets sleep lock value. On 1 system wont go to sleep.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "lockSleep", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    sda_set_sleep_lock(argS->arg[1].val_s);
    return 1;
  }

  //#!##### Turn on the LCD
  //#!    sys.hw.wakeLcd();
  //#!Turns on the LCD screen.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "wakeLcd", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_set_lcd_state(LCD_ON);
    return 1;
  }

  //#!##### Get LCD state
  //#!    sys.hw.getLcdState();
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

  //#!##### Get USB State
  //#!    sys.hw.getUsbState();
  //#!Gets state of usb port. Useful for determining if the SDA is connected to PC.
  //#!Return: 1 when powered from usb, otherwise 0
  if (sysFuncMatch(argS->callId, "getUsbState", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (svpSGlobal.pwrType == POWER_USB) {
      result->value.val_u = 1;
    } else {
      result->value.val_u = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set notification led pattern
  //#!    sys.hw.setLed([num]led_type);
  //#!Sets notification led to a given pattern, uses:
  //#!LED_ON, LED_OFF ,LED_BLINK, LED_SHORTBLINK, LED_ALARM
  //#!Return: None
  if (sysFuncMatch(argS->callId, "setLed", s)) {
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

  //#
  //#!#### Expansion Ports
  //#

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


uint8_t sda_os_hw_buttons_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Buttons
  //#!##### Get button event
  //#!    sys.hw.btn.getEvent([num] btn)
  //#!Return last button event
  //#!Return: 0 - none, 1-pressed, 2 - hold, 3 - released
  if (sysFuncMatch(argS->callId, "getEvent", s)) {
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
  //#!    sys.hw.btn.clrEvent([num]btn)
  //#!Sets button event to EV_NONE
  //#!Return: None
  if (sysFuncMatch(argS->callId, "clrEvent", s)) {
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
  //#!    sys.hw.btn.stdbyEn([num]val)
  //#!Enables button readout with LCD off
  //#!Return: None
  if (sysFuncMatch(argS->callId, "stdbyEn", s)) {
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

  return 0;
}

