#include "svp_crypto.h"
#include "cifra/chacha20poly1305.h"
#include "cifra/salsa20.h"
#include "cifra/pbkdf2.h"
#include "cifra/sha2.h"

// device encryption key
static uint8_t svp_crypto_dek[KEY_LEN];

// user encryption key
static uint8_t svp_crypto_usr[KEY_LEN];

// key encryption key
static uint8_t svp_crypto_kek[KEY_LEN];

// TODO: random salt
const uint8_t kek_salt[] = SDA_PASSWORD_SALT;

// Flags that the SDA is after device key load
static uint8_t svp_crypto_after_dkl;

// kek & dek in memory
static uint8_t svp_crypto_unlocked;

// dek available in conf file
static uint8_t svp_crypto_set_up;

// local headers
uint8_t svp_crypto_store_dek();

int constant_time_memcmp(const void *a, const void *b, size_t len) {
  const uint8_t *ua = a;
  const uint8_t *ub = b;
  int result = 0;
  for (size_t i = 0; i < len; i++) {
    result |= (ua[i] ^ ub[i]);
  }
  return result == 0 ? 0 : -1;
}

void copy_key(uint8_t *source, uint8_t *dest) {
  uint32_t i = 0;

  while (i < KEY_LEN) {
    dest[i] = source[i];
    i++;
  }
}

void clear_key(uint8_t *dest) {
  uint32_t i = 0;

  while (i < KEY_LEN) {
    dest[i] = 0;
    i++;
  }
}

