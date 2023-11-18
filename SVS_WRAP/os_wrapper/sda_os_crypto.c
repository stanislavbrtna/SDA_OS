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

uint8_t sda_os_crypto_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {
  uint8_t argType[11];

  //#!#### Text obfuscation


  //#!##### Unlock overlay init
  //#!    sys.cr.unLockInit();
  //#!Creates unlock overlay
  //#!Return: [num] overlay ID, 0 when error
  if (sysFuncMatch(argS->callId, "unLockInit", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (svp_crypto_get_lock() == 0) {
      result->value.val_u = password_overlay_init();
    } else {
      result->value.val_u = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Unlock overlay update
  //#!    sys.cr.update([num] ovId);
  //#!Updates unlock overlay
  //#!Return: None
  if (sysFuncMatch(argS->callId, "update", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    password_overlay_update(argS->arg[1].val_u);
    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Unlock overlay get ok
  //#!    sys.cr.getOk([num] ovId);
  //#!Gets if unlock was successfull
  //#!Return: [num] 1 - unlock success, 2 - unlock canceled
  if (sysFuncMatch(argS->callId, "getOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_u = password_overlay_get_ok(argS->arg[1].val_u);
    if (result->value.val_u == 1) {
      svmSetCryptoUnlock(1);
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Unlock overlay clear ok
  //#!    sys.cr.clrOk([num] ovId);
  //#!Creates unlock overlay
  //#!Return: None
  if (sysFuncMatch(argS->callId, "clrOk", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    password_overlay_clear_ok(argS->arg[1].val_u);
    result->value.val_u = 0;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get if is locked
  //#!    sys.cr.getLock();
  //#!Gets if crypto is unlocked
  //#!Return: [num] 1 - crypto unlocked, 0 - crypto locked
  if (sysFuncMatch(argS->callId, "getLock", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    result->value.val_u = svmGetCryptoUnlock() && svp_crypto_get_lock();
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Loads password as a key
  //#!    sys.cr.loadPass();
  //#!Loads OS password as a key
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "loadPass", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (svmGetCryptoUnlock()) {
      svp_crypto_set_pass_as_key();
      result->value.val_u = 0;
    } else {
      result->value.val_u = 1;
    }
    
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Load custom key string
  //#!    sys.cr.loadStr([str]key);
  //#!Loads custom string as a crypto key
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "loadStr", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (svmGetCryptoUnlock()) {
      result->value.val_u = svp_crypto_set_key(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Load custom keyfile
  //#!    sys.cr.loadKey([str]keyfile);
  //#!Loads custom keyfile as a crypto key
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "loadKey", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (svmGetCryptoUnlock()) {
      result->value.val_u = svp_crypto_load_keyfile(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Load OS keyfile
  //#!    sys.cr.loadOSKey();
  //#!Loads OS keyfile as a crypto key
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "loadOSKey", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (svmGetCryptoUnlock()) {
      result->value.val_u = svp_crypto_load_os_keyfile();
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Generate keyfile
  //#!    sys.cr.genKey([str]keyfile);
  //#!Generates custom keyfile.
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "genKey", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (svmGetCryptoUnlock()) {
      result->value.val_u = svp_crypto_generate_keyfile(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Lock
  //#!    sys.cr.lock();
  //#!Locks sda encryption
  //#!Return: None
  if (sysFuncMatch(argS->callId, "lock", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (svmGetCryptoUnlock()) {
      svmSetCryptoUnlock(0);
      svp_crypto_lock();
    }
    return 1;
  }

  //#!##### Encrypt file
  //#!    sys.cr.encrypt([str]fname);
  //#!Encrypts file.
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "encrypt", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (svmGetCryptoUnlock()) {
      result->value.val_u = svp_encrypt(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Decrypt file
  //#!    sys.cr.decrypt([str]fname);
  //#!Encrypts file.
  //#!Return: 0 if success, 1 if error
  if (sysFuncMatch(argS->callId, "decrypt", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (svmGetCryptoUnlock()) {
      result->value.val_u = svp_decrypt(s->stringField + argS->arg[1].val_str);
    } else {
      result->value.val_u = 1;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Encrypt string
  //#!    sys.cr.encryptStr([str]source);
  //#!Encrypts given string.
  //#!Return: [str] encryptedString
  if (sysFuncMatch(argS->callId, "encryptStr", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (svmGetCryptoUnlock()) {
      uint8_t *dest;
      uint16_t str_id;

      dest = strNewPLen(sda_strlen(s->stringField + argS->arg[1].val_str)*2 + 1, &str_id, s);
      sda_encrypt_string((uint8_t*) (s->stringField + argS->arg[1].val_str), dest, sda_strlen(s->stringField + argS->arg[1].val_str)*2 + 1);

      result->value.val_str = str_id;
    } else {
      result->value.val_str = strNew((uint8_t*) "", s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Decrypt string
  //#!    sys.cr.decryptStr([str]source);
  //#!Decrypts given string.
  //#!Return: [str] decryptedString
  if (sysFuncMatch(argS->callId, "decryptStr", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (svmGetCryptoUnlock()) {
      uint8_t *dest;
      uint16_t str_id;

      dest = strNewPLen(sda_strlen(s->stringField + argS->arg[1].val_str)*2 + 1, &str_id, s);
      sda_decrypt_string((uint8_t*) (s->stringField + argS->arg[1].val_str), dest, sda_strlen(s->stringField + argS->arg[1].val_str)*2 + 1);

      result->value.val_str = str_id;
    } else {
      result->value.val_str = strNew((uint8_t*) "", s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  return 0;
}