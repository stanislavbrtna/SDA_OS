#include "sda_crypto_buffers.h"

void sda_crypto_fwrite_encr(svp_file *svmFile,
                            uint8_t key_type,
                            const uint8_t *data,
                            uint32_t data_len) {
  // Get nonce
  uint8_t nonce[12];
  generate_rnd_array(nonce, sizeof(nonce));

  sda_crypto_encryption_init(nonce, key_type);

  svp_fwrite(svmFile, nonce, sizeof(nonce));

  uint8_t buffer[64]; // ChaCha20 block size = 64
  uint32_t bytes_processed = 0;

  while (bytes_processed < data_len) {
    uint32_t chunk_size = data_len - bytes_processed;
    if (chunk_size > sizeof(buffer)) {
      chunk_size = sizeof(buffer);
    }

    sda_crypto_cypher_block(data + bytes_processed, buffer, chunk_size);
    svp_fwrite(svmFile, buffer, chunk_size);

    bytes_processed += chunk_size;
  }
}

void sda_crypto_fread_encr(svp_file *svmFile,
                           uint8_t key_type,
                           uint8_t *data,
                           uint32_t max_data_len) {
  uint8_t nonce[12];
  svp_fread(svmFile, nonce, sizeof(nonce));

  sda_crypto_encryption_init(nonce, key_type);

  uint32_t remaining_file_size = max_data_len;

  if (svp_get_size(svmFile) - sizeof(nonce) < max_data_len) {
    printf("%s: Error, file is smaller than the buffer!\n", __FUNCTION__);
    return 1;
  }

  uint8_t buffer[64];
  uint32_t bytes_processed = 0;

  while (bytes_processed < remaining_file_size) {
    uint32_t chunk_size = (uint32_t)(remaining_file_size - bytes_processed);
    if (chunk_size > sizeof(buffer)) {
      chunk_size = sizeof(buffer);
    }

    svp_fread(svmFile, buffer, chunk_size);
    sda_crypto_cypher_block(buffer, data + bytes_processed, chunk_size);

    bytes_processed += chunk_size;
  }

  return 0;
}