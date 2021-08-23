#ifndef SDA_CRYPTO_H
#define SDA_CRYPTO_H
#include "sda_util.h"

#define KEY_LEN_MAX 128
#define PASS_LEN_MAX 32

void svp_crypto_init();
uint8_t svp_crypto_unlock(uint8_t * key);
void svp_crypto_lock();

uint8_t svp_crypto_change_password(uint8_t * new_pass);

uint8_t svp_crypto_set_key(uint8_t * new_key);
uint8_t svp_crypto_set_pass_as_key();

uint8_t svp_crypto_load_keyfile(uint8_t * fname);
uint8_t svp_crypto_generate_keyfile(uint8_t * fname);

uint8_t svp_encrypt(uint8_t * fname);
uint8_t svp_decrypt(uint8_t * fname);

uint8_t svp_crypto_stream_init();
uint8_t svp_crypto_stream_encrypt(uint8_t c);
uint8_t svp_crypto_stream_decrypt(uint8_t c);

uint8_t svp_crypto_set_pass_as_key();

void svp_crypto_test();
#endif
