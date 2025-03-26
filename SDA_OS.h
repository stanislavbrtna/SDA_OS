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
#include "sda_gui/sda_keyboard/sda_keyboard.h"
#include "sda_fs/svp_fs.h"
#include "sda_fs/sda_conf.h"
#include "sda_fs/svp_fs_misc.h"
#include "sda_fs/svp_csv_parse.h"
#include "sda_fs/bdb/sda_bdb.h"
#include "sda_util/svp_crypto.h"
#include "sda_util/sda_crypto_key_mgmt.h"
#include "sda_util/sda_crypto_strings.h"
#include "sda_util/sda_time.h"
#include "sda_graphics/sda_p16.h"
#include "sda_graphics/sda_images.h"
#include "sda_graphics/sda_sic.h"
#include "sda_graphics/sda_os_icons.h"
#include "sda_util/native_drivers/sda_keyboard_driver.h"
#include "sda_gui/sda_overlays_api.h"
#include "sda_gui/sda_system_overlays/sda_system_overlays.h"
#include "sda_system/sda_misc.h"
#include "sda_svm/sda_svm.h"
#include "sda_system/sda_alarms.h"
#include "sda_system/sda_settings.h"
#include "sda_system/sda_irq.h"
#include "sda_system/sda_battery.h"
#include "sda_system/sda_screen_slots.h"
#include "sda_system/sda_power_management.h"
#include "sda_system/sda_strings.h"
#include "sda_system/sda_resource_lock.h"
#include "SVS_WRAP/sda_wrapper.h"
#include "SVS_WRAP/os_wrapper/sda_os_wrapper.h"
#include "SVS_WRAP/sda_files.h"

#include "SVP_SCREENS/svp_screens.h"

#include "sda_gui/sda_widgets.h"
#include "sda_gui/sda_system_overlays/sda_system_overlays.h"
#include "sda_gui/sda_tray/sda_tray.h"
#include "sda_gui/sda_scr_btn_handler.h"


// main config
extern svpStatusStruct svpSGlobal;

//irq flags
extern volatile sdaLockState touch_lock;
extern volatile sdaLockState irq_lock;
extern volatile sdaLockState tick_lock;

extern gr2context sda_sys_con;
extern gr2context sda_app_con;

extern gr2context * sda_current_con;

// main functions of SDA OS
uint8_t sda_main_loop();
uint8_t svp_irq();

//========================== misc SVP functions ==============================//

// errors
void sda_show_error_message(uint8_t * text);
void svp_errSoftPrint(svsVM *s);
void svs_hardErrHandler();

void setRedrawFlag();

void svp_switch_main_dir();

void sda_load_config();

void sda_interrupt_sleep();

void rtc_write_password(uint8_t *pwd);
uint8_t rtc_read_password(uint8_t *pwd);

uint32_t rtc_read_locked();
void rtc_write_locked(uint32_t val);

void sda_main_redraw();
void sda_main_process_touch();
#endif
