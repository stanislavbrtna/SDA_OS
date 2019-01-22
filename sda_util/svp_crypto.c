#include "svp_crypto.h"

uint8_t svp_crypto_key[KEY_LEN_MAX];
uint8_t svp_crpyto_unlocked;

void svp_crypto_init() {
	uint16_t i = 0;
	uint8_t default_key[] = "def";

	for(i = 0; default_key[i] != 0; i++) {
		svp_crypto_key[i] = default_key[i];
	}
	svp_crypto_key[i] = 0;
	svp_crpyto_unlocked = 0;
	return;
}

uint8_t svp_crypto_unlock(uint8_t * key) {
	static uint8_t fails;
	uint16_t i = 0;

	if (fails >= 5) {
		return 3;
	}

	while (svp_crypto_key[i] != 0) {
		if ((key[i] == 0) && (svp_crypto_key[i] != 0)) {
			svp_crpyto_unlocked = 0;
			fails++;
			return 2;
		}
		if (svp_crypto_key[i] != key[i]) {
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
	return 0;
}

void svp_crypto_lock() {
	svp_crpyto_unlocked = 0;
}

uint8_t svp_crypto_change_key(uint8_t * new_key) {

	if (!svp_crpyto_unlocked) {
		return 1;
	}

	for(uint16_t i = 0; (new_key[i] != 0) && (i < KEY_LEN_MAX); i++) {
		svp_crypto_key[i] = new_key[i];
	}

	return 0;
}

uint8_t svp_encrypt(uint8_t * fname) {
	svp_file source;
	uint8_t nextchar;
	uint8_t prevVal;
	uint32_t keyLen;
	uint32_t i;

	if (!svp_crpyto_unlocked) {
		return 1;
	}

	if(!svp_fopen_rw(&source, fname)) {
		return 1;
	}

	i = 0;

	while(svp_crypto_key[i] != 0){
		i++;
	}
	keyLen = i;

	if (svp_feof(&source)){
		return 0;
	}
	//read
	nextchar = svp_fread_u8(&source);

	i = 0;
	prevVal = 0;
	while(!svp_feof(&source)) {
		prevVal = (nextchar + svp_crypto_key[i % keyLen] + prevVal) % 256;
		//rewind
		svp_fseek(&source, svp_ftell(&source) - 1);
		//write
		svp_fwrite_u8(&source, prevVal);
		nextchar = svp_fread_u8(&source);
		i++;
	}

	svp_fclose(&source);

	return 0;
}

uint8_t svp_decrypt(uint8_t * fname) {
	svp_file source;
	uint8_t nextchar;
	uint8_t prevVal;
	uint32_t keyLen;
	uint32_t i;

	if (!svp_crpyto_unlocked) {
		return 1;
	}

	if(!svp_fopen_rw(&source, fname)) {
		return 1;
	}

	i = 0;
	keyLen = 0;
	while(svp_crypto_key[i] != 0) {
		i++;
	}
	keyLen = i;

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
		prevVal = (nextchar - svp_crypto_key[i % keyLen] - prevVal) % 256;
		//rewind
		svp_fseek(&source, svp_ftell(&source) - 1);
		//write
		svp_fwrite_u8(&source, prevVal);
		prevVal = nextchar;
		nextchar = svp_fread_u8(&source);
		i++;
	}

	svp_fclose(&source);

	return 0;
}
