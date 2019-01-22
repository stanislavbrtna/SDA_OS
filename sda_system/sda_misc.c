/*
Copyright (c) 2018 Stanislav Brtna

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "sda_misc.h"

// globals
extern uint8_t timeUpdateFlag;
extern volatile uint16_t svsCounter;
extern uint8_t sleepLock;

static gr2EventType keyEvLocal[6];

// here will be misc svs functions originaly in main

uint8_t getKbdKey() {
	if (svpSGlobal.kbdFlag == 1) {
		svpSGlobal.kbdFlag = 0;
		return 1;
	}
	return 0;
}

uint16_t svp_get_uptime() {
  return svpSGlobal.uptime;
}

uint8_t getTimeUpdateFlag() {
  return timeUpdateFlag;
}

void setTimeUpdateFlag() {
  timeUpdateFlag = 1;
}

void showKeyboard() {
  svpSGlobal.kbdVisible = 1;
}

void hideKeyboard() {
  pscg_text_deactivate(&sda_sys_con);
  pscg_text_deactivate(&sda_app_con);
  svpSGlobal.kbdKeyStr[0] = 0;
  svpSGlobal.kbdVisible = 0;
}

void set_svp_counter(uint16_t val) {
  svsCounter = val;
}

uint16_t get_svp_counter() {
  return svsCounter;
}

void svp_set_sleep_lock(uint8_t val) {
	sleepLock = val;
}

void pscg_error_callback(uint8_t *str, gr2context * c) {
	if (c == &sda_sys_con) {
		// error occured in system gui, perform reset
		printf("PSCG Error (system): %s\n", str);
		svs_hardErrHandler();
	} else {
		// error occured inside application
		// kill app, show error
		printf("PSCG Error (app): %s\n", str);
		sdaSvmSetError(str);
		sdaSvmKillApp();
	}
}

void svp_clr_button_ev() {
  svpSGlobal.keyEv[0] = EV_NONE;
  svpSGlobal.keyEv[1] = EV_NONE;
  svpSGlobal.keyEv[2] = EV_NONE;
  svpSGlobal.keyEv[3] = EV_NONE;
  svpSGlobal.keyEv[4] = EV_NONE;
  svpSGlobal.keyEv[5] = EV_NONE;
}

gr2EventType sda_wrap_get_button(uint8_t num) {
	return keyEvLocal[num];
}

void sda_wrap_clear_button(uint8_t num) {
	keyEvLocal[num] = EV_NONE;
}

void sda_store_buttons() {
	for (uint16_t i = 0; i < 6; i++) {
  	keyEvLocal[i] = svpSGlobal.keyEv[i];
  }
}

uint32_t svp_str_add(uint8_t *str, uint8_t *str2) {
  uint32_t i, n;
  i = 0;
  while(str[i] != 0) {
    i++;
  }

  n = 0;
  while(str2[n] != 0) {
    str[i] = str2[n];
    i++;
    n++;
  }
  str[i] = 0;
  return n+1;
}

uint32_t sda_strcp(uint8_t *in, uint8_t *out, uint32_t len) {
  uint32_t x;
  for (x = 0; x < (len - 1); x++) {
    out[x] = in[x];
    if (in[x] == 0) {
      return (x + 1);
    }
  }
  out[len] = 0;
  return 0;
}

// TODO replace this with sda_strcp
void sda_setbuff(uint8_t * source, uint8_t * target) {
	uint16_t x;
	x = 0;
	while(1) {
		if (source[x]!= 0){
			target[x] = source [x];
		} else {
			target[x] = source [x];
			return;
		}
		x++;
	}
}

uint32_t sda_strlen(uint8_t * str) {
  uint32_t len = 0;

  while (str[len] != 0) {
    len++;
  }

  return len;
}

void sda_int_to_str(uint8_t * buff, int32_t val, uint32_t len) {
	uint16_t a;
	uint8_t negative = 0;

	if (val == 0) {
		buff[0] = '0';
		buff[1] = 0;
		return;
	}

	//printf("write-i32-DBG: val: %d\n", val);
	for(uint16_t x = 0; x < len; x++) {
		buff[x] = ' ';
	}
	buff[len - 1] = 0;

	if (val < 0) {
		val *= -1;
		negative = 1;
	}

	a = 1;
	while (0 != val) {
		buff[len - a - 1] = (val % 10 + 48);
		val = val / 10;
		a++;
		if ((uint32_t)a == len){
			buff[0] = 0;
			return;
		}
	}

	if (negative) {
		buff[len - a - 1] = '-';
	}
	//printf("sda_int_to_str-DBG: i1: %s\n", buff);

	while(buff[0] == ' ') {
		for(uint16_t x = 0; x < len ; x++) {
			buff[x] = buff[x + 1];
		}
	}
}
