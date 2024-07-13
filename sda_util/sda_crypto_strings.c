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

#include "sda_crypto_strings.h"


uint8_t nibble_to_hex(uint8_t val) {
  val = val & 0x0f;

  if (val < 10) {
    return val + '0';
  } else {
    return val + 'A' - 10;
  }
}

uint8_t hex_to_nibble(uint8_t val) {
  if (val < 'A') {
    return val - '0';
  } else {
    return val - 'A' + 10;
  }
}


uint8_t sda_encrypt_string(uint8_t * source, uint8_t * dest, uint32_t len) {
  uint8_t encr_char;

  if (svp_crypto_get_lock() == 0) {
    return 1;
  }
 
  svp_crypto_stream_init();

  uint32_t i = 0;
  uint32_t b = 0;
  
  while(source[i] != 0) {
    encr_char = svp_crypto_stream_encrypt(source[i]);
    i++;

    dest[b] = nibble_to_hex(encr_char >> 4);
    b++;
    dest[b] = nibble_to_hex(encr_char);
    b++;

    //printf("%u converted as: %c %c\n", encr_char, dest[b-2], dest[b-1]);

    if (b >= len) {
      printf("%s: Target buffer overflow!\n", __FUNCTION__);
      return 1;
    }
  }
  dest[b] = 0;

  return 0;
}

uint8_t sda_decrypt_string(uint8_t * source, uint8_t * dest, uint32_t len) {
  uint8_t raw_byte;

  if (svp_crypto_get_lock() == 0) {
    return 1;
  }

  svp_crypto_stream_init();

  uint32_t i = 0;
  uint32_t b = 0;

  while(source[b] != 0) {
    
    raw_byte = (hex_to_nibble(source[b]) << 4) + (hex_to_nibble(source[b + 1]) & 0x0F) ;
    b += 2;

    //printf("%u with sources: %c %c\n", raw_byte, source[b-2], source[b-1]);

    dest[i] = svp_crypto_stream_decrypt(raw_byte);
    i++;

    if (i >= len) {
      printf("%s: Target buffer overflow!\n", __FUNCTION__);
      return 1;
    }
  }

  dest[i] = 0;

  return 0;
}


uint8_t sda_test_crypto_strings() {
  uint8_t buff[256];
  uint8_t deBuff[256];

  svp_crypto_unlock("def");

  sda_encrypt_string("Quick brown fox jumps over the žluťoučký kůň.", buff, sizeof(buff));

  printf("Encrypted: %s \n", buff);

  sda_decrypt_string(buff, deBuff, sizeof(deBuff));

  printf("Decrypted again: %s \n", deBuff);

  if(sda_strcp("Quick brown fox jumps over the žluťoučký kůň.", deBuff, sizeof(deBuff))) {
    return 0;
  } else {
    printf("%s: Test failed, strings doesn't match!", __FUNCTION__);
    return 1;
  }
}