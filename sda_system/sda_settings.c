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

#include "sda_settings.h"

// Default colors
#define DEFAULT_BORDER_COLOR     0
#define DEFAULT_TEXT_COLOR       6435
#define DEFAULT_BACKGROUND_COLOR 65535
#define DEFAULT_FILL_COLOR       57083
#define DEFAULT_ACTIVE_COLOR     32095

// display calibration data structure
extern touchCalibDataStruct touchCalibData;

static uint8_t sda_settings_switch_to_main(sda_conf *conffile, uint8_t *dirbuf) {  
  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();
  if (sda_conf_open(conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("ERROR: Failed to open cfg file!\n");
    return 1;
  }
  return 0;
}

void sda_load_config() {
  uint8_t dirbuf[258];
  sda_conf conffile;
  svp_file calib;
  touchCalibDataStruct calibData;

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  printf("Loading config: (svp.cfg)\n");

  if(sda_settings_switch_to_main(&conffile, dirbuf)) return;

  // Load color scheme
  gr2_set_border_color((uint16_t) sda_conf_key_read_i32(&conffile, (uint8_t *)"border_color", DEFAULT_BORDER_COLOR), &sda_sys_con);
  gr2_set_text_color((uint16_t) sda_conf_key_read_i32(&conffile, (uint8_t *)"text_color", DEFAULT_TEXT_COLOR), &sda_sys_con);
  gr2_set_background_color((uint16_t) sda_conf_key_read_i32(&conffile, (uint8_t *)"background_color", DEFAULT_BACKGROUND_COLOR), &sda_sys_con);
  gr2_set_fill_color((uint16_t) sda_conf_key_read_i32(&conffile, (uint8_t *)"fill_color", DEFAULT_FILL_COLOR), &sda_sys_con);
  gr2_set_active_color((uint16_t) sda_conf_key_read_i32(&conffile, (uint8_t *)"active_color", DEFAULT_ACTIVE_COLOR), &sda_sys_con);

  // sleep timer
  svpSGlobal.lcdShutdownTime = sda_conf_key_read_i32(&conffile, (uint8_t *)"sleep_time", 5);

  if ((svpSGlobal.lcdShutdownTime < 2) || (svpSGlobal.lcdShutdownTime > 10)) {
    svpSGlobal.lcdShutdownTime = 5;
  }

  // mute
  svpSGlobal.mute = sda_conf_key_read_i32(&conffile, (uint8_t *)"mute", 0);

  // usb debug
  sda_usb_enable_for_dbg(sda_conf_key_read_i32(&conffile, (uint8_t *)"usb_debug", 0));
  svmSetAutocahceEnable(sda_conf_key_read_i32(&conffile, (uint8_t *)"svm_autocache_enabled", 0));
  
  svp_set_irq_redraw(); //po nastavení barev překreslíme panel / redraw the tray after loading color settings
  sda_conf_close(&conffile);

  // handle possible on-boot calibration
  if (svp_getLcdCalibrationFlag() == 0) {
    // load calibration data
    if (svp_fexists((uint8_t *)"calib.dat") == 1) {
      svp_fopen_rw(&calib, (uint8_t *)"calib.dat");
      svp_fread(&calib, &calibData, sizeof(calibData));
      svp_set_calibration_data(calibData);
      svp_fclose(&calib);
    } else {
      printf("Warning: calib.dat missing, calibrating now.\n");
      sdaLockState tickLockOld;
      tickLockOld = tick_lock;
      tick_lock = SDA_LOCK_LOCKED;

      sda_calibrate();

      tick_lock = tickLockOld;
      sda_store_calibration();
      setRedrawFlag();
    }
  } else {
    sda_store_calibration();
  }
  svp_chdir(dirbuf);
  printf("Done.\n");
}


void sda_store_config_gui(uint8_t set_def) {
  sda_conf conffile;
  uint8_t dirbuf[258];

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  printf("Storing config: (svp.cfg)\n");
  if (sda_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
    return;
  }
  if (set_def == 0) {
    sda_conf_key_write_i32(&conffile, (uint8_t *)"border_color", gr2_get_border_color(&sda_sys_con));
    sda_conf_key_write_i32(&conffile, (uint8_t *)"text_color", gr2_get_text_color(&sda_sys_con));
    sda_conf_key_write_i32(&conffile, (uint8_t *)"background_color", gr2_get_background_color(&sda_sys_con));
    sda_conf_key_write_i32(&conffile, (uint8_t *)"fill_color", gr2_get_fill_color(&sda_sys_con));
    sda_conf_key_write_i32(&conffile, (uint8_t *)"active_color", gr2_get_active_color(&sda_sys_con));
  } else {
    printf("Setting Default Values.\n");
    sda_conf_key_write_i32(&conffile, (uint8_t *)"border_color", DEFAULT_BORDER_COLOR);
    sda_conf_key_write_i32(&conffile, (uint8_t *)"text_color", DEFAULT_TEXT_COLOR);
    sda_conf_key_write_i32(&conffile, (uint8_t *)"background_color", DEFAULT_BACKGROUND_COLOR);
    sda_conf_key_write_i32(&conffile, (uint8_t *)"fill_color", DEFAULT_FILL_COLOR);
    sda_conf_key_write_i32(&conffile, (uint8_t *)"active_color", DEFAULT_ACTIVE_COLOR);
  }
  sda_conf_key_write_i32(&conffile, (uint8_t *)"sleep_time", svpSGlobal.lcdShutdownTime);
  sda_conf_close(&conffile);
  svp_chdir(dirbuf);
  printf("Done.\n");
}


void sda_store_settings() {
  sda_store_sleep_time();
  sda_store_dbg_options();
  sda_store_mute_config();
  sda_store_calibration();
}


void sda_store_sleep_time() {
  sda_conf conffile;
  uint8_t dirbuf[258];
  if(sda_settings_switch_to_main(&conffile, dirbuf)) return;
  sda_conf_key_write_i32(&conffile, (uint8_t *)"sleep_time", svpSGlobal.lcdShutdownTime);
  sda_conf_close(&conffile);
  svp_chdir(dirbuf);
  printf("Done.\n");
}


void sda_store_dbg_options() {
  sda_conf conffile;
  uint8_t dirbuf[258];
  if(sda_settings_switch_to_main(&conffile, dirbuf)) return;
  sda_conf_key_write_i32(&conffile, (uint8_t *)"usb_debug", sda_usb_get_enable_for_dbg());
  sda_conf_key_write_i32(&conffile, (uint8_t *)"svm_autocache_enabled", svmGetAutocahceEnable());
  sda_conf_close(&conffile);
  svp_chdir(dirbuf);
  printf("Done.\n");
}


void sda_store_mute_config() {
  sda_conf conffile;
  uint8_t dirbuf[258];
  if(sda_settings_switch_to_main(&conffile, dirbuf)) return;
  sda_conf_key_write_i32(&conffile, (uint8_t *)"mute", svpSGlobal.mute);
  sda_conf_close(&conffile);
  svp_chdir(dirbuf);
  printf("Done.\n");
}


void sda_store_calibration() {
  uint8_t dirbuf[258];
  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();
  svp_file calib;

  if(svp_fopen_rw(&calib, (uint8_t *)"calib.dat")){
    svp_fwrite(&calib, &touchCalibData, sizeof(touchCalibData));
    svp_fclose(&calib);
  }

  svp_chdir(dirbuf);
  printf("Done.\n");
}
