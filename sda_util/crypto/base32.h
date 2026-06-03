#ifndef _BASE32_H_
#define _BASE32_H_

#include "../../SDA_OS.h"

int base32_decode(const char *src, uint8_t *dst, int dst_len);

size_t get_base32_encoded_length(size_t input_length);
void base32_encode(const uint8_t *input, size_t input_len, char *output);

#endif