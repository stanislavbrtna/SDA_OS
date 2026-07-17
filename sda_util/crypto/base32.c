#include "base32.h"


// Generated with gemini
int base32_decode(const char *src, uint8_t *dst, int dst_len) {
  int idx = 0, bit_buf = 0, bit_cnt = 0;
  for (int i = 0; src[i] != '\0'; i++) {
    char ch = src[i];
    if (ch == '=')
      break; // End of padding

    int val;
    if (ch >= 'A' && ch <= 'Z')
      val = ch - 'A';
    else if (ch >= 'a' && ch <= 'z')
      val = ch - 'a';
    else if (ch >= '2' && ch <= '7')
      val = ch - '2' + 26;
    else
      continue; // Ignore invalid characters like spaces

    bit_buf = (bit_buf << 5) | val;
    bit_cnt += 5;

    if (bit_cnt >= 8) {
      bit_cnt -= 8;
      if (idx < dst_len) {
        dst[idx++] = (bit_buf >> bit_cnt) & 0xFF;
      }
    }
  }
  return idx;
}

size_t get_base32_encoded_length(size_t input_length) { return ((input_length + 4) / 5) * 8; }

static const char base32_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

void base32_encode(const uint8_t *input, size_t input_len, char *output) {
  size_t i = 0;
  size_t j = 0;

  while (i < input_len) {
    uint64_t bits = 0;
    int count = 0;

    for (int b = 0; b < 5; b++) {
      if (i < input_len) {
        bits |= ((uint64_t)input[i++]) << (32 - b * 8);
        count++;
      }
    }

    int out_chars;
    switch (count) {
    case 1:
      out_chars = 2;
      break;
    case 2:
      out_chars = 4;
      break;
    case 3:
      out_chars = 5;
      break;
    case 4:
      out_chars = 7;
      break;
    case 5:
      out_chars = 8;
      break;
    default:
      out_chars = 0;
      break;
    }

    for (int c = 0; c < 8; c++) {
      if (c < out_chars) {
        int index = (bits >> (35 - c * 5)) & 0x1F;
        output[j++] = base32_alphabet[index];
      } else {
        output[j++] = '=';
      }
    }
  }
  output[j] = '\0';
}