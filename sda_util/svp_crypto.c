#include "svp_crypto.h"

uint8_t svp_crypto_key[KEY_LEN_MAX];
uint8_t svp_crypto_password[PASS_LEN_MAX];
uint8_t svp_crypto_unlocked;
uint8_t svp_crypto_set_up;

uint8_t svp_successfull_unlock;


void svp_crypto_init() {
  uint16_t i = 0;
  uint8_t default_password[] = "def";

  if (rtc_read_password(svp_crypto_password)) {
    for(i = 0; default_password[i] != 0; i++) {
      svp_crypto_password[i] = default_password[i];
    }
    svp_crypto_password[i] = 0;
    svp_crypto_set_up = 0;
  } else {
    svp_crypto_set_up = 1;
  }
  
  svp_crypto_unlocked = 0;
  svp_successfull_unlock = 0;
  return;
}


uint8_t svp_crypto_get_if_set_up() {
  return svp_crypto_set_up;
}


uint8_t svp_crypto_get_lock() {
  return svp_crypto_unlocked;
}


uint8_t svp_crypto_unlock(uint8_t * password) {
  static uint8_t fails;
  uint16_t i = 0;

  if (fails >= 5) {
    return 3;
  }

  while (svp_crypto_password[i] != 0) {
    if ((password[i] == 0) && (svp_crypto_password[i] != 0)) {
      svp_crypto_unlocked = 0;
      fails++;
      return 2;
    }
    if (svp_crypto_password[i] != password[i]) {
      svp_crypto_unlocked = 0;
      fails++;
      return 2;
    }
    i++;
  }

  if (password[i] != 0) {
    fails++;
    return 2;
  }

  svp_crypto_unlocked = 1;
  svp_successfull_unlock = 1;
  fails = 0;
  svp_crypto_set_pass_as_key();
  return 0;
}

uint8_t svp_crypto_unlock_nopass() {
  
  if(svp_successfull_unlock == 0) {
    return 1;
  }
  
  svp_crypto_unlocked = 1;
  svp_crypto_set_pass_as_key();
  return 0;
}


void svp_crypto_lock() {
  svp_crypto_unlocked = 0;
}


uint8_t svp_crypto_change_password(uint8_t * new_pass) {
  uint16_t i;

  if (!svp_crypto_unlocked) {
    return 1;
  }

  for(i = 0; (new_pass[i] != 0) && (i < PASS_LEN_MAX - 1); i++) {
    svp_crypto_password[i] = new_pass[i];
  }
  svp_crypto_password[i] = 0;

  rtc_write_password(svp_crypto_password);
  svp_crypto_set_up = 1;

  return 0;
}


uint8_t svp_crypto_set_key(uint8_t * new_key) {
  uint16_t i;

  if (!svp_crypto_unlocked) {
    return 1;
  }

  for(i = 0; (new_key[i] != 0) && (i < KEY_LEN_MAX - 1); i++) {
    svp_crypto_key[i] = new_key[i];
  }
  svp_crypto_key[i] = 0;

  return 0;
}


uint8_t svp_crypto_set_pass_as_key() {
  if (!svp_crypto_unlocked) {
    return 1;
  }

  return svp_crypto_set_key(svp_crypto_password);
}


uint8_t svp_crypto_load_key_to_str(uint8_t * fname, uint8_t* str) {
  svp_file source;
  uint8_t nextchar;
  uint32_t i;

  if (!svp_crypto_unlocked) {
    return 1;
  }

  svp_crypto_stream_init();

  if(!svp_fopen_rw(&source, fname)) {
    printf("svp_crypto_load_key_to_str: fopen error!\n");
    return 1;
  }

  //read
  if (svp_feof(&source)) {
    printf("svp_crypto_load_key_to_str: file empty!\n");
    return 1;
  }

  nextchar = svp_fread_u8(&source);

  if (svp_feof(&source)) {
    return 1;
  }

  i = 0;
  while(!svp_feof(&source)) {
    str[i] = svp_crypto_stream_decrypt(nextchar);
    nextchar = svp_fread_u8(&source);
    i++;
    
    if (i == KEY_LEN_MAX - 1) {
      break;
    }
  }
  str[i] = 0;

  svp_fclose(&source);
  return 0;
}


uint8_t svp_crypto_write_keyfile(uint8_t *fname, uint8_t *key) {
  svp_file source;
  uint32_t i;

  if (!svp_crypto_unlocked) {
    printf("svp_crypto_write_keyfile: error: crypto locked!\n");
    return 1;
  }

  if(!svp_fopen_rw(&source, fname)) {
    printf("svp_crypto_write_keyfile: error: fopen failed!\n");
    return 1;
  }

  svp_crypto_stream_init();

  svp_fseek(&source, 0);
  svp_truncate(&source);

  i = 0;
  while(i < KEY_LEN_MAX) {
    svp_fwrite_u8(&source, svp_crypto_stream_encrypt(key[i]));
    i++;
  }

  svp_fclose(&source);

  return 0;
}


