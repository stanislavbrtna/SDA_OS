#ifndef SDA_CRYPTO_H
#define SDA_CRYPTO_H
#include "sda_util.h"

#define KEY_LEN_MAX 128
#define PASS_LEN_MAX 32

// setup

// init crypto
void svp_crypto_init();

// Check key and unlock
uint8_t svp_crypto_unlock(uint8_t * key);

// Just unlock, works only after an successful unlock with password
// for OS use only
uint8_t svp_crypto_unlock_nopass();

// Lock
void svp_crypto_lock();

// getters
uint8_t svp_crypto_get_lock();
uint8_t svp_crypto_get_if_set_up();

// change password
uint8_t svp_crypto_change_password(uint8_t * new_pass);

// key management
uint8_t svp_crypto_set_key(uint8_t * new_key);
uint8_t svp_crypto_set_pass_as_key();

uint8_t svp_crypto_load_keyfile(uint8_t * fname);
uint8_t svp_crypto_generate_keyfile(uint8_t * fname);
uint32_t svp_crypto_get_key_crc(uint8_t *fname);
uint8_t svp_crypto_reencrypt_key(uint8_t *fname, uint8_t *oldpass, uint8_t *newpass);

// file encryption
uint8_t svp_encrypt(uint8_t * fname);
uint8_t svp_decrypt(uint8_t * fname);

// misc stream encryption functions
uint8_t svp_crypto_stream_init();
uint8_t svp_crypto_stream_encrypt(uint8_t c);
uint8_t svp_crypto_stream_decrypt(uint8_t c);

// internal functions
uint8_t svp_crypto_load_key_to_str(uint8_t * fname, uint8_t* str);
uint8_t svp_crypto_write_keyfile(uint8_t *fname, uint8_t *key);
uint32_t crc32b(unsigned char *message);
void svp_crypto_test();

#endif