void print_hex(const char *label, const uint8_t *data, size_t len) {
  printf("%s: ", label);
  for (size_t i = 0; i < len; i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
}

void derive_key_from_pin(const char *pin, const uint8_t *salt, uint8_t *out_key) {
  // High iteration count to slow down brute-force attacks
  uint32_t iterations = 15000;

  // Derive a strong 32-byte key from a weak PIN string
  cf_pbkdf2_hmac((const uint8_t *)pin,
                 strlen(pin),
                 salt,
                 16, // 16-byte random salt
                 iterations,
                 out_key,
                 32, // Output 32-byte key
                 &cf_sha256);
}

void generate_rnd_array(uint8_t *dest, size_t len) {
  for (size_t i = 0; i < len; i++) {
    dest[i] = (uint8_t)svp_random();
  }
}

void sda_crypto_init() {

  svp_crypto_after_dkl = 0;
  svp_crypto_set_up = 0;
  svp_crypto_unlocked = 0;
  return;
}

uint8_t sda_crypto_get_lock() { return svp_crypto_unlocked; }

uint8_t sda_crypto_get_if_set_up() { return svp_crypto_set_up; }

uint8_t sda_crypto_get_if_after_dkl() { return svp_crypto_after_dkl; }

// TODO: move imports
void derive_key_from_pin(const char *pin, const uint8_t *salt, uint8_t *out_key);
int base32_decode(const char *src, uint8_t *dst, int dst_len);
void base32_encode(const uint8_t *input, size_t input_len, char *output);
size_t get_base32_encoded_length(size_t input_length);

uint8_t sda_crypto_unlock(uint8_t *password) {
  static uint8_t fails;
  uint16_t i = 0;
  uint8_t derived_key[KEY_LEN];

  if (fails >= 5) {
    return 3;
  }

  derive_key_from_pin(password, kek_salt, derived_key);

  if (svp_crypto_after_dkl) {
    if (constant_time_memcmp(derived_key, svp_crypto_kek, KEY_LEN)) {
      svp_crypto_unlocked = 0;
      // printf("Wrong pass after DFU\n");
      fails++;
      return 1;
    }
  } else {

    uint8_t dirbuf[258];
    sda_conf conffile;

    svp_getcwd(dirbuf, 256);
    svp_switch_main_dir();

    if (sda_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
      printf("Failed to open cfg file\n");
    }

    uint8_t dek_base32[128];
    uint8_t dek_and_tag[32 + 16];
    uint8_t dek_nonce32[128];
    uint8_t dek_nonce[12];
    uint8_t dek_decoded[KEY_LEN];

    sda_conf_key_read(&conffile, (uint8_t *)"key_dek", dek_base32, sizeof(dek_base32));
    sda_conf_key_read(&conffile, (uint8_t *)"key_nonce", dek_nonce32, sizeof(dek_nonce32));

    // printf("Dek and tag: %s\n", dek_base32);
    base32_decode(dek_base32, dek_and_tag, KEY_LEN + 16);
    base32_decode(dek_nonce32, dek_nonce, 12);
    // print_hex("Dek&tag decoded unlock", dek_and_tag, KEY_LEN + 16);

    if (cf_chacha20poly1305_decrypt(
            derived_key, dek_nonce, NULL, 0, dek_and_tag, 32, dek_and_tag + KEY_LEN, dek_decoded)) {
      svp_crypto_unlocked = 0;
      fails++;
      svp_chdir(dirbuf);
      // printf("Wrong pass before DFU\n");
      return 2;
    }

    copy_key(dek_decoded, svp_crypto_dek);
    copy_key(derived_key, svp_crypto_kek);

    svp_crypto_after_dkl = 1;
    svp_chdir(dirbuf);
  }

  svp_crypto_unlocked = 1;
  fails = 0;
  // printf("gud pass aft_dfu:%u\n", svp_crypto_after_dkl);
  return 0;
}

// used to unlock crypto for app that was suspended previously
uint8_t sda_crypto_unlock_nopass() {

  if (svp_crypto_after_dkl == 0) {
    return 1;
  }

  svp_crypto_unlocked = 1;
  return 0;
}

void sda_crypto_lock() { svp_crypto_unlocked = 0; }

uint8_t sda_crypto_change_password(uint8_t *new_pass) {
  uint16_t i;

  if (!svp_crypto_unlocked) {
    return 1;
  }

  derive_key_from_pin(new_pass, kek_salt, svp_crypto_kek);

  svp_crypto_store_dek();

  return 0;
}

void store_val_base32(sda_conf *c, uint8_t *conf_key, uint8_t *data, size_t data_len) {
  uint8_t outbuff[128];

  if (get_base32_encoded_length(data_len) > 128) {
    printf("!data len too long!\n");
    return;
  }

  base32_encode(data, data_len, outbuff);

  sda_conf_key_write(c, conf_key, outbuff);
  printf("%s: writing key %s val: %s\n", __FUNCTION__, conf_key, outbuff);
}

uint8_t svp_crypto_store_dek() {
  uint8_t dirbuf[258];
  sda_conf conffile;

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
  }

  uint8_t key_encr[KEY_LEN + 16];
  uint8_t nonce[12];

  generate_rnd_array(nonce, 12);

  cf_chacha20poly1305_encrypt(
      svp_crypto_kek, nonce, NULL, 0, svp_crypto_dek, KEY_LEN, key_encr, key_encr + KEY_LEN);

  // print_hex("Dek&tag written", key_encr, KEY_LEN + 16);

  store_val_base32(&conffile, "key_dek", key_encr, KEY_LEN + 16);
  store_val_base32(&conffile, "key_nonce", nonce, KEY_LEN);

  sda_conf_close(&conffile);

  svp_chdir(dirbuf);
}

uint8_t svp_crypto_load_user_key(uint8_t *user_key_base32) { return 0; }

uint8_t svp_crypto_load_user_pass(uint8_t *user_pass) { return 0; }

static cf_chacha20_ctx ctx;

uint8_t sda_crypto_encryption_init(uint8_t *nonce, uint8_t keytype) {

  if (!svp_crypto_unlocked) {
    return 1;
  }

  uint8_t *key = &svp_crypto_dek;

  if(keytype == SDA_KEY_USR) {
    key = &svp_crypto_usr;
  }

  cf_chacha20_init(&ctx, key, KEY_LEN, nonce);

  return 0;
}

uint8_t sda_crypto_cypher_block(uint8_t *input, uint8_t *output, size_t len) {
  if (!svp_crypto_unlocked) {
    return 1;
  }

  uint8_t out;

  cf_chacha20_cipher(&ctx, input, output, len);

  return out;
}

