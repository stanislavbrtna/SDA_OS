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

#include "sda_conf.h"

#define CHECK_VALID_PRINT_ERROR_RETURN_ZERO do{if(fc->valid == 0){printf("ERROR: %s: Operation on invalid file!\n", __FUNCTION__);return 0;}}while(0)
#define CHECK_VALID_PRINT_ERROR_RETURN do{if(fc->valid == 0){printf("ERROR: %s: Operation on invalid file!\n", __FUNCTION__);return;}}while(0)

// opens conf file, 1 is returned when ok
uint8_t sda_conf_open(sda_conf *fc, uint8_t * fname) {
  fc->escaping = 1;
  fc->valid = 0;
  if (svp_fopen_rw(&(fc->fil), fname) == 0) {
    printf("ERROR: %s: file: %s not valid.\n", __FUNCTION__, fname);
    return 0;
  }
  fc->valid = 1;
  return 1;
}


uint8_t sda_conf_close(sda_conf *fc) {
  CHECK_VALID_PRINT_ERROR_RETURN_ZERO;
  svp_fclose(&(fc->fil));
  return 0;
}


void sda_conf_set_escaping(sda_conf *fc, uint8_t val) {
  fc->escaping = val;
}


uint16_t sda_strlen_ext(uint8_t *str, uint8_t escaping) {
  uint16_t x = 0;
  uint16_t len = 0;

  while(str[x] != 0) {
    if ((str[x] == '#') && (escaping == 1)) {
      len++;
    }
    x++;
    len++;
  }

  return len; //vrátí len i s terminátorem
}

uint8_t sda_conf_get_key(sda_conf *fc, uint8_t *buffer, uint16_t len) {
  CHECK_VALID_PRINT_ERROR_RETURN_ZERO;
  for(uint16_t x = 0; x < len; x++) {
    buffer[x] = svp_fread_u8(&(fc->fil));
    if (buffer[x] == '=') {
      buffer[x] = 0;
      return 1;
    } else if ((buffer[x] == SDA_ENDLINE) || svp_feof(&(fc->fil))) {
      return 0;
    }
  }
  return 0;
}

uint8_t sda_conf_skip_key(sda_conf *fc) {
  uint8_t c;

  CHECK_VALID_PRINT_ERROR_RETURN_ZERO;
  for(uint16_t x = 0; x < MAX_KEY_LEN; x++) {
    c = svp_fread_u8(&(fc->fil));
    if (c == '=') {
      return 1;
    } else if ((c == SDA_ENDLINE) || svp_feof(&(fc->fil))) {
      return 0;
    }
  }
  return 0;
}

uint32_t sda_conf_skip_line(sda_conf *fc) {
  uint8_t c = 0;
  uint32_t skipped = 0;

  CHECK_VALID_PRINT_ERROR_RETURN_ZERO;
  while(!(svp_feof(&(fc->fil))) && !(c == SDA_ENDLINE)) {
    c = svp_fread_u8(&(fc->fil));
    skipped++;
  };
  return skipped;
}


// seeks to the start of the key, when successfull
uint8_t sda_conf_key_exists(sda_conf *fc, uint8_t* key) {
  uint8_t buffer[MAX_KEY_LEN];
  uint32_t keystart;
  uint32_t startPosition;

  CHECK_VALID_PRINT_ERROR_RETURN_ZERO;
  startPosition = svp_ftell(&(fc->fil));

  while (!svp_feof(&(fc->fil))) {
    keystart = svp_ftell(&(fc->fil));
    if (sda_conf_get_key(fc, buffer, sizeof(buffer))) {
      if (svp_strcmp(buffer, key)) {
        svp_fseek(&(fc->fil), keystart);
        return 1;
      }
      sda_conf_skip_line(fc);
    }
  }

  svp_fseek(&(fc->fil), 0);
  keystart = 0;

  while (keystart < startPosition) {
    keystart = svp_ftell(&(fc->fil));
    if (sda_conf_get_key(fc, buffer, sizeof(buffer))) {
      if (svp_strcmp(buffer, key)) {
        svp_fseek(&(fc->fil), keystart);
        return 1;
      }
      sda_conf_skip_line(fc);
    }
  }

  return 0;
}


