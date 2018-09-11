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

#ifndef SVP_BASICS_H
#define SVP_BASICS_H

// constants and constraints (just defines)
#include "sda_consts.h"

//
#include "../sda_platform.h"

// gui library defines and data types
#include "GR2/GR2.h"

// SDA-OS data types
#include "sda_types.h"

#ifdef LANG_EN
#include "Strings_EN.h"
#define LANG_VAL 1
#else
#include "Strings_CZ.h"
#define LANG_VAL 0
#endif

#include "SVS/svs_basics.h"
#include "sda_base.h"
#include "sda_util/sda_util.h"
#include "sda_util/sda_keyboard.h"
#include "sda_util/svp_fs.h"
#include "sda_util/svp_conf.h"
#include "sda_util/svp_fs_misc.h"
#include "sda_util/svp_csv_parse.h"
#include "sda_util/svp_crypto.h"
#include "sda_util/sda_time.h"
#include "sda_system/sda_overlays_api.h"
#include "sda_system/sda_misc.h"
#include "sda_system/sda_svm.h"
#include "sda_system/sda_alarms.h"
#include "sda_system/sda_settings.h"
#include "sda_system/sda_irq.h"
#include "SVS_WRAP/sda_wrapper.h"

#include "SVP_SCREENS/svp_screens.h"

#include "sda_gui/sda_widgets.h"
#include "sda_gui/sda_system_overlays.h"
#include "sda_gui/sda_tray.h"
#include "sda_gui/sda_tray.h"

// main config
extern svpStatusStruct svpSGlobal;

//irq flags
extern volatile uint8_t touch_lock;
extern volatile uint8_t redraw_lock;
extern volatile uint8_t irq_lock;

//fonts
extern uint8_t *font8x8_basic;
extern uint8_t *font18;
extern uint8_t *font32;

extern gr2context sda_sys_con;
extern gr2context sda_app_con;

extern gr2context * sda_current_con;

// main functions of SDA OS
uint8_t sda_main_loop();
uint8_t svp_irq();

//===========================misc SVP functions===============================//

// appSlots
void sdaSlotSetValid(uint16_t slot);
void sdaSlotSetInValid(uint16_t slot);
uint16_t sdaSlotGetValid(uint16_t slot);
uint16_t sdaGetSlotScreen(uint8_t slot);
void sdaSlotOnTop(uint8_t slot);

// errors
void sda_show_error_message(uint8_t * text);
void svp_errSoftPrint(svsVM *s);
void svs_hardErrHandler();

void sdaSetSleepLock(uint8_t val);

//graphics
void setRedrawFlag();

void svp_switch_main_dir();

void sda_load_config();

#endif
