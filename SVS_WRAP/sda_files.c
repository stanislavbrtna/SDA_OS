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

#include "sda_files.h"

//files
svp_file readFil[SDA_FILES_OPEN_MAX];
uint8_t  fr_open[SDA_FILES_OPEN_MAX];

//copy API
svp_file copySource;
svp_file copyDest;
uint16_t copyChunkSize;
uint8_t  copyStatus; // 0 - nothing, 1 - in progress
uint32_t copyProgress;
uint32_t copySize;


void sda_files_copyer() {
  uint16_t x = 0;
  uint8_t buff;
  if (copyStatus == 2) {
    for(x = 0; x < copyChunkSize; x++) {
      buff = svp_fread_u8(&copySource);

      if (svp_feof(&copySource)) {
      #ifndef PC
        // fix for different EOF detection on pc and umc
        svp_fwrite_u8(&copyDest, buff);
      #endif
        copyStatus = 0;
        svp_fclose(&copySource);
        svp_truncate(&copyDest);
        svp_fclose(&copyDest);
        return;
      }

      svp_fwrite_u8(&copyDest, buff);
      copyProgress++;
    }
  }
}


void sda_files_close() {
  for(uint16_t i = 0; i < SDA_FILES_OPEN_MAX; i++) {
    if (fr_open[i]) {
      svp_fclose(&readFil[i]);
      fr_open[i] = 0;
    }
  }

  sda_files_close_conf_csv();
}

extern svsVM svm;
extern sdaSvmMetadata svmMeta;

uint8_t sda_fr_fname_open(uint16_t index, uint8_t * fname) {
  if (index >= SDA_FILES_OPEN_MAX) {
     errSoft((uint8_t *)"error: file index not valid!", &svm);
     return 0;
  }

  sda_strcp(fname, svmMeta.openFileName[index], sizeof(svmMeta.openFileName[index]));
  svmMeta.openFileUsed[index] = svp_fopen_rw(&readFil[index], fname);
  fr_open[index] = svmMeta.openFileUsed[index];
  return fr_open[index];
}


uint8_t sda_fr_fname_reopen(uint16_t index) {
  if (index >= SDA_FILES_OPEN_MAX) {
     errSoft((uint8_t *)"error: file index not valid!", &svm);
     return 0;
  }

  if (!svp_fexists(svmMeta.openFileName[index])) {
    return 0;
  }

  svmMeta.openFileUsed[index] = svp_fopen_rw(&readFil[index], svmMeta.openFileName[index]);
  fr_open[index] = svmMeta.openFileUsed[index];
  return fr_open[index];
}


uint8_t sda_files_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_fs_csv_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_fs_conf_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_fs_bdb_wrapper(varRetVal *result, argStruct *argS, svsVM *s);


void sda_files_wrapper_init() {
  addSysWrapper(sda_files_wrapper, (uint8_t *)"fs");
  addSysWrapper(sda_fs_csv_wrapper, (uint8_t *)"fs.csv");
  addSysWrapper(sda_fs_conf_wrapper, (uint8_t *)"fs.conf");
  addSysWrapper(sda_fs_bdb_wrapper, (uint8_t *)"fs.db");
}

//#!### SDA Files

//#!
//#!Since SDA_OS 1.0.2, more than one general purpose file is supported.
//#!Functions for basic file i/o operations now accept optional index parameter
//#!that specifies what file is used.
//#!Number of files currently openned is defined in SDA_FILES_OPEN_MAX define.
//#!Default value is 10.
//#!

