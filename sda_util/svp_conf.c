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

#include "svp_conf.h"


uint8_t svp_conf_open(svp_conf *fc, uint8_t * fname) {
  fc->escaping = 1;
  if (svp_fopen_rw(&(fc->fil), fname) == 0) {
    return 0;
  }
  return 1;
}


uint8_t svp_conf_close(svp_conf *fc) {
  svp_fclose(&(fc->fil));
  return 0;
}


void svp_conf_set_escaping(svp_conf *fc, uint8_t val) {
  fc->escaping = val;
}


uint16_t svp_strlen_ext(uint8_t *str, uint8_t escaping) {
  uint16_t x = 0;
  uint16_t len = 0;

  while(str[x] != 0) {
    if ((str[x] == '#') && (escaping == 1)) {
      //printf("escaping: %u\n", escaping);
      len++;
    }
    x++;
    len++;
  }

  return len; //vrátí len i s terminátorem
}


uint8_t svp_conf_key_exists(svp_conf *fc, uint8_t* key) {
  uint8_t buffer[MAX_KEY_LEN];
  uint16_t x;
  uint8_t skip = 0, pass = 0;
  uint32_t keystart;

  keystart = svp_ftell(&(fc->fil));

  while (!svp_feof(&(fc->fil)) || (pass == 0)) {
    skip = 0; //flag pro přeskočení neplatného klíče (za klíčem není =)
            //je to hlavně k přeskakování prázdných řádků
    for(x = 0; x < MAX_KEY_LEN; x++) {
      buffer[x] = svp_fread_u8(&(fc->fil));
      if (buffer[x] == '=') {
        buffer[x] = 0;
        //printf("testoid: buff:%s key:%s\n", buffer, key);
        if (svp_strcmp(buffer, key)) {
          return 1;
        } else {
          break;
        }
      } else if (buffer[x] == SVP_ENDLINE) {
        skip = 1;
        break;
      }
    }
    if (skip == 0) {
      while(!svp_feof(&(fc->fil))){
        if (svp_fread_u8(&(fc->fil)) == SVP_ENDLINE) {
          break;
        }
      }
    }

    if (pass == 1){
      if (svp_ftell(&(fc->fil)) >= keystart) {
        break;
      }
    }

    if (svp_feof(&(fc->fil))) {
      svp_fseek(&(fc->fil), 0); //fseek na začátek
      pass = 1;
    }
  }
  return 0;
}


uint8_t svp_conf_key_read(svp_conf *fc, uint8_t* key, uint8_t* ret_buff, uint16_t len) {
  uint8_t buffer[MAX_KEY_LEN];
  uint16_t x;
  uint8_t skip;
  uint8_t pass = 0;
  uint32_t keystart;

  // pass zero is to scan file from current position to the end of file,
  // pass one is from start of file to keystart

  keystart = svp_ftell(&(fc->fil));

  while (!svp_feof(&(fc->fil)) | (pass == 0)) {
    skip = 0;
    for(x = 0; x < MAX_KEY_LEN; x++) {
        buffer[x] = svp_fread_u8(&(fc->fil));
        if (buffer[x] == '=') {
          buffer[x] = 0;
          if (svp_strcmp(buffer, key)) {
            x = 0;
            while (!svp_feof(&(fc->fil))) {
              if (x < len) {
                ret_buff[x] = svp_fread_u8(&(fc->fil));

                if (ret_buff[x] == 13) { // cr
                  continue;
                }

                if (fc->escaping == 1) {
                  if (ret_buff[x] == '#') {
                    ret_buff[x + 1] = svp_fread_u8(&(fc->fil));
                    if (ret_buff[x + 1] != '#') {
                      ret_buff[x] = '\n';
                      x++;
                    }
                  }
                }

                if (ret_buff[x] == SVP_ENDLINE) {
                  ret_buff[x] = 0;
                  return 1;
                } else if(svp_feof(&(fc->fil))) {
                  ret_buff[x] = 0;
                  return 1;
                }
                x++;
              }else{
                ret_buff[x] = 0;
                return 1;
              }
            }
          } else {
            break;
          }
        }else if(buffer[x] == SVP_ENDLINE) {
          skip = 1;
          break;
        }
    }
    if (skip == 0) {
      while(!svp_feof(&(fc->fil))) {
        if (svp_fread_u8(&(fc->fil)) == SVP_ENDLINE) {
          break;
        }
      }
    }

    if (pass == 1){
      if (svp_ftell(&(fc->fil)) >= keystart) {
        break;
      }
    }

    if (svp_feof(&(fc->fil))) {
      svp_fseek(&(fc->fil), 0); //fseek na začátek
      pass = 1;
    }
  }
  ret_buff[0] = 0;
  return 0;
}

// writes key with right escaping
void svp_conf_write_str(svp_conf *fc, uint8_t * val_buff) {
  uint32_t x = 0;

  while(val_buff[x] != 0) {
    if (fc->escaping == 1) {
      if (val_buff[x] == '\n') {
        svp_fwrite_u8(&(fc->fil), '#');
      } else if (val_buff[x] == '#') {
        svp_fwrite_u8(&(fc->fil), '#');
        svp_fwrite_u8(&(fc->fil), '#');
      } else {
        svp_fwrite_u8(&(fc->fil), val_buff[x]);
      }
    } else {
      svp_fwrite_u8(&(fc->fil), val_buff[x]);
    }
    x++;
  }
}


