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

#include "sda_gr2_wrap.h"

//#!  Automatically generated documentation for GR2 SVS wrapper, follows markdown syntax.
//#!
//#!### GR2 Graphics library
//#!
//#!
//#!#### Constants
//#!
//#!##### Element events
//#!|  Constant     |  Value |  Meaning          |
//#!|  ---          |  ---   |  ---              |
//#!|  EV_PRESSED   |  1     |  Event: pressed   |
//#!|  EV_RELEASED  |  3     |  Event: released  |
//#!|  EV_HOLD      |  2     |  Event: hold      |
//#!|  EV_NONE      |  0     |  Event: none      |
//#!
//#!##### UI Colors
//#!|  Constant       |  Value |  Meaning            |
//#!|  ---            |  ---   |  ---                |
//#!|  COL_BORDER     |  1     |  Color: Border      |
//#!|  COL_TEXT       |  2     |  Color: Text        |
//#!|  COL_BACKGROUND |  3     |  Color: Background  |
//#!|  COL_FILL       |  4     |  Color: Fill        |
//#!|  COL_ACTIVE     |  5     |  Color: active      |
//#!
//#!##### Text alignment
//#!|  Constant     |  Value |  Meaning            |
//#!|  ---          |  ---   |  ---                |
//#!|  ALIGN_LEFT   |  5     |  Text align: Left   |
//#!|  ALIGN_RIGHT  |  5     |  Text align: Right  |
//#!|  ALIGN_CENTER |  5     |  Text align: Center |
//#!

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
  uint8_t ret = 0;
  
  ret = sda_gr2_getset_subwrap(result, argS, s);
  if (ret != 2) {
    return ret;
  }

  ret = sda_gr2_txt_subwrap(result, argS, s);
  if (ret != 2) {
    return ret;
  }

  ret = sda_gr2_inits_subwrap(result, argS, s);
  if (ret != 2) {
    return ret;
  }

  return 0;
}

