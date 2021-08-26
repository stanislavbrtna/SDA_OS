/*
Copyright (c) 2021 Stanislav Brtna

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

#include "sda_crypto_misc.h"


void sda_crypto_keyfile_boot_check() {
  uint8_t dirbuf[258];
  sda_conf conffile;
  uint32_t fileCRC;
  uint32_t confCRC;

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  // check if exists
  if (svp_fexists("sys_keyfile.svk")) {
    puts("Keyfile check: Keyfile exists.");
    if (svp_crypto_get_if_set_up()) {
      fileCRC = svp_crypto_get_key_crc("sys_keyfile.svk");
      printf("CRC: %u\n", fileCRC);

      if (sda_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
        printf("Failed to open cfg file\n");
      }

      confCRC = sda_conf_key_read_i32(&conffile, (uint8_t *)"key_crc", 0);
      
      if (confCRC != fileCRC) {
        printf("Key crc does not match, conf CRC: %u\n", confCRC);
      }
      sda_conf_close(&conffile);
    } else {
      puts("Crypto is not initialized, nothing to check.");
    }
    
    svp_chdir(dirbuf);
    return;
  }

  puts("Keyfile check: Keyfile not found.");
  svp_chdir(dirbuf);
  return;
}


uint8_t sda_crypto_keyfile_init_check() {
  uint8_t dirbuf[258];
  sda_conf conffile;
  uint32_t fileCRC;
  uint32_t confCRC;
  uint8_t retval = 1;

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
  }

  // check if exists
  if (svp_fexists("sys_keyfile.svk")) {
    puts("Keyfile check: Keyfile exists.");
    if (svp_crypto_get_if_set_up()) {
      fileCRC = svp_crypto_get_key_crc("sys_keyfile.svk");
      printf("CRC: %u\n", fileCRC);    

      confCRC = sda_conf_key_read_i32(&conffile, (uint8_t *)"key_crc", 0);
      
      if (confCRC != fileCRC) {
        printf("Key crc does not match, conf CRC: %u\n", confCRC);
      } else {
        puts("Key crc matches.");
        retval = 0;
      }
    } else {
      puts("Crypto is not initialized, nothing to check.");
    }
  } else {
    puts("Keyfile check: Keyfile not found, will be created");
    svp_crypto_generate_keyfile("sys_keyfile.svk");
    fileCRC = svp_crypto_get_key_crc("sys_keyfile.svk");
    printf("init_check CRC: %u\n", fileCRC);
    sda_conf_key_write_i32(&conffile, (uint8_t *)"key_crc", fileCRC);
    retval = 0;
  }

  sda_conf_close(&conffile);
  svp_chdir(dirbuf);
  return retval;
}


uint8_t svp_crypto_load_os_keyfile() {
  uint8_t dirbuf[258];
  sda_conf conffile;
  uint32_t fileCRC;
  uint32_t confCRC;

  if (!svp_crypto_get_lock()) {
    return 1;
  }

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
  }

  if (svp_fexists("sys_keyfile.svk")) {
    fileCRC = svp_crypto_get_key_crc("sys_keyfile.svk");
    confCRC = sda_conf_key_read_i32(&conffile, (uint8_t *)"key_crc", 0);
    sda_conf_close(&conffile);
    
    if (confCRC != fileCRC) {
      printf("Key crc does not match, conf CRC: %u, file CRC:%u \n", confCRC, fileCRC);
      return 1;
    } else {
      svp_crypto_load_keyfile("sys_keyfile.svk");
      svp_chdir(dirbuf);
      return 0;
    }
    
  } else {
    puts("Keyfile check: Keyfile missing.");
    return 1;
  }
}


uint8_t svp_crypto_reencrypt_os_keyfile(uint8_t* oldpass, uint8_t* newpass) {
  uint8_t dirbuf[258];

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  svp_crypto_reencrypt_key("sys_keyfile.svk", oldpass, newpass);

  svp_chdir(dirbuf);
}