uint8_t sda_conf_key_read(sda_conf *fc, uint8_t* key, uint8_t* ret_buff, uint16_t len) {
  uint32_t x = 0;
  uint32_t startPosition = 0;

  CHECK_VALID_PRINT_ERROR_RETURN_ZERO;

  if (sda_conf_key_exists(fc, key)) {
    startPosition = svp_ftell(&(fc->fil));
    sda_conf_skip_key(fc);

    while (!svp_feof(&(fc->fil))) {
      if (x < len) {
        ret_buff[x] = svp_fread_u8(&(fc->fil));

        if (ret_buff[x] == 13) { // CR, our endline is LF, so we ignore that
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

        if ((ret_buff[x] == SDA_ENDLINE) || svp_feof(&(fc->fil))) {
          break; // got key
        }

        x++;
      } else {
        break; // value too long
      }
    }
    ret_buff[x] = 0;
    svp_fseek(&(fc->fil), startPosition);
    return 1;
  }
  ret_buff[x] = 0;
  return 0;
}


// writes key with right escaping
void sda_conf_write_str(sda_conf *fc, uint8_t * val_buff) {
  uint32_t x = 0;

  CHECK_VALID_PRINT_ERROR_RETURN;

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


void sda_conf_key_write(sda_conf *fc, uint8_t* key, uint8_t* val_buff){
  uint32_t x;
  uint32_t keystart = 0;
  uint16_t keylen = 0;
  uint16_t strlen = 0;
  uint32_t fsize = 0;
  uint8_t cbuff = 0;

  CHECK_VALID_PRINT_ERROR_RETURN;
  //printf("storing: %s in %s\n", val_buff, key);
  //easy part:
  if (!sda_conf_key_exists(fc, key)) {

    if (svp_get_size(&(fc->fil))!= 0) {
      svp_fseek(&(fc->fil), svp_get_size(&(fc->fil)) - 1);
      if (svp_fread_u8(&(fc->fil)) != SDA_ENDLINE) {
        svp_fwrite_u8(&(fc->fil), SDA_ENDLINE);
      }
    } else {
      svp_fseek(&(fc->fil), 0);
    }

    // key write
    for (x = 0; x < MAX_KEY_LEN; x++) {
      if (key[x] != 0) {
          svp_fwrite_u8(&(fc->fil), key[x]);
      } else {
        break;
      }
    }

    svp_fwrite_u8(&(fc->fil), '=');
    sda_conf_write_str(fc, val_buff);

    svp_fwrite_u8(&(fc->fil), SDA_ENDLINE);
  } else {
    sda_conf_skip_key(fc);
    keystart = svp_ftell(&(fc->fil));

    while(!svp_feof(&(fc->fil))) {
      if (svp_fread_u8(&(fc->fil)) == SDA_ENDLINE) {
        break;
      } else {
        keylen++;
      }
    }

    strlen = sda_strlen_ext(val_buff, fc->escaping);

    if (strlen <= keylen) {

      svp_fseek(&(fc->fil), keystart);

      sda_conf_write_str(fc, val_buff);

      fsize = svp_get_size(&(fc->fil));

      for (x = keystart + keylen; x < fsize; x++) {
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
      sda_conf_write_str(fc, val_buff);
    }
  }
}


void sda_conf_key_remove(sda_conf *fc, uint8_t* key) {
  uint32_t x;
  uint32_t keystart = 0;
  uint16_t keylen = 0;
  uint32_t fsize = 0;
  uint8_t cbuff = 0;

  CHECK_VALID_PRINT_ERROR_RETURN;
  if (sda_conf_key_exists(fc, key)) {
    keystart = svp_ftell(&(fc->fil)); // behind endline

    while(!svp_feof(&(fc->fil))) {
      if (svp_fread_u8(&(fc->fil)) == SDA_ENDLINE) {
        break;
      } else {
        keylen++;
      }
    }
    keylen++;

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

int32_t sda_conf_key_read_i32(sda_conf *fc, uint8_t* key, int32_t def) {
  uint8_t strBuff[23];
  uint16_t x = 0;
  uint16_t y = 0;
  uint8_t sign = 0;
  int32_t result = 0;

  CHECK_VALID_PRINT_ERROR_RETURN_ZERO;

  if (sda_conf_key_read(fc, key, strBuff, 22) == 0) {
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


void sda_conf_key_write_i32(sda_conf *fc, uint8_t* key, int32_t val) {
  uint8_t i[22];

  CHECK_VALID_PRINT_ERROR_RETURN;
  if (val == 0){
    sda_conf_key_write(fc, key, (uint8_t *)"0");
    return;
  }

  sda_int_to_str(i, val, sizeof(i));
  sda_conf_key_write(fc, key, i);
}


uint8_t sda_conf_key_contains(sda_conf *fc, uint8_t* key, uint8_t* value) {
  uint32_t x = 0;
  uint32_t startPosition = 0;

  CHECK_VALID_PRINT_ERROR_RETURN_ZERO;

  if (sda_conf_key_exists(fc, key)) {
    startPosition = svp_ftell(&(fc->fil));
    sda_conf_skip_key(fc);
    while (!svp_feof(&(fc->fil))) {
      uint8_t c = svp_fread_u8(&(fc->fil));
      if (value[x] == c) {
        x++;
      } else {
        if (x != 0) {
          x = 0;
        }
      }

      if (value[x] == 0) {
        svp_fseek(&(fc->fil), startPosition);
        return 1;
      }

      if ((c == SDA_ENDLINE) || svp_feof(&(fc->fil))) {
        break;
      }
    }
    svp_fseek(&(fc->fil), startPosition);
    return 0;
  }

  return 2;
}

