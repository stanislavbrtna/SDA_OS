#include "svp_crypto.h"

uint8_t svp_crypto_key[KEY_LEN_MAX];
uint8_t svp_crypto_password[PASS_LEN_MAX];
uint8_t svp_crpyto_unlocked;

void svp_crypto_init() {
  uint16_t i = 0;
  uint8_t default_password[] = "def";

  if (rtc_read_password(svp_crypto_key)) {
    for(i = 0; default_password[i] != 0; i++) {
      svp_crypto_password[i] = default_password[i];
    }
    svp_crypto_password[i] = 0;
  }

  svp_crpyto_unlocked = 0;
  return;
}

uint8_t svp_crypto_unlock(uint8_t * key) {
  static uint8_t fails;
  uint16_t i = 0;

  if (fails >= 5) {
    return 3;
  }

  while (svp_crypto_password[i] != 0) {
    if ((key[i] == 0) && (svp_crypto_password[i] != 0)) {
      svp_crpyto_unlocked = 0;
      fails++;
      return 2;
    }
    if (svp_crypto_password[i] != key[i]) {
      svp_crpyto_unlocked = 0;
      fails++;
      return 2;
    }
    i++;
  }

  if (key[i] != 0) {
    fails++;
    return 2;
  }

  svp_crpyto_unlocked = 1;
  fails = 0;
  svp_crypto_set_pass_as_key();
  return 0;
}


void svp_crypto_lock() {
  svp_crpyto_unlocked = 0;
}


uint8_t svp_crypto_change_password(uint8_t * new_pass) {
  uint16_t i;

  if (!svp_crpyto_unlocked) {
    return 1;
  }

  for(i = 0; (new_pass[i] != 0) && (i < PASS_LEN_MAX - 1); i++) {
    svp_crypto_password[i] = new_pass[i];
  }
  svp_crypto_password[i] = 0;

  rtc_write_password(svp_crypto_password);

  return 0;
}


uint8_t svp_crypto_set_key(uint8_t * new_key) {
  uint16_t i;

  if (!svp_crpyto_unlocked) {
    return 1;
  }

  for(i = 0; (new_key[i] != 0) && (i < KEY_LEN_MAX - 1); i++) {
    svp_crypto_key[i] = new_key[i];
  }
  svp_crypto_key[i] = 0;

  return 0;
}


uint8_t svp_crypto_set_pass_as_key() {
  if (!svp_crpyto_unlocked) {
    return 1;
  }

  svp_crypto_set_key(svp_crypto_password);
}


uint8_t svp_crypto_load_keyfile(uint8_t * fname) {
  svp_file source;
  uint8_t new_key[KEY_LEN_MAX];
  uint8_t nextchar;
  uint8_t prevVal;
  uint32_t keyLen;
  uint32_t i;

  if (!svp_crpyto_unlocked) {
    return 1;
  }

  svp_crypto_stream_init();

  if(!svp_fopen_rw(&source, fname)) {
    return 1;
  }

  //read
  if (svp_feof(&source)) {
    return 0;
  }

  nextchar = svp_fread_u8(&source);

  if (svp_feof(&source)) {
    return 0;
  }

  i = 0;
  prevVal = 0;
  while(!svp_feof(&source)) {
    new_key[i] = svp_crypto_stream_decrypt(nextchar);
    nextchar = svp_fread_u8(&source);
    i++;
    
    if (i == KEY_LEN_MAX - 1) {
      new_key[i] = 0;
      break;
    }
  }

  svp_crypto_set_key(new_key);

  svp_fclose(&source);
  return 0;
}


uint8_t svp_crypto_generate_keyfile(uint8_t * fname) {
  svp_file source;
  uint32_t i;

  if(svp_fexists(fname)) {
    return 1;
  }

  if(!svp_fopen_rw(&source, fname)) {
    return 2;
  }

  for(i = 0; (i < KEY_LEN_MAX - 1); i++) {
    svp_fwrite_u8(&source, (uint8_t) svp_random());
  }
  svp_fclose(&source);

  return 0;
}