void svp_conf_key_write(svp_conf *fc, uint8_t* key, uint8_t* val_buff){
  uint32_t x;
  uint32_t keystart = 0;
  uint16_t keylen = 0;
  uint16_t strlen = 0;
  uint32_t fsize = 0;
  uint8_t cbuff = 0;

  //easy part:
  if (!svp_conf_key_exists(fc, key)) {

    if (svp_get_size(&(fc->fil))!= 0) {
      svp_fseek(&(fc->fil), svp_get_size(&(fc->fil)) - 1);
      svp_fwrite_u8(&(fc->fil), SVP_ENDLINE);
    } else {
      svp_fseek(&(fc->fil), 0);
    }
    //zápis klíče
    for (x = 0; x < MAX_KEY_LEN; x++) {
      if (key[x] != 0) {
          svp_fwrite_u8(&(fc->fil), key[x]);
      } else {
        break;
      }
    }
    //rovnítko
    svp_fwrite_u8(&(fc->fil), '=');

    svp_conf_write_str(fc, val_buff);

    svp_fwrite_u8(&(fc->fil), SVP_ENDLINE);
  } else {
    //hard part
    //key exists posunul ukazatel za rovnítko, tudíž můžem rovnou měřit dick
    keystart = svp_ftell(&(fc->fil));

    while(!svp_feof(&(fc->fil))) {
      if (svp_fread_u8(&(fc->fil)) == SVP_ENDLINE) {
        break;
      } else {
        keylen++;
      }
    }

    strlen = svp_strlen_ext(val_buff, fc->escaping);

    //printf("strlen: %u, keylen: %u\n", strlen, keylen);

    //printf("a: %u, b: %u\n", svp_strlen_ext("karelGkrdel", 1), svp_strlen_ext("karel#krdel", 1));

    if (strlen <= keylen) {

      svp_fseek(&(fc->fil), keystart);

      svp_conf_write_str(fc, val_buff);

      fsize = svp_get_size(&(fc->fil));

      for (x = keystart + keylen; x + 1 < fsize; x++) {
        svp_fseek(&(fc->fil), x);
        cbuff = svp_fread_u8(&(fc->fil));
        svp_fseek(&(fc->fil), x - keylen + strlen);
        svp_fwrite_u8(&(fc->fil), cbuff);
      }
      svp_fseek(&(fc->fil), fsize - keylen + strlen);

      svp_truncate(&(fc->fil));

    } else {
      fsize = svp_get_size(&(fc->fil));

      //možná tam má bejt x+1

      for (x = fsize - 1; x + 1 > keystart; x--) {
        svp_fseek(&(fc->fil), x);

        cbuff = svp_fread_u8(&(fc->fil));

        svp_fseek(&(fc->fil), x + strlen - keylen); //tím si nejsem 100% jist
        svp_fwrite_u8(&(fc->fil), cbuff);
      }

      svp_fseek(&(fc->fil), keystart);
      svp_conf_write_str(fc, val_buff);
    }
  }
}


void svp_conf_key_remove(svp_conf *fc, uint8_t* key) {
  uint32_t x;
  uint32_t keystart = 0;
  uint16_t keylen = 0;
  uint32_t fsize = 0;
  uint8_t cbuff = 0;

  if (svp_conf_key_exists(fc, key)) {
    svp_fseek(&(fc->fil), svp_ftell(&(fc->fil)) - 1);
    cbuff = svp_fread_u8(&(fc->fil));
    while (cbuff != SVP_ENDLINE) {
      if (svp_ftell(&(fc->fil)) > 2) {
        svp_fseek(&(fc->fil), svp_ftell(&(fc->fil)) - 2);
        cbuff = svp_fread_u8(&(fc->fil));
      } else {
        svp_fseek(&(fc->fil), 0);
        break;
      }

    }

    keystart = svp_ftell(&(fc->fil)); // behind endline

    while(!svp_feof(&(fc->fil))) {
      if (svp_fread_u8(&(fc->fil)) == SVP_ENDLINE) {
        break;
      } else {
        keylen++;
      }
    }
    keylen++;
    //printf("keylen: %s %u\n", key, keylen);

    svp_fseek(&(fc->fil), keystart);

    fsize = svp_get_size(&(fc->fil));

    for (x = keystart + keylen; x < fsize; x++) {
      svp_fseek(&(fc->fil), x);
      cbuff = svp_fread_u8(&(fc->fil));
      svp_fseek(&(fc->fil), x - keylen);
      svp_fwrite_u8(&(fc->fil), cbuff);
    }
    svp_fseek(&(fc->fil), fsize - keylen);
    svp_truncate(&(fc->fil));
  }
}

// misc functions

int32_t svp_conf_key_read_i32(svp_conf *fc, uint8_t* key, int32_t def) {
  uint8_t strBuff[23];
  uint16_t x = 0;
  uint16_t y = 0;
  uint8_t sign = 0;
  int32_t result = 0;

  if (svp_conf_key_read(fc, key, strBuff, 22) == 0) {
    return def;
  }

  while (strBuff[x] == ' ') {
    x++;
  }

  if (strBuff[x] == '-') {
    sign = 1;
    x++;
  }

  for(y = 0; y < 16; y++) {
    if ((strBuff[x] != 0) && (strBuff[x] != ' ')) {
      result *= 10;
      result += strBuff[x] - 48;
      x++;
    }
  }

  if (sign) {
    return (result * (-1));
  } else {
    return result;
  }
}


void svp_conf_key_write_i32(svp_conf *fc, uint8_t* key, int32_t val) {
  uint8_t i[22];

  if (val == 0){
    svp_conf_key_write(fc, key, (uint8_t *)"0");
    return;
  }

  sda_int_to_str(i, val, sizeof(i));
  svp_conf_key_write(fc, key, i);
}
