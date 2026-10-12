#ifndef SDA_CRYPTO_BUFFERS_H
#define SDA_CRYPTO_BUFFERS_H
#include "svp_crypto.h"

void sda_crypto_fwrite_encr(svp_file *svmFile,
                            uint8_t key_type,
                            const uint8_t *data,
                            uint32_t data_len);
void sda_crypto_fread_encr(svp_file *svmFile,
                           uint8_t key_type,
                           uint8_t *data,
                           uint32_t max_data_len);

#endif