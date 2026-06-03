#ifndef SDA_CRYPTO_H
#define SDA_CRYPTO_H
#include "../sda_util.h"
#include "base32.h"

#define KEY_LEN      32
#define PASS_LEN_MAX 32

#define SDA_KEY_DEK 0
#define SDA_KEY_USR 1

// init crypto
void svp_crypto_init();

// Reset key and password
void svp_crypto_reset(uint8_t *new_pass);
// Remove encryption keys
void sda_crypto_remove();

// Check key and unlock
uint8_t svp_crypto_unlock(uint8_t *key);

// Just unlock, works only after an successful unlock with password
// for OS use only
uint8_t svp_crypto_unlock_nopass();

// Lock
void svp_crypto_lock();

// getters
uint8_t svp_crypto_get_lock();
uint8_t svp_crypto_get_if_set_up();
uint8_t svp_crypto_get_if_after_dfu();

// change password
uint8_t svp_crypto_change_password(uint8_t *new_pass);

// file encryption
uint8_t svp_encrypt(uint8_t *fname);
uint8_t svp_decrypt(uint8_t *fname);

// misc encryption functions
// encryption init
uint8_t svp_crypto_encryption_init(uint8_t *nonce, uint8_t keytype);

// block
uint8_t svp_crypto_cypher_block(uint8_t *input, uint8_t *output, size_t len);

// stream
uint8_t svp_crypto_stream_encrypt(uint8_t c);
uint8_t svp_crypto_stream_decrypt(uint8_t c);

// utils
uint32_t sda_crypto_generate_totp(const char *secret_b32, int32_t offset);

// internal functions
uint32_t crc32b(unsigned char *message);
uint32_t crc32b_len(uint8_t *message, uint32_t len);
void svp_crypto_test();

#endif
