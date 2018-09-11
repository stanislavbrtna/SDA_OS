#ifndef SDA_CRYPTO_H
#define SDA_CRYPTO_H
#include "sda_util.h"

#define KEY_LEN_MAX 32

void svp_crypto_init();
uint8_t svp_crypto_unlock(uint8_t * key);
void svp_crypto_lock();
uint8_t svp_encrypt(uint8_t * fname);
uint8_t svp_decrypt(uint8_t * fname);
uint8_t svp_crypto_change_key(uint8_t * new_key);

#endif