uint8_t sda_crypto_stream_encrypt(uint8_t c) {
  if (!svp_crypto_unlocked) {
    return 1;
  }

  uint8_t out;

  cf_chacha20_cipher(&ctx, &c, &out, 1);

  return out;
}

uint8_t sda_crypto_stream_decrypt(uint8_t c) {

  if (!svp_crypto_unlocked) {
    return 1;
  }

  uint8_t out;

  cf_chacha20_cipher(&ctx, &c, &out, 1);

  return out;
}

uint8_t sda_encrypt(uint8_t *fname, uint8_t keytype) {
  svp_file source;

  if (!svp_crypto_unlocked) {
    return 1;
  }

  if (!svp_fopen_rw(&source, fname)) {
    return 1;
  }

  if (svp_feof(&source)) {
    return 0;
  }

  uint8_t nonce[12];

  generate_rnd_array(nonce, 12);

  sda_crypto_encryption_init(nonce, keytype);

  // read
  uint8_t nextchar = svp_fread_u8(&source);

  while (!svp_feof(&source)) {
    uint8_t encr_char = sda_crypto_stream_encrypt(nextchar);
    svp_fseek(&source, svp_ftell(&source) - 1);
    svp_fwrite_u8(&source, encr_char);
    nextchar = svp_fread_u8(&source);
  }
  svp_fwrite(&source, nonce, 12);
  svp_fclose(&source);
  return 0;
}

uint8_t sda_decrypt(uint8_t *fname, uint8_t keytype) {
  svp_file source;

  if (!svp_crypto_unlocked) {
    return 1;
  }

  if (!svp_fopen_rw(&source, fname)) {
    return 1;
  }

  if (svp_feof(&source)) {
    return 0;
  }

  uint8_t nonce[12];

  svp_fseek(&source, svp_get_size(&source));
  svp_fseek(&source, svp_ftell(&source) - 12);

  svp_fread(&source, nonce, 12);

  svp_fseek(&source, 0);

  sda_crypto_encryption_init(nonce, keytype);

  // read
  uint8_t nextchar = svp_fread_u8(&source);

  while (!svp_feof(&source)) {
    uint8_t encr_char = sda_crypto_stream_encrypt(nextchar);
    svp_fseek(&source, svp_ftell(&source) - 1);
    svp_fwrite_u8(&source, encr_char);
    nextchar = svp_fread_u8(&source);
  }

  svp_fseek(&source, svp_ftell(&source) - 13);
  svp_truncate(&source);

  svp_fclose(&source);

  return 0;
}

void sda_crypto_boot_check() {
  uint8_t dirbuf[258];
  sda_conf conffile;

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
  }

  if (sda_conf_key_exists(&conffile, "key_dek") && sda_conf_key_exists(&conffile, "key_nonce")) {
    printf("%s: Encryption is set up in conf.\n", __FUNCTION__);
    svp_crypto_set_up = 1;
  } else {
    printf("%s: Encryption is not set up in conf.\n", __FUNCTION__);
  }

  svp_chdir(dirbuf);
}

void sda_crypto_remove() {
  uint8_t dirbuf[258];
  sda_conf conffile;

  svp_getcwd(dirbuf, 256);
  svp_switch_main_dir();

  if (sda_conf_open(&conffile, (uint8_t *)"svp.cfg") == 0) {
    printf("Failed to open cfg file\n");
  }

  clear_key(svp_crypto_dek);
  clear_key(svp_crypto_kek);
  clear_key(svp_crypto_usr);

  sda_conf_key_remove(&conffile, "key_dek");
  sda_conf_key_remove(&conffile, "key_nonce");

  printf("%s: Encryption is removed, keys are deleted.\n", __FUNCTION__);

  svp_crypto_set_up = 0;
  svp_crypto_after_dkl = 0;

  svp_chdir(dirbuf);
}