static uint32_t crypto_counter;
static uint8_t crypto_prev_char;
static uint32_t crypto_key_len;

uint8_t svp_crypto_stream_init() {
  uint32_t i;

  if (!svp_crpyto_unlocked) {
    return 1;
  }

  i = 0;

  while(svp_crypto_key[i] != 0){
    i++;
  }
  crypto_key_len = i;

  if (crypto_key_len == 0) {
    printf("crypto: Key must not be empty!\n");
    return 1;
  }

  crypto_prev_char = 0;
  crypto_counter = 0;
}


uint8_t svp_crypto_stream_encrypt(uint8_t c) {
  if (!svp_crpyto_unlocked) {
    return 1;
  }

  crypto_prev_char = (c + svp_crypto_key[crypto_counter % crypto_key_len] + crypto_prev_char) % 256;
  crypto_counter++;
  return crypto_prev_char;
}


uint8_t svp_crypto_stream_decrypt(uint8_t c) {
  uint8_t decrChar = 0;
  if (!svp_crpyto_unlocked) {
    return 1;
  }

  decrChar = (c - svp_crypto_key[crypto_counter % crypto_key_len] - crypto_prev_char) % 256;
  crypto_counter++;
  crypto_prev_char = c;
  return decrChar;
}


uint8_t svp_encrypt(uint8_t * fname) {
  svp_file source;
  uint8_t nextchar;
  uint8_t encr_char;

  if (!svp_crpyto_unlocked) {
    return 1;
  }

  if(!svp_fopen_rw(&source, fname)) {
    return 1;
  }

  if (svp_feof(&source)){
    return 0;
  }
  
  svp_crypto_stream_init();
  
  //read
  nextchar = svp_fread_u8(&source);

  while(!svp_feof(&source)) {
    encr_char = svp_crypto_stream_encrypt(nextchar);  
    svp_fseek(&source, svp_ftell(&source) - 1);
    svp_fwrite_u8(&source, encr_char);
    nextchar = svp_fread_u8(&source);
  }

  svp_fclose(&source);
  return 0;
}


uint8_t svp_decrypt(uint8_t * fname) {
  svp_file source;
  uint8_t nextchar;
  uint8_t decrChar;

  if (!svp_crpyto_unlocked) {
    return 1;
  }

  if(!svp_fopen_rw(&source, fname)) {
    return 1;
  }

  if (svp_feof(&source)) {
    return 0;
  }

  svp_crypto_stream_init();

  nextchar = svp_fread_u8(&source);

  if (svp_feof(&source)) {
    return 0;
  }

  while(!svp_feof(&source)) {
    decrChar = svp_crypto_stream_decrypt(nextchar);
    svp_fseek(&source, svp_ftell(&source) - 1);
    svp_fwrite_u8(&source, decrChar);
    nextchar = svp_fread_u8(&source);
  }

  svp_fclose(&source);
  return 0;
}


void svp_printkey(uint8_t *key) {
  return;
}

void svp_crypto_test() {
  puts("crypto test begin");
  // non-valid unlock
  if (!svp_crypto_unlock("notpass")) {
    puts("unlocked with wrong password");
    return;
  }

  // valid unlock
  if (svp_crypto_unlock("def")) {
    puts("failed to unlock");
    return;
  }

  // change password
  printf("change password: old:%s new:", svp_crypto_password);
  svp_crypto_change_password("new password");
  printf("%s\n", svp_crypto_password);

  // set key
  printf("change key: old:%s new:", svp_crypto_key);
  svp_crypto_set_key("new keyx");
  printf("%s\n", svp_crypto_key);

  // generate
  svp_crypto_generate_keyfile("keyfile.svk");

  // load
  svp_crypto_load_keyfile("keyfile.svk");

  // encrypt
  svp_encrypt("testfile.txt");

  puts("encrypted");
  getchar();
  // decrypt
  svp_decrypt("testfile.txt");

  puts("crypto test end");
}