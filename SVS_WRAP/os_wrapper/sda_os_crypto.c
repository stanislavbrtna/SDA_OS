/*
Copyright (c) 2022 Stanislav Brtna

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

#include "sda_os_wrapper.h"

extern sdaSvmMetadata svmMeta;

uint8_t sda_os_crypto_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  // #!#### Text obfuscation

  // #!##### Unlock overlay init
  // #!    sys.cr.unLockInit();
  // #!Creates Pin unlock overlay
  // #!
  // #!Return: [num] overlay ID, 0 when error
  if (sysFuncMatch(argS->callId, "unLockInit", s)) {
    if (sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (sda_crypto_get_lock()) {
      if(svpSGlobal.usePinForLock) {
        result->value.val_u = pin_overlay_init(0);
      } else {
        result->value.val_u = password_overlay_init();
      }
    } else {
      result->value.val_u = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Unlock overlay update
  // #!    sys.cr.update([num] ovId);
  // #!Updates unlock overlay
  // #!
  // #!Return: None
  if (sysFuncMatch(argS->callId, "update", s)) {
    argType[1] = SVS_TYPE_NUM;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if(svpSGlobal.usePinForLock) {
      pin_overlay_update(argS->arg[1].val_u);
    } else {
      password_overlay_update(argS->arg[1].val_u);
    }
    
    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Unlock overlay get ok
  // #!    sys.cr.getOk([num] ovId);
  // #!Gets if unlock was successfull
  // #!
  // #!Return: [num] 1 - unlock success, 2 - unlock canceled
  if (sysFuncMatch(argS->callId, "getOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if(svpSGlobal.usePinForLock) {
      result->value.val_u = pin_overlay_get_ok(argS->arg[1].val_u);
    } else {
      result->value.val_u = password_overlay_get_ok(argS->arg[1].val_u);
    }
    
    if (result->value.val_u == 1) {
      svmSetCryptoUnlock(1);
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Unlock overlay clear ok
  // #!    sys.cr.clrOk([num] ovId);
  // #!Creates unlock overlay
  // #!
  // #!Return: None
  if (sysFuncMatch(argS->callId, "clrOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if(svpSGlobal.usePinForLock) {
      pin_overlay_clear_ok(argS->arg[1].val_u);
    } else {
      password_overlay_clear_ok(argS->arg[1].val_u);
    }
    
    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Get if is locked
  // #!    sys.cr.getLock();
  // #!Gets if current app can use PIN key
  // #!
  // #!Return: [num] 1 - crypto unlocked, 0 - crypto locked
  if (sysFuncMatch(argS->callId, "getLock", s)) {
    if (sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_u = svmGetCryptoUnlock();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Get if crypto is set up
  // #!    sys.cr.enabled();
  // #!Gets if crypto is enabled. (Device is has keys loaded.)
  // #!
  // #!Return: [num] 1 - crypto enabled, 0 - crypto disabled
  if (sysFuncMatch(argS->callId, "enabled", s)) {
    if (sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = sda_crypto_get_if_after_dkl();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Load custom key string
  // #!    sys.cr.loadKey([str]key, [str]password);
  // #!Loads custom string as a crypto key
  // #!
  // #!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "loadKey", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if (sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    result->value.val_s = sda_crypto_load_usr_key(s->stringField + argS->arg[1].val_str,
                                                  s->stringField + argS->arg[2].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Derive user key from password
  // #!    sys.cr.deriveKey([str]password);
  // #!Loads derived key as a user key.
  // #!
  if (sysFuncMatch(argS->callId, "deriveKey", s)) {
    argType[1] = SVS_TYPE_STR;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    sda_crypto_derive_usr_key(s->stringField + argS->arg[1].val_str);

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Generate keystring
  // #!    sys.cr.genKey([str]password);
  // #!Generates custom keystring.
  // #!
  // #!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "genKey", s)) {
    argType[1] = SVS_TYPE_STR;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    uint8_t *dest;
    uint16_t str_id;

    dest = strNewPLen(98, &str_id, s);

    sda_crypto_generate_usr_key(dest, s->stringField + argS->arg[1].val_str);

    result->value.val_str = str_id;
    result->type = SVS_TYPE_STR;
    return 1;
  }

  // #!##### Clear User key
  // #!    sys.cr.clearKey();
  // #!Clears usr key from memory.
  // #!
  // #!Return: None
  if (sysFuncMatch(argS->callId, "clearKey", s)) {
    if (sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    sda_crypto_clear_usr_key();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Set key type
  // #!    sys.cr.setKey([num]keytype);
  // #!Sets given key for crypto operations.
  // #!0 - Device encryption key, 1 - user encryption key
  // #!
  // #!Return: none
  if (sysFuncMatch(argS->callId, "setKey", s)) {
    argType[1] = SVS_TYPE_NUM;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if(svmGetCryptoUnlock() == 0 && argS->arg[1].val_s == SDA_KEY_PIN) {
      errSoft((uint8_t *)"sda_os_crypto_wrapper: PIN_KEY is not available.", s);
      return 0;
    }
    svmMeta.cryptoKey = argS->arg[1].val_s;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Lock
  // #!    sys.cr.lock();
  // #!Locks sda encryption
  // #!
  // #!Return: None
  if (sysFuncMatch(argS->callId, "lock", s)) {
    if (sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (sda_crypto_get_if_after_dkl()) {
      svmSetCryptoUnlock(0);
    }
    return 1;
  }

  // #!##### Encrypt file
  // #!    sys.cr.encrypt([str]fname);
  // #!Encrypts file.
  // #!
  // #!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "encrypt", s)) {
    argType[1] = SVS_TYPE_STR;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (sda_crypto_get_if_after_dkl()) {
      result->value.val_u = sda_encrypt(s->stringField + argS->arg[1].val_str, svmMeta.cryptoKey);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Decrypt file
  // #!    sys.cr.decrypt([str]fname);
  // #!Encrypts file.
  // #!
  // #!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "decrypt", s)) {
    argType[1] = SVS_TYPE_STR;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (sda_crypto_get_if_after_dkl()) {
      result->value.val_u = sda_decrypt(s->stringField + argS->arg[1].val_str, svmMeta.cryptoKey);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Get TOTP
  // #!    sys.cr.getTotp([str]secret, [num]time_offset_s);
  // #!Returns OTP auth code for given secret.
  // #!
  // #!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "getTotp", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_NUM;
    if (sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    result->value.val_u =
        sda_crypto_generate_totp(s->stringField + argS->arg[1].val_str, argS->arg[2].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Get TOTP Remaining time
  // #!    sys.cr.getTRem([num]time_offset_s);
  // #!Returns for how long the OTP code is valid, in seconds.
  // #!
  // #!Return: [num] time_in_s
  if (sysFuncMatch(argS->callId, "getTRem", s)) {
    argType[1] = SVS_TYPE_NUM;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_u =
        sda_crypto_totp_remaining(argS->arg[1].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  // #!##### Encrypt string
  // #!    sys.cr.encryptStr([str]source);
  // #!Encrypts given string.
  // #!
  // #!Return: [str] encryptedString
  if (sysFuncMatch(argS->callId, "encryptStr", s)) {
    argType[1] = SVS_TYPE_STR;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (sda_crypto_get_if_after_dkl()) {
      uint8_t *dest;
      uint16_t str_id;
      uint32_t len = sda_strlen(s->stringField + argS->arg[1].val_str) * 2 + 1 + 12 * 2;

      dest = strNewPLen(len, &str_id, s);
      sda_encrypt_string(
          (uint8_t *)(s->stringField + argS->arg[1].val_str), dest, len, svmMeta.cryptoKey);

      result->value.val_str = str_id;
    } else {
      result->value.val_str = strNew((uint8_t *)"", s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  // #!##### Decrypt string
  // #!    sys.cr.decryptStr([str]source);
  // #!Decrypts given string.
  // #!
  // #!Return: [str] decryptedString
  if (sysFuncMatch(argS->callId, "decryptStr", s)) {
    argType[1] = SVS_TYPE_STR;
    if (sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (sda_crypto_get_if_after_dkl()) {
      uint8_t *dest;
      uint16_t str_id;
      uint32_t len = sda_strlen(s->stringField + argS->arg[1].val_str) * 2 + 1 + 12 * 2;

      dest = strNewPLen(len, &str_id, s);

      sda_decrypt_string(
          (uint8_t *)(s->stringField + argS->arg[1].val_str), dest, len, svmMeta.cryptoKey);

      result->value.val_str = str_id;
    } else {
      result->value.val_str = strNew((uint8_t *)"", s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  return 0;
}