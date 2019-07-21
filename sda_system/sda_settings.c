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

// display calibration data structure
extern touchCalibDataStruct touchCalibData;

void sda_load_config() {
  uint8_t dirbuf[258];
  svp_conf conffile;
  svp_file calib;
  touchCalibDataStruct calibData;

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  printf("Loading config: (svp.cfg)\n");
  if (svp_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
  }

  pscg_set_border_color((uint16_t) svp_conf_key_read_i32(&conffile, (uint8_t *)"border_color", 0), &sda_sys_con);
  pscg_set_text_color((uint16_t) svp_conf_key_read_i32(&conffile, (uint8_t *)"text_color", 0), &sda_sys_con);
  pscg_set_background_color((uint16_t) svp_conf_key_read_i32(&conffile, (uint8_t *)"background_color", 0xF800), &sda_sys_con);
  pscg_set_fill_color((uint16_t) svp_conf_key_read_i32(&conffile, (uint8_t *)"fill_color", 0x07E0), &sda_sys_con);
  pscg_set_active_color((uint16_t) svp_conf_key_read_i32(&conffile, (uint8_t *)"active_color", 0xFFFF), &sda_sys_con);

  //sleepTimer
  svpSGlobal.lcdShutdownTime = svp_conf_key_read_i32(&conffile, (uint8_t *)"sleep_time", 5);

  svpSGlobal.mute = svp_conf_key_read_i32(&conffile, (uint8_t *)"mute", 0);

  if ((svpSGlobal.lcdShutdownTime < 2)||(svpSGlobal.lcdShutdownTime > 10)) {
    svpSGlobal.lcdShutdownTime = 5;
  }
  svp_set_irq_redraw(); //po nastavení barev překreslíme panel / redraw the tray after loading color settings
  svp_conf_close(&conffile);

  // handle possible on-boot calibration
  if (svp_getLcdCalibrationFlag() == 0) {
    // load calibration data
    // TODO: perform calibration if calib.dat is missing
    if (svp_fexists((uint8_t *)"calib.dat") == 1) {
      svp_fopen_rw(&calib, (uint8_t *)"calib.dat");
      svp_fread(&calib, &calibData, sizeof(calibData));
      svp_set_calibration_data(calibData);
      svp_fclose(&calib);
    } else {
      printf("Warning: calib.dat missing, using default calibration data.\n");
    }
  } else {
    sda_store_calibration();
  }
  svp_chdir(dirbuf);
  printf("Done.\n");
}


void sda_store_config_gui(uint8_t set_def) {
  svp_conf conffile;
  uint8_t dirbuf[258];

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  printf("Storing config: (svp.cfg)\n");
  if (svp_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
    return;
  }
  if (set_def == 0) {
    svp_conf_key_write_i32(&conffile, (uint8_t *)"border_color", pscg_get_border_color(&sda_sys_con));
    svp_conf_key_write_i32(&conffile, (uint8_t *)"text_color", pscg_get_text_color(&sda_sys_con));
    svp_conf_key_write_i32(&conffile, (uint8_t *)"background_color", pscg_get_background_color(&sda_sys_con));
    svp_conf_key_write_i32(&conffile, (uint8_t *)"fill_color", pscg_get_fill_color(&sda_sys_con));
    svp_conf_key_write_i32(&conffile, (uint8_t *)"active_color", pscg_get_active_color(&sda_sys_con));
  } else {
    printf("Setting Default Values.\n");
    svp_conf_key_write_i32(&conffile, (uint8_t *)"border_color", 0);
    svp_conf_key_write_i32(&conffile, (uint8_t *)"text_color",0);
    svp_conf_key_write_i32(&conffile, (uint8_t *)"background_color", 0xF800);
    svp_conf_key_write_i32(&conffile, (uint8_t *)"fill_color", 0x07E0);
    svp_conf_key_write_i32(&conffile, (uint8_t *)"active_color", 0xFFFF);
  }
  svp_conf_key_write_i32(&conffile, (uint8_t *)"sleep_time", svpSGlobal.lcdShutdownTime);
  svp_conf_close(&conffile);
  svp_chdir(dirbuf);
  printf("Done.\n");

}


void sda_store_config() {
  svp_conf conffile;
  uint8_t dirbuf[258];
  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();
  printf("Storing config: (svp.cfg)\n");
  if (svp_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
    return;
  }
  svp_conf_key_write_i32(&conffile, (uint8_t *)"sleep_time", svpSGlobal.lcdShutdownTime);
  svp_conf_close(&conffile);
  svp_chdir(dirbuf);
  printf("Done.\n");
}


void sda_store_mute_config() {
  svp_conf conffile;
  uint8_t dirbuf[258];
  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();
  printf("Storing mute config: (svp.cfg)\n");
  if (svp_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
    return;
  }
  svp_conf_key_write_i32(&conffile, (uint8_t *)"mute", svpSGlobal.mute);
  svp_conf_close(&conffile);
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
