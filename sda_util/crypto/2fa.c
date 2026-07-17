#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Header files from the cifra library
#include "cifra/hmac.h"
#include "cifra/sha1.h"

#include "../../SDA_OS.h"

// generated with gemini
uint32_t sda_crypto_generate_totp(const char *secret_b32, int32_t time_offset) {
  uint8_t key[100];

  int key_len = base32_decode(secret_b32, key, sizeof(key));

  // 2. Calculate the time step (30-second intervals since Unix epoch)
  uint64_t timestamp =
      (1167602400 + (uint64_t)(svpSGlobal.timestamp + time_offset)) / 30; //(uint64_t)time(NULL) / 30;

  // 3. Convert the timestamp integer to an 8-byte Big-Endian array
  uint8_t msg[8];
  for (int i = 7; i >= 0; i--) {
    msg[i] = timestamp & 0xFF;
    timestamp >>= 8;
  }

  // 4. Calculate HMAC-SHA1 using the CIFRA generic function
  uint8_t hmac_out[CF_SHA1_HASHSZ]; // 20 bytes output buffer
  cf_hmac(key, key_len, msg, sizeof(msg), hmac_out, &cf_sha1);

  // 5. Dynamic Truncation according to RFC 4226/6238
  int offset = hmac_out[CF_SHA1_HASHSZ - 1] & 0x0F;

  uint32_t code = ((hmac_out[offset] & 0x7F) << 24) | ((hmac_out[offset + 1] & 0xFF) << 16) |
      ((hmac_out[offset + 2] & 0xFF) << 8) | (hmac_out[offset + 3] & 0xFF);

  // 6. Reduce to a 6-digit code (modulo 10^6)
  return code % 1000000;
}

uint32_t sda_crypto_totp_remaining(int32_t time_offset) {
  uint64_t timestamp = (1167602400 + (uint64_t)(svpSGlobal.timestamp + time_offset)) / 30;
  return (uint32_t)(((uint64_t)timestamp + 1)*30) - (1167602400 + (uint64_t)(svpSGlobal.timestamp + time_offset));
}