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

#include "sda_wrapper.h"

uint8_t sda_os_hw_com_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  static uint8_t serialQueue[32];
  static uint8_t serialLen;

  //#!### Communication

  //#!#### Serial transmit queue
  //#!Could be used for all the transmission modes.

  //#!##### Serial expansion transmit queue
  //#!    sys.com.qAdd([num]data);
  //#!Queues given hex value to transmit buffer.
  //#!Max 32 bytes.
  //#!Return: [num] 1-ok, 0-full buffer
  if (sysFuncMatch(argS->callId, "qAdd", s)) {

    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (serialLen < sizeof(serialQueue)) {
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
  //#!    sys.com.qClr();
  //#!Clears transmit buffer.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "sqClr", s)) {

    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    serialLen = 0;
    return 1;
  }

  //#!#### USB serial interface
  //#!

  //#!##### USB serial transmit
  //#!    sys.com.usbTrs([str]data);
  //#!Sends given string to usb serial port.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "usbTrs", s)) {
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
      sda_usb_serial_transmit(c, i);
      result->value.val_s = 1;
    } else {
      result->value.val_s = 0;
    }

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### USB serial transmit queue
  //#!    sys.com.usbTrsQ();
  //#!Sends previously stored queue to the initialized serial port.
  //#!Queue can be filled with sys.srlTrsQAdd and cleared with sys.srlTrsQClr.
  //#!Max 32 bytes.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "usbTrsQ", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (serialLen != 0) {
      sda_usb_serial_transmit(serialQueue, serialLen);
    }
    serialLen = 0;
    return 1;
  }

  //#!##### USB serial receive
  //#!    sys.com.usbRcv([num]timeout);
  //#!Gets string (max 512 bytes) from USB serial port.
  //#!If nothing is sent during timeout (in ms), empty string is returned.
  //#!Return: [str] data
  if (sysFuncMatch(argS->callId, "usbRcv", s)) {
    uint8_t c[512];

    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (sda_usb_serial_recieve(c, sizeof(c), argS->arg[1].val_s)){
      c[511] = 0;
      result->value.val_u = strNew(c, s);
    } else {
      result->value.val_u = strNew((uint8_t *)"", s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### USB serial receive init
  //#!    sys.com.usbRcvIT();
  //#!Initializes usb serial port receive operation in non-blocking mode
  //#!Returns 1 if ok, 0 if error occurred
  //#!Return: [num] result


  if (sysFuncMatch(argS->callId, "usbRcvIT", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = sda_usb_serial_recieve_init();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### USB serial get ready flag
  //#!    sys.com.usbGetRd();
  //#!Gets transmission ready flag. Returns 1 if data is pending,
  //#!2 if whole line of data is pending
  //#!Return: [num] ready
  if (sysFuncMatch(argS->callId, "usbGetRd", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = sda_usb_serial_get_rdy();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### USB serial get pending data
  //#!    sys.com.usbGetStr();
  //#!Gets the pending string and resets the serial interface
  //#!for another ready flag.
  //#!Return: [str] pending

  static uint8_t usb_c[513];
  static uint16_t usb_len;

  if (sysFuncMatch(argS->callId, "usbGetStr", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (sda_usb_serial_get_str(usb_c)){
      usb_c[512] = 0;
      result->value.val_u = strNew(usb_c, s);
    } else {
      result->value.val_u = strNew((uint8_t *)"", s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### USB serial get pending data
  //#!    sys.com.usbGetBytes();
  //#!Gets the bytes from a serial interface and stores them in local buffer (512 Bytes max)
  //#!Return: [num] bytes used

  if (sysFuncMatch(argS->callId, "usbGetBytes", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    usb_len = sda_usb_serial_get_str(usb_c);

    result->value.val_s = usb_len;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### USB serial get pending data
  //#!    sys.com.usbGetByte([num] index);
  //#!Reads the byte value from a serial interface local buffer (512 Bytes)
  //#!Return: [num] byte value (0 - 255, -1 when error occurs)

  if (sysFuncMatch(argS->callId, "usbGetByte", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (argS->arg[1].val_s < usb_len) {
      result->value.val_s = usb_c[argS->arg[1].val_s];
    } else {
      result->value.val_s = (int32_t)-1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!#### Expansion port serial interface
  //#!
  //#!##### Serial expansion transmit
  //#!Code to init the internal expansion port serial interface:
  //#!    sys.hw.iPinDef(15, PIN_ALT, PIN_NOPULL);
  //#!    sys.hw.iPinDef(16, PIN_OUT, PIN_NOPULL);
  //#!Code to init the external expansion port serial interface:
  //#!    sys.hw.ePinDef(5, PIN_ALT, PIN_NOPULL);
  //#!    sys.hw.ePinDef(6, PIN_ALT, PIN_NOPULL);

  //#!##### Serial expansion transmit
  //#!    sys.com.uartTrs([str]data);
  //#!Sends given string to serial port on internal or external expansion connector.
  //#!Depends on what is initialized.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "uartTrs", s)) {
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

  //#!##### Serial expansion transmit queue
  //#!    sys.com.uartTrsQ();
  //#!Sends previously stored queue to the initialized serial port.
  //#!Max 32 bytes.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "uartTrsQ", s)) {

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
  //#!    sys.com.uartRcv([num]timeout);
  //#!Gets string (max 512 bytes) from currently initialized serial port.
  //#!If nothing is sent during timeout (in ms), empty string is returned.
  //#!Return: [str] data
  if (sysFuncMatch(argS->callId, "uartRcv", s)) {
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

  //#!#### Serial recieve in non-blocking mode
  //#!For more flexible serial interface operations

  //#!##### Serial expansion receive
  //#!    sys.com.uartRcvIT();
  //#!Initializes serial port receive operation in non-blocking mode
  //#!Returns 1 if ok, 0 if error occurred
  //#!Return: [num] result


  if (sysFuncMatch(argS->callId, "uartRcvIT", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = sda_serial_recieve_init();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Serial expansion get ready flag
  //#!    sys.com.uartGetRd();
  //#!Gets transmission ready flag. Returns 1 if data is pending,
  //#!2 if whole line of data is pending
  //#!Return: [num] ready
  if (sysFuncMatch(argS->callId, "uartGetRd", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = sda_serial_get_rdy();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Serial expansion get pending data
  //#!    sys.com.uartGetStr();
  //#!Gets the pending string and resets the serial interface
  //#!for another ready flag.
  //#!Return: [str] pending

  static uint8_t c[513];
  static uint16_t len;

  if (sysFuncMatch(argS->callId, "uartGetStr", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (sda_serial_get_str(c)){
      c[512] = 0;
      result->value.val_u = strNew(c, s);
    } else {
      result->value.val_u = strNew((uint8_t *)"", s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Serial expansion get pending data
  //#!    sys.com.uartGetBytes();
  //#!Gets the bytes from a serial interface and stores them in local buffer (512 Bytes)
  //#!Return: [num] bytes used

  if (sysFuncMatch(argS->callId, "uartGetBytes", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    len = sda_serial_get_str(c);
    
    result->value.val_s = len;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Serial expansion get pending data
  //#!    sys.com.uartGetByte([num] index);
  //#!Gets the byte value from a serial interface local buffer (512 Bytes)
  //#!Return: [num] byte value (0 - 255, -1 when error occurs)

  if (sysFuncMatch(argS->callId, "uartGetByte", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    
    if (argS->arg[1].val_s < len) {
      result->value.val_s = c[argS->arg[1].val_s];
    } else {
      result->value.val_s = (int32_t)-1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}