uint8_t svp_crypto_load_keyfile(uint8_t * fname) {
  uint8_t new_key[KEY_LEN_MAX];

  if (!svp_crypto_unlocked) {
    return 1;
  }

  if(svp_crypto_load_key_to_str(fname, new_key)) {
    return 2;
  }

  svp_crypto_set_key(new_key);

  return 0;
}


uint8_t svp_crypto_generate_keyfile(uint8_t * fname) {
  uint32_t i;
  uint8_t key[KEY_LEN_MAX];
  uint8_t val = 0;
  uint8_t retval = 0;
  uint8_t unlock_pre = 0;

  if(svp_fexists(fname)) {
    return 1;
  }

  for(i = 0; (i < KEY_LEN_MAX - 1); i++) {
    do {
      val = (uint8_t) svp_random();
    } while(val == 0);

    key[i] = val;
  }

  unlock_pre = svp_crypto_unlocked;

  svp_crypto_unlocked = 1;

  retval = svp_crypto_write_keyfile(fname, key);

  svp_crypto_unlocked = unlock_pre;

  return retval;
}


static uint32_t crypto_counter;
static uint8_t crypto_prev_char;
static uint32_t crypto_key_len;

uint8_t svp_crypto_stream_init() {
  uint32_t i;

  if (!svp_crypto_unlocked) {
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

  return 0;
}


uint8_t svp_crypto_stream_encrypt(uint8_t c) {
  if (!svp_crypto_unlocked) {
    return 1;
  }

  crypto_prev_char = (c + svp_crypto_key[crypto_counter % crypto_key_len] + crypto_prev_char) % 256;
  crypto_counter++;
  return crypto_prev_char;
}


uint8_t svp_crypto_stream_decrypt(uint8_t c) {
  uint8_t decrChar = 0;
  if (!svp_crypto_unlocked) {
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

  if (!svp_crypto_unlocked) {
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

  if (!svp_crypto_unlocked) {
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


// taken from somewhere in the internetz
uint32_t crc32b(uint8_t *message) {
   int32_t i, j;
   uint32_t byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (message[i] != 0) {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}


uint32_t svp_crypto_get_key_crc(uint8_t *fname) {
  uint8_t new_key[KEY_LEN_MAX];
  uint8_t unlockPre = 0;

  unlockPre = svp_crypto_unlocked;

  svp_crypto_unlocked = 1;
  svp_crypto_set_pass_as_key();

  svp_crypto_load_key_to_str(fname, new_key);

  svp_crypto_unlocked = unlockPre;
  
  return crc32b(new_key);

}


uint8_t svp_crypto_reencrypt_key(uint8_t *fname, uint8_t *oldpass, uint8_t *newpass) {
  uint8_t new_key[KEY_LEN_MAX];
  uint32_t crc;

  if (!svp_crypto_unlocked) {
    return 1;
  }

  svp_crypto_set_key(oldpass);
  svp_crypto_load_key_to_str(fname, new_key);

  // write & encrypt
  svp_crypto_set_key(newpass);
  svp_crypto_write_keyfile(fname, new_key);

  crc = svp_crypto_get_key_crc(fname);
  if (crc != crc32b(new_key)) {
    printf("Error: CRC does not match! CRC before: %u, after : %u\n", (unsigned int)crc32b(new_key), (unsigned int)crc);
  }

  return 0;
}


void svp_printkey(uint8_t *key) {
  uint32_t i = 0;
  printf("Key:\n");
  while(key[i] != 0) {
    printf("%u\n", key[i]);
    i++;
  }
}


void svp_crypto_test() {
  puts("crypto test begin");
  // non-valid unlock
  if (!svp_crypto_unlock((uint8_t *)"notpass")) {
    puts("unlocked with wrong password");
    return;
  }

  // valid unlock
  if (svp_crypto_unlock((uint8_t *)"def")) {
    puts("failed to unlock");
    return;
  }

  // change password
  printf("change password: old:%s new:", svp_crypto_password);
  svp_crypto_change_password((uint8_t *)"new password");
  printf("%s\n", svp_crypto_password);

  // set key
  printf("change key: old:%s new:", svp_crypto_key);
  svp_crypto_set_key((uint8_t *)"new keyx");
  printf("%s\n", svp_crypto_key);

  // generate
  svp_crypto_generate_keyfile((uint8_t *)"keyfile.svk");

  // load
  svp_crypto_load_keyfile((uint8_t *)"keyfile.svk");

  // encrypt
  svp_encrypt((uint8_t *)"testfile.txt");

  puts("encrypted");
  getchar();
  // decrypt
  svp_decrypt((uint8_t *)"testfile.txt");

  puts("crypto test end");
}