void sda_crypto_reset(uint8_t *new_pass) {

  generate_rnd_array(svp_crypto_dek, KEY_LEN);
  svp_crypto_unlocked = 1;
  sda_crypto_change_password(new_pass);

  printf("%s: Password reset successfull!\n", __FUNCTION__);
  svp_crypto_unlocked = 0;
  svp_crypto_after_dkl = 1;
  svp_crypto_set_up = 1;
}

uint8_t sda_crypto_load_usr_key(uint8_t *base32_enc_key, uint8_t *password) {
  uint8_t nonce_dek_and_tag[12 + 32 + 16];
  uint8_t key_decrypted[KEY_LEN];

  uint8_t derived_key[KEY_LEN];
  derive_key_from_pin(password, kek_salt, derived_key);

  base32_decode(base32_enc_key, nonce_dek_and_tag, sizeof(nonce_dek_and_tag));

  //print_hex("nonceDekTag", nonce_dek_and_tag, sizeof(nonce_dek_and_tag));
  //print_hex("derived key", derived_key, KEY_LEN);

  if (cf_chacha20poly1305_decrypt(derived_key,
                                  nonce_dek_and_tag,
                                  NULL,
                                  0,
                                  nonce_dek_and_tag + 12,
                                  KEY_LEN,
                                  nonce_dek_and_tag + KEY_LEN + 12,
                                  key_decrypted)) {
    return 1;
  }

  //print_hex("loaded key", key_decrypted, KEY_LEN);

  copy_key(key_decrypted, svp_crypto_usr);

  return 0;
}

uint8_t sda_crypto_generate_usr_key(uint8_t *base32_output_buffer, uint8_t *password) {
  uint8_t nonce_dek_and_tag[12 + 32 + 16];
  uint8_t derived_key[KEY_LEN];
  uint8_t new_key[KEY_LEN];
  uint8_t new_nonce[12];

  derive_key_from_pin(password, kek_salt, derived_key);

  generate_rnd_array(nonce_dek_and_tag, 12);
  generate_rnd_array(new_key, KEY_LEN);

  //print_hex("generated key", new_key, KEY_LEN);
  //print_hex("derived key", derived_key, KEY_LEN);

  cf_chacha20poly1305_encrypt(derived_key,
                              nonce_dek_and_tag,
                              NULL,
                              0,
                              new_key,
                              KEY_LEN,
                              nonce_dek_and_tag + 12,
                              nonce_dek_and_tag + KEY_LEN + 12);

  //print_hex("nonceDekTag", nonce_dek_and_tag, sizeof(nonce_dek_and_tag));

  base32_encode(nonce_dek_and_tag, sizeof(nonce_dek_and_tag), base32_output_buffer);

  return 0;
}

void sda_crypto_derive_usr_key(uint8_t *password) {
  uint8_t derived_key[KEY_LEN];
  derive_key_from_pin(password, kek_salt, derived_key);
  copy_key(derived_key, svp_crypto_usr);
}

void sda_crypto_test() {

  // puts("crypto test begin");
  // // non-valid unlock
  // if (!sda_crypto_unlock((uint8_t *)"notpass")) {
  //   puts("unlocked with wrong password");
  //   return;
  // }

  // // valid unlock
  // if (sda_crypto_unlock((uint8_t *)"def")) {
  //   puts("failed to unlock");
  //   return;
  // }

  // // change password
  // printf("change password: old:%s new:", svp_crypto_password);
  // sda_crypto_change_password((uint8_t *)"new password");
  // printf("%s\n", svp_crypto_password);

  // // set key
  // //printf("change key: old:%s new:", svp_crypto_key);
  // //svp_crypto_set_key((uint8_t *)"new keyx");
  // //printf("%s\n", svp_crypto_key);

  // // generate
  // svp_crypto_generate_keyfile((uint8_t *)"keyfile.svk");

  // // load
  // svp_crypto_load_keyfile((uint8_t *)"keyfile.svk");

  // // encrypt
  // sda_encrypt((uint8_t *)"testfile.txt");

  // puts("encrypted");
  // getchar();
  // // decrypt
  // sda_decrypt((uint8_t *)"testfile.txt");

  // puts("crypto test end");
}