uint8_t sda_files_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {

  uint8_t argType[11];

  //#!##### Open file
  //#!    sys.fs.open([str]fname);
  //#!    sys.fs.open([num]index, [str]fname);
  //#!Opens text file for read or write. If no file index is given, index 0 is used.
  //#!
  //#!Return: 1 on success, 0 on failure
  if (sysFuncMatch(argS->callId, "open", s)) {
    if(argS->usedup == 1) {
      argType[1] = SVS_TYPE_STR;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      result->value.val_u = sda_fr_fname_open(0, s->stringField + argS->arg[1].val_str);
    } else {
      argType[1] = SVS_TYPE_NUM;
      argType[2] = SVS_TYPE_STR;
      if(sysExecTypeCheck(argS, argType, 2, s)) {
        return 0;
      }
      result->value.val_u = sda_fr_fname_open(argS->arg[1].val_s, s->stringField + argS->arg[2].val_str);
    }

    
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Read given number of chars
  //#!    sys.fs.readChars([num] bytes);
  //#!    sys.fs.readChars([num]index, [num] bytes);
  //#!Reads given number of chars from file.
  //#!If no file index is given, index 0 is used.
  //#!
  //#!Return: [str] result
  if (sysFuncMatch(argS->callId, "readChars", s)) {
    uint16_t file_index = 0;
    uint32_t goal_len;

    if(argS->usedup == 1) {
      argType[1] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      goal_len = argS->arg[1].val_s;
    } else {
      argType[1] = SVS_TYPE_NUM;
      argType[2] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 2, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
      goal_len = argS->arg[2].val_s;
    }

    strNewStreamInit(s);
    uint8_t c;
    uint32_t len = 1;

    if (!fr_open[file_index]) {
      result->value.val_u = strNewStreamEnd(s);
      result->type = SVS_TYPE_STR;
      return 1;
    }

    while (!svp_feof(&readFil[file_index])) {
      c = svp_fread_u8(&readFil[file_index]);
      if (strNewStreamPush(c, s) || (goal_len == len)) {
        break;
      }
      len++;
    }

    result->value.val_u = strNewStreamEnd(s);
    result->type = SVS_TYPE_STR;

    return 1;
  }

  //#!##### Writes given string to file
  //#!    sys.fs.writeChars([str]string);
  //#!    sys.fs.writeChars([num]index, [str]string);
  //#!Writes given string to file.
  //#!
  //#!Return: 1 - ok, 0 - fail
  if (sysFuncMatch(argS->callId, "writeChars", s)) {
    uint16_t file_index = 0;
    uint8_t * str;
    uint32_t i = 0;

    if(argS->usedup == 1) {
      argType[1] = SVS_TYPE_STR;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      str = s->stringField + argS->arg[1].val_str;
    } else {
      argType[1] = SVS_TYPE_NUM;
      argType[2] = SVS_TYPE_STR;
      if(sysExecTypeCheck(argS, argType, 2, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
      str = s->stringField + argS->arg[2].val_str;
    }  

    if (!fr_open[file_index]) {
      result->value.val_u = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    }

    while (str[i] != 0) {
      svp_fwrite_u8(&readFil[file_index], str[i]);
      i++;
    }
    result->value.val_u = 1;
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Read byte from file
  //#!    sys.fs.readByte();
  //#!    sys.fs.readByte([num]index);
  //#!Reads byte from file.
  //#!
  //#!Return: [num] result: 0 to 255 - ok, -1 - error, -2 - EOF
  if (sysFuncMatch(argS->callId, "readByte", s)) {
    uint16_t file_index = 0;

    if(argS->usedup == 0) {
      if(sysExecTypeCheck(argS, argType, 0, s)) {
        return 0;
      }
    } else {
      argType[1] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
    }

    result->value.val_s = -1;
    result->type = SVS_TYPE_NUM;

    if (!fr_open[file_index]) {
      return 1;
    }

    if (svp_feof(&readFil[file_index])) {
      result->value.val_s = -2;
      return 1;
    }

    result->value.val_s = (int32_t) svp_fread_u8(&readFil[file_index]);

    return 1;
  }

  //#!##### Write byte to file
  //#!    sys.fs.writeByte([num] byte (0 - 255));
  //#!    sys.fs.writeByte([num]index, [num] byte (0 - 255));
  //#!Writes byte to file.
  //#!
  //#!Return: [num] 0 - fail, 1 - ok
  if (sysFuncMatch(argS->callId, "writeByte", s)) {
    uint16_t file_index = 0;
    uint8_t byte;

    if(argS->usedup == 1) {
      argType[1] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      byte = (uint8_t) argS->arg[1].val_s;
    } else {
      argType[1] = SVS_TYPE_NUM;
      argType[2] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 2, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
      byte = (uint8_t) argS->arg[2].val_s;
    }

    result->value.val_s = 0;
    result->type = SVS_TYPE_NUM;

    if (!fr_open[file_index]) {
      return 1;
    }

    svp_fwrite_u8(&readFil[file_index], byte);
    result->value.val_s = 1;

    return 1;
  }

  //#!##### Seek position in file
  //#!    sys.fs.seek([num] pos_from_start);
  //#!    sys.fs.seek([num]index, [num] pos_from_start);
  //#!Writes byte to file.
  //#!
  //#!Return: [num] 0 - fail, 1 - ok
  if (sysFuncMatch(argS->callId, "seek", s)) {
    uint16_t file_index = 0;
    uint32_t seek_val;

    if(argS->usedup == 1) {
      argType[1] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      seek_val = (uint32_t) argS->arg[1].val_s;
    } else {
      argType[1] = SVS_TYPE_NUM;
      argType[2] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 2, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
      seek_val = (uint32_t) argS->arg[2].val_s;
    }

    if (!fr_open[file_index]) {
      result->value.val_s = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    }

    svp_fseek(&readFil[file_index], seek_val);

    result->value.val_s = 1;
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Truncate file
  //#!    sys.fs.truncate();
  //#!Truncate currently opened file at the position of write pointer.
  //#!
  //#!Return: [num] 0 - fail, 1 - ok
  if (sysFuncMatch(argS->callId, "truncate", s)) {
    uint16_t file_index = 0;

    if(argS->usedup == 0) {
      if(sysExecTypeCheck(argS, argType, 0, s)) {
        return 0;
      }
    } else {
      argType[1] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
    }

    if (!fr_open[file_index]) {
      result->type = SVS_TYPE_NUM;
      result->value.val_s = 0;
      return 1;
    }

    svp_truncate(&readFil[file_index]);

    result->value.val_s = 1;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Tels position in file
  //#!    sys.fs.tell();
  //#!    sys.fs.tell([num]index);
  //#!Returns current write pointer position in the file.
  //#!
  //#!Return: [num] pos
  if (sysFuncMatch(argS->callId, "tell", s)) {
    uint16_t file_index = 0;

    if(argS->usedup == 0) {
      if(sysExecTypeCheck(argS, argType, 0, s)) {
        return 0;
      }
    } else {
      argType[1] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
    }

    result->value.val_s = 0;
    result->type = SVS_TYPE_NUM;

    if (!fr_open[file_index]) {
      return 1;
    }

    result->value.val_s = svp_ftell(&readFil[file_index]);

    return 1;
  }

  //#!##### Get size of file
  //#!    sys.fs.size();
  //#!    sys.fs.size([num] index);
  //#!Returns size of openned file.
  //#!
  //#!Return: [num] size in bytes
  if (sysFuncMatch(argS->callId, "size", s)) {
    uint16_t file_index = 0;

    if(argS->usedup == 0) {
      if(sysExecTypeCheck(argS, argType, 0, s)) {
        return 0;
      }
    } else {
      argType[1] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
    }

    if (fr_open[file_index]) {
      result->value.val_u = svp_get_size(&readFil[file_index]);
    }else{
      result->value.val_u = 0;
    }
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Get last modification time
  //#!    sys.fs.mtime();
  //#!    sys.fs.mtime([num] index);
  //#!Returns last modified time of a openned file.
  //#!In sda timestamp format.
  //#!
  //#!Return: [num] size in bytes
  if (sysFuncMatch(argS->callId, "mtime", s)) {
    uint16_t file_index = 0;

    if(argS->usedup == 0) {
      if(sysExecTypeCheck(argS, argType, 0, s)) {
        return 0;
      }
    } else {
      argType[1] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
    }

    if (fr_open[file_index]) {
      result->value.val_u = svp_get_mtime(&readFil[file_index]);
    }else{
      result->value.val_u = 0;
    }
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Close file
  //#!    sys.fs.close();
  //#!    sys.fs.close([num] index);
  //#!Closes open file.
  //#!
  //#!Return: [num] 1 - ok, 0 - error
  if (sysFuncMatch(argS->callId, "close", s)) {
    uint16_t file_index = 0;

    if(argS->usedup == 0) {
      if(sysExecTypeCheck(argS, argType, 0, s)) {
        return 0;
      }
    } else {
      argType[1] = SVS_TYPE_NUM;
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
      file_index = argS->arg[1].val_s;
    }
    
    result->value.val_u = 0;

    if (fr_open[file_index]) {
      svp_fclose(&readFil[file_index]);
      fr_open[file_index] = 0;
      svmMeta.openFileUsed[file_index] = 0;
      result->value.val_u = 1;
    }

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!
  //#!#### Directory functions
  //#!

  //#!##### Get if path is dir
  //#!    sys.fs.isDir([str] path);
  //#!Gets if path is a directory or not.
  //#!
  //#!Return: [num] 0 - file, 1 - dir
  if (sysFuncMatch(argS->callId, "isDir", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = (int32_t)svp_is_dir(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Create directory
  //#!    sys.fs.mkDir([str] name);
  //#!Creates new directory
  //#!
  //#!Return: [num] 1 - ok, 0 - fail
  if (sysFuncMatch(argS->callId, "mkDir", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = (int32_t)svp_mkdir(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Change working directory
  //#!    sys.fs.chDir([str] pathInData);
  //#!Changes working directory.
  //#!call sys.fs.chDir(0); or sys.fs.chDir(); to get to the DATA context
  //#!call sys.fs.chDir(1); to get to the APPS context
  //#!
  //#!Return: [num] 1 - ok, 0 - fail
  if (sysFuncMatch(argS->callId, "chDir", s)) {

    uint8_t r = 0;

    if (argS->usedup == 1 && argS->argType[1] == SVS_TYPE_STR) {
      r = svp_chdir(s->stringField + argS->arg[1].val_str);
      svmUpdateCurrentWD();
    }

    if (argS->usedup == 1 && argS->argType[1] == SVS_TYPE_NUM) {
      if (argS->arg[1].val_s == 1) {
        svp_switch_main_dir();
        r = svp_chdir((uint8_t *)"APPS");
        svmUpdateCurrentWD();
      }

      if (argS->arg[1].val_s == 0) {
        svp_switch_main_dir();
        r = svp_chdir((uint8_t *)"DATA");
        svmUpdateCurrentWD();
      }
    }

    if (argS->usedup == 0) {
      svp_switch_main_dir();
      r = svp_chdir((uint8_t *)"DATA");
      svmUpdateCurrentWD();
    }

    result->value.val_s = (int32_t) (1 - r);
    result->type = SVS_TYPE_NUM;
    return 1;

    errSoft((uint8_t *)"sysExecTypeCheck: Wrong type or count of arguments for sys function fChDir()!", s);
    return 0;
  }

  //#!##### Get current working directory
  //#!    sys.fs.getCWD();
  //#!Returns current working directory
  //#!
  //#!Return: [str]path
  if (sysFuncMatch(argS->callId, "getCWD", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    uint8_t cwd_buff[256];
    svp_getcwd(cwd_buff, sizeof(cwd_buff));
    
    if (sda_str_find(cwd_buff, "APPS")) {
      result->value.val_str = strNew((uint8_t *)cwd_buff + sda_str_find(cwd_buff, "APPS") + 4, s);
    }

    if (sda_str_find(cwd_buff, "DATA")) {
      result->value.val_str = strNew((uint8_t *)cwd_buff + sda_str_find(cwd_buff, "DATA") + 4, s);
    }

    result->type = SVS_TYPE_STR;

    return 1;
  }

  //#!##### Get current working context
  //#!    sys.fs.getCWC();
  //#!Returns current working context
  //#!
  //#!Return: [num]context 0 - DATA, 1 - APPS
  if (sysFuncMatch(argS->callId, "getCWC", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    uint8_t cwd_buff[256];
    svp_getcwd(cwd_buff, sizeof(cwd_buff));
    
    if (sda_str_find(cwd_buff, "APPS")) {
      result->value.val_u = 1;
    }

    if (sda_str_find(cwd_buff, "DATA")) {
      result->value.val_u = 0;
    }

    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!
  //#!#### File copy
  //#!

  //#!##### File copy select source
  //#!    sys.fs.copySource([str]source);
  //#!Selects source file for copy operation.
  //#!
  //#!Return: [num] 1 - ok, 0 - failed
  if (sysFuncMatch(argS->callId, "copySource", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    uint8_t ok;

    if (copyStatus != 0) {
      result->value.val_s = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    }

    ok = svp_fopen_read(&copySource, s->stringField + argS->arg[1].val_str);

    if (ok == 0) {
      result->value.val_s = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    } else {
      copyStatus = 1;
    }

    result->value.val_s = 1;
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### File copy start
  //#!    sys.fs.copyStart([str]dest, [num]ChunkSize);
  //#!Starts copy operation, chunksize of bytes will be copyed each cycle.
  //#!
  //#!Return: [num] 1 - ok, 0 - failed
  if (sysFuncMatch(argS->callId, "copyStart", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    uint8_t ok;

    if (copyStatus != 1) {
      result->value.val_s = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    }

    ok = svp_fopen_rw(&copyDest, s->stringField + argS->arg[1].val_str);

    if (ok == 0) {
      result->value.val_s = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    } else {
      copyStatus = 2;
      copySize = svp_get_size(&copySource);
      copyProgress = 0;
      copyChunkSize = (uint16_t) argS->arg[2].val_s;
    }

    result->value.val_s = 1;
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### File copy status
  //#!    sys.fs.copyStat([num]opt);
  //#! opt: 0 - status ret: [num]0 - nothing, 1 - source selected, 2 - copy in progress
  //#!
  //#! opt: 1 - size of source [num]bytes
  //#!
  //#! opt: 2 - remaining bytes [num]bytes
  if (sysFuncMatch(argS->callId, "copyStat", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (argS->arg[1].val_s == 0) {
      result->value.val_s = copyStatus;
      result->type = SVS_TYPE_NUM;
    }

    if (argS->arg[1].val_s == 1) {
      result->value.val_s = (int32_t) copySize;
      result->type = SVS_TYPE_NUM;
    }

    if (argS->arg[1].val_s == 2) {
      result->value.val_s = (int32_t) copyProgress;
      result->type = SVS_TYPE_NUM;
    }

    return 1;
  }

  //#!##### Stop current copy operation
  //#!    sys.fs.copyStop();
  //#!Stops current copy operation.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "copyStop", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (copyStatus != 0) {
      svp_fclose(&copySource);
      if (copyStatus == 2) {
        svp_fclose(&copyDest);
      }
    }

    copyStatus = 0;

    return 1;
  }

  //#!
  //#!#### Check, remove, rename
  //#!
  
  //#!##### Check if file exist
  //#!    sys.fs.exists([str]fname);
  //#!Checks if file exists.
  //#!
  //#!Return: 1 if file exists, otherwise 0
  if (sysFuncMatch(argS->callId, "exists", s)) {
    argType[1] = 1;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_u =svp_fexists(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Remove file
  //#!    sys.fs.delete([str]fname);
  //#!Deletes file with fiven fname. Can also delete empty directories.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "delete", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (svp_is_dir(s->stringField + argS->arg[1].val_str)) {
      svp_rmdir(s->stringField + argS->arg[1].val_str);
    } else {
      svp_unlink(s->stringField + argS->arg[1].val_str);
    }
    return 1;
  }

  //#!##### Move/rename file
  //#!    sys.fs.rename([str]oldPath, [str]newPath);
  //#!Moves/renames given file.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "rename", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    svp_rename(s->stringField + argS->arg[1].val_str, s->stringField + argS->arg[2].val_str);
    return 1;
  }

  //#!
  //#!#### Find files
  //#!

  //#!##### Find begin
  //#!    sys.fs.find([str]extension, [str]directory);
  //#!Inits file find operation, returns first result.
  //#!
  //#!Return: [str]filename or "" if none
  if (sysFuncMatch(argS->callId, "find", s)) {
    uint8_t buff[129];
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    if(svp_extFind(buff, 128, s->stringField + argS->arg[1].val_str, s->stringField + argS->arg[2].val_str)) {
      result->value.val_str = strNew(buff, s);
      result->type = SVS_TYPE_STR;
    }else{
      result->value.val_str = strNew((uint8_t *)"", s);
      result->type = SVS_TYPE_STR;
    }
    return 1;
  }

  //#!##### Find next
  //#!    sys.fs.findNext();
  //#!Next iteration of file find operation.
  //#!
  //#!Return: [str]filename or "" if none
  if (sysFuncMatch(argS->callId, "findNext", s)) {
    uint8_t buff[129];
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if(svp_extFindNext(buff, 128)) {
      result->value.val_str = strNew(buff, s);
      result->type = SVS_TYPE_STR;
    }else{
      result->value.val_str = strNew((uint8_t *)"", s);
      result->type = SVS_TYPE_STR;
    }
    return 1;
  }

  //#!##### Example
  //#!    
  //#!    for(findfil = sys.fs.find("txt", "."); findfil != ""; findfil = sys.fs.findNext();) {
  //#!      print("found: " + findfil);
  //#!    }
  //#!    

  //#!
  //#!Find is not stateless, sys.fs.find must be re-inicialized after recursive call.
  //#!Example of recursive function:
  //#!    
  //#!    function ls {
  //#!      local findfil;
  //#!      local n = 0;
  //#!      print(arg0);
  //#!      for(findfil = sys.fs.find("", arg0); findfil != ""; findfil = sys.fs.findNext();) {
  //#!        if (sys.fs.isDir(arg0 + "/" + findfil)) {
  //#!          ls(arg0 + "/" + findfil);
  //#!          findfil = sys.fs.find("", arg0);
  //#!          local x;
  //#!          for (x = 0; x<n; x++;) {
  //#!            findfil = sys.fs.findNext();
  //#!          }
  //#!        } else {
  //#!          print(arg0 + "/" + findfil);
  //#!        }
  //#!        n++;
  //#!      }  
  //#!    }
  //#!    


  //#!
  //#!#### Files as strings
  //#!

  //#!##### Reads file as string
  //#!    sys.fs.readStr([str]fname);
  //#!Reads text file to svs string buffer.
  //#!
  //#!Return: [str]FileContents
  if (sysFuncMatch(argS->callId, "readStr", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_u = svp_load_file_to_svs(s->stringField + argS->arg[1].val_str, s);
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Write string as file
  //#!    sys.fs.writeStr([str]str, [str]fname);
  //#!Writes svs string to file.
  //#!
  //#!Return: None
  if (sysFuncMatch(argS->callId, "writeStr", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    svp_store_svs_to_file(s->stringField + argS->arg[2].val_str, s->stringField + argS->arg[1].val_str, s);
    return 1;
  }

  return 0;
}
