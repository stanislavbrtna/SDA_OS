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

#include "sda_strings.h"

uint32_t sda_str_add(uint8_t *str, uint8_t *str2) {
  uint32_t i, n;
  i = 0;
  while(str[i] != 0) {
    i++;
  }

  n = 0;
  while(str2[n] != 0) {
    str[i] = str2[n];
    i++;
    n++;
  }
  str[i] = 0;
  return n + 1;
}


uint32_t sda_strcp(uint8_t *in, uint8_t *out, uint32_t len) {
  uint32_t x;
  for (x = 0; x < (len - 1); x++) {
    out[x] = in[x];
    if (in[x] == 0) {
      return (x + 1);
    }
  }
  out[len - 1] = 0;
  return 0;
}

// returns the string lenght, including zero terminator
uint32_t sda_strlen(uint8_t * str) {
  uint32_t len = 0;

  while (str[len] != 0) {
    len++;
  }

  return len;
}


void sda_int_to_str(uint8_t * buff, int32_t val, uint32_t len) {
  uint16_t a;
  uint8_t negative = 0;

  if (val == 0) {
    buff[0] = '0';
    buff[1] = 0;
    return;
  }

  for(uint16_t x = 0; x < len; x++) {
    buff[x] = ' ';
  }
  buff[len - 1] = 0;

  if (val < 0) {
    val *= -1;
    negative = 1;
  }

  a = 1;
  while (0 != val) {
    buff[len - a - 1] = (val % 10 + 48);
    val = val / 10;
    a++;
    if ((uint32_t)a == len){
      buff[0] = 0;
      return;
    }
  }

  if (negative) {
    buff[len - a - 1] = '-';
  }

  while(buff[0] == ' ') {
    for(uint16_t x = 0; x < len ; x++) {
      buff[x] = buff[x + 1];
    }
  }
}

// needs at least 17 chars long buffer
void sda_time_to_str(uint8_t * buff, uint32_t time_secs) {
  uint8_t a = 0;

  for (a = 0; a < 16; a++) {
    buff[a] = ' ';
  }

  buff[16] = 0;

  if (time_secs < 60) {
    buff[15] = 's';
    buff[14] = time_secs % 10 + 48;
    buff[13] = (time_secs) / 10 % 6 + 48;
    buff[12] = ' ';
  }

  if (time_secs > 60) {
    buff[11] = 'm';
    buff[10] = (time_secs / 60) % 10 + 48;
    buff[9] = (time_secs / 60) / 10 % 6 + 48;
  }

  if (time_secs > 3600) {
    buff[8] = ' ';
    buff[7] = 'h';

    buff[6] = ((time_secs / 3600) % 24) % 10 + 48;
    buff[5] = ((time_secs / 3600) % 24) / 10 % 6 + 48;

  }

  if (time_secs > 3600 * 24) {
    buff[4] = ' ';
    buff[3] = 'd';

    buff[2] = ((time_secs / 3600) / 24) % 10 + 48;
    buff[1] = ((time_secs / 3600) / 24) / 10 % 10 + 48;
    buff[0] = ((time_secs / 3600) / 24) / 100 % 10 + 48;
  }

  while (buff[0] == ' ') {
    for (a = 0; a < 16; a++) {
      buff[a] = buff[a + 1];
    }
  }
}


uint16_t sda_str_insert(uint8_t *index1, uint8_t *index2, uint8_t *buff, uint16_t pos, uint16_t len) {
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t cont;

  // source
  while((index1[x] != 0) && (x != pos)) {
    if (x > len) {
      return 1;
    }
    buff[y] = index1[x];

    y++;
    x++;
  }

  cont = x;
  x = 0;

  // insert string no. 2
  while(index2[x] != 0) {
    buff[y] = index2[x];
    x++;
    y++;
  }

  // finish string no. 1
  x = cont;

  while(index1[x] != 0) {

    buff[y] = index1[x];
    x++;
    y++;
  }

  buff[y] = 0;

  x = 0;
  while(buff[x] != 0){
    index1[x] = buff[x];
    x++;
  }

  index1[x] = buff[x];
  return 1;
}


uint32_t sda_str_find(uint8_t* str, uint8_t* pattern) {
  uint32_t i_str = 0;
  uint32_t i_pattern = 0;

  while (str[i_str] != 0) {
    if (str[i_str] == pattern[i_pattern]) {
      i_pattern++;

      if (pattern[i_pattern] == 0) {
        return i_str - i_pattern + 1;
      }
    } else {
      i_pattern = 0;
    }

    i_str++;
  }

  return 0;
}

uint8_t sda_str_lower(uint8_t c) {
  if (c >= 'A' && c <= 'Z') {
    return c + 32;
  } else {
    return c;
  }
}