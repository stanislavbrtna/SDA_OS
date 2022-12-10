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

//file
svp_file readFil;
uint8_t fr_filename[64];
uint8_t fr_open;

//Conf file
sda_conf conFile;
uint8_t conf_filename[64];
uint8_t conf_open;

//Csv file
svp_csvf csvFile;
uint8_t csv_filename[64];
uint8_t csv_open;

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
    for(x = 0; x < copyChunkSize; x++){
      buff = svp_fread_u8(&copySource);

      if (svp_feof(&copySource)) {
      #ifndef PC
        // fix for different EOF detection on pc and umc
        svp_fwrite_u8(&copyDest, buff);

      #endif

        copyStatus = 0;
        svp_fclose(&copySource);
        svp_fclose(&copyDest);
        return;
      }

      svp_fwrite_u8(&copyDest, buff);
      copyProgress++;
    }
  }
}


void sda_files_close() {
  if (fr_open) {
    svp_fclose(&readFil);
    fr_open = 0;
  }

  if (conf_open) {
    conf_open = 0;
    sda_conf_close(&conFile);
  }

  if (csv_open) {
    csv_open = 0;
    svp_csv_close(&csvFile);
  }
}


uint8_t * sda_get_fr_fname() {
  if (fr_open) {
    return fr_filename;
  } else {
    return 0;
  }
}


uint8_t * sda_get_conf_fname() {
  if (conf_open) {
    return conf_filename;
  } else {
    return 0;
  }
}


uint8_t * sda_get_csv_fname() {
  if (csv_open) {
    return csv_filename;
  } else {
    return 0;
  }
}


uint8_t sda_fr_fname_open(uint8_t * fname) {
  sda_strcp(fname, fr_filename, sizeof(fr_filename));
  fr_open = svp_fopen_rw(&readFil, fname);
  return fr_open;
}


uint8_t sda_files_conf_open(uint8_t * fname) {
  sda_strcp(fname, conf_filename, sizeof(conf_filename));
  conf_open = sda_conf_open(&conFile, fname);
  return conf_open;
}


uint8_t sda_files_csv_open(uint8_t * fname) {
  sda_strcp(fname, csv_filename, sizeof(csv_filename));
  csv_open = svp_csv_open(&csvFile, fname);
  return csv_open;
}


uint8_t sda_files_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_fs_csv_wrapper(varRetVal *result, argStruct *argS, svsVM *s);
uint8_t sda_fs_conf_wrapper(varRetVal *result, argStruct *argS, svsVM *s);


void sda_files_wrapper_init() {
  addSysWrapper(sda_files_wrapper, (uint8_t *)"fs");
  addSysWrapper(sda_fs_csv_wrapper, (uint8_t *)"fs.csv");
  addSysWrapper(sda_fs_conf_wrapper, (uint8_t *)"fs.conf");
}

//#!### SDA Files

uint8_t sda_files_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {

  uint8_t argType[11];

  static uint8_t readBuff[2048];

  //#!##### Open file
  //#!    sys.fs.open([str]fname);
  //#!Opens text file for read or write.
  //#!Return: 1 on success, 0 on failure
  if (sysFuncMatch(argS->callId, "open", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_u = sda_fr_fname_open(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Read given number of chars
  //#!    sys.fs.readChars([num] bytes);
  //#!Reads given number of chars from file.
  //#!Return: [str] result
  if (sysFuncMatch(argS->callId, "readChars", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    strNewStreamInit(s);
    uint8_t c;
    uint16_t len = 1;

    if (!fr_open) {
      result->value.val_u = strNewStreamEnd(s);
      result->type = SVS_TYPE_STR;
      return 1;
    }

    while (!svp_feof(&readFil)) {
      c = svp_fread_u8(&readFil);
      if (strNewStreamPush(c, s) || (argS->arg[1].val_s == len)) {
        break;
      }
      len++;
    }

    result->value.val_u = strNewStreamEnd(s);
    result->type = SVS_TYPE_STR;

    return 1;
  }

  //#!##### Writes given string to file
  //#!    sys.fs.writeChars([str] string);
  //#!Writes given string to file.
  //#!Return: 1 - ok, 0 - fail
  if (sysFuncMatch(argS->callId, "writeChars", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    uint8_t * str;
    uint32_t i = 0;

    str = s->stringField + argS->arg[1].val_str;

    if (!fr_open) {
      result->value.val_u = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    }

    while (str[i] != 0) {
      svp_fwrite_u8(&readFil, str[i]);
      i++;
    }
    result->value.val_u = 1;
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Read byte from file
  //#!    sys.fs.readByte();
  //#!Reads byte from file.
  //#!Return: [num] result: 0 to 255 - ok, -1 - error, -2 - EOF
  if (sysFuncMatch(argS->callId, "readByte", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = -1;
    result->type = SVS_TYPE_NUM;

    if (!fr_open) {
      return 1;
    }

    if (svp_feof(&readFil)) {
      result->value.val_s = -2;
      return 1;
    }

    result->value.val_s = (int32_t) svp_fread_u8(&readFil);

    return 1;
  }

  //#!##### Write byte to file
  //#!    sys.fs.writeByte([num] byte (0 - 255));
  //#!Writes byte to file.
  //#!Return: [num] 0 - fail, 1 - ok
  if (sysFuncMatch(argS->callId, "writeByte", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = 0;
    result->type = SVS_TYPE_NUM;

    if (!fr_open) {
      return 1;
    }

    svp_fwrite_u8(&readFil, (uint8_t) argS->arg[1].val_s);
    result->value.val_s = 1;

    return 1;
  }

  //#!##### Seek position in file
  //#!    sys.fs.seek([num] pos_from_start);
  //#!Writes byte to file.
  //#!Return: [num] 0 - fail, 1 - ok
  if (sysFuncMatch(argS->callId, "seek", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (!fr_open) {
      result->value.val_s = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    }

    svp_fseek(&readFil, (uint32_t) argS->arg[1].val_s);

    result->value.val_s = 1;
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Truncate file
  //#!    sys.fs.truncate();
  //#!Truncate currently opened file at the position of write pointer.
  //#!Return: [num] 0 - fail, 1 - ok
  if (sysFuncMatch(argS->callId, "truncate", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (!fr_open) {
      result->type = SVS_TYPE_NUM;
      result->value.val_s = 0;
      return 1;
    }

    svp_truncate(&readFil);

    result->value.val_s = 1;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get if path is dir
  //#!    sys.fs.isDir([str] path);
  //#!Gets if path is a directory or not.
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
  //#!call sys.fs.chDir(); to get to the DATA directory
  //#!call sys.fs.chDir(1); to get to the APPS directory
  //#!Return: None
  if (sysFuncMatch(argS->callId, "chDir", s)) {

    if (argS->usedup == 1 && argS->argType[1] == SVS_TYPE_STR) {
      result->value.val_s = (int32_t)svp_chdir(s->stringField + argS->arg[1].val_str);
      result->type = SVS_TYPE_NUM;
      sdaUpdateCurrentWD();
      return 1;
    }

    if (argS->usedup == 1 && argS->argType[1] == SVS_TYPE_NUM) {
      if (argS->arg[1].val_s == 1) {
        svp_switch_main_dir();
        result->value.val_s = (int32_t)svp_chdir((uint8_t *)"APPS");
        result->type = SVS_TYPE_NUM;
        sdaUpdateCurrentWD();
      }
      return 1;
    }

    if (argS->usedup == 0) {
      svp_switch_main_dir();
      result->value.val_s = (int32_t)svp_chdir((uint8_t *)"DATA");
      result->type = SVS_TYPE_NUM;
      sdaUpdateCurrentWD();
      return 1;
    }

    errSoft((uint8_t *)"sysExecTypeCheck: Wrong type or count of arguments for sys function fChDir()!", s);
    return 0;
  }

  //#!##### File copy select source
  //#!    sys.fs.copySource([str]source);
  //#!Selects source file for copy operation.
  //#!Return: [num] 1 - ok, 0 - failed
  if (sysFuncMatch(argS->callId, "copySource", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    uint8_t ok = 1;

    if (copyStatus != 0) {
      result->value.val_s = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    }

    ok *= svp_fopen_read(&copySource, s->stringField + argS->arg[1].val_str);

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
  //#!Return: [num] 1 - ok, 0 - failed
  if (sysFuncMatch(argS->callId, "copyStart", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    uint8_t ok = 1;

    if (copyStatus != 1) {
      result->value.val_s = 0;
      result->type = SVS_TYPE_NUM;
      return 1;
    }

    ok *= svp_fopen_rw(&copyDest, s->stringField + argS->arg[1].val_str);

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
  //#! opt: 1 - size of source [num]bytes
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

  //#!##### Tels position in file
  //#!    sys.fs.tell();
  //#!Returns current write pointer position in the file.
  //#!Return: [num] pos
  if (sysFuncMatch(argS->callId, "tell", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = 0;
    result->type = SVS_TYPE_NUM;

    if (!fr_open) {
      return 1;
    }

    result->value.val_s = svp_ftell(&readFil);

    return 1;
  }

  //#!##### Get size of file
  //#!    sys.fs.size();
  //#!Returns size of openned file.
  //#!Return: Size of openned file
  if (sysFuncMatch(argS->callId, "size", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (fr_open) {
      result->value.val_u = svp_get_size(&readFil);
    }else{
      result->value.val_u = 0;
    }
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Check if file exist
  //#!    sys.fs.exists([str]fname);
  //#!Checks if file exists.
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

  //#!##### Seek in file and fill text field
  //#!    sys.fs.seekFill([num]text_id, [num]filePos);
  //#!Fills pscg element with text from filePos position.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "seekFill", s)) {
    uint8_t c;
    uint32_t i = 0;
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    svp_fseek(&readFil, 0);

    if (svp_get_size(&readFil) > (uint32_t) argS->arg[2].val_s) {
      svp_fseek(&readFil, argS->arg[2].val_s);
    } else {
      svp_fseek(&readFil, svp_get_size(&readFil) - 80);
    }

    //zarážení o enter nefunguje dobře, zarážíme se o mezeru
    if (argS->arg[2].val_s != 0) {
      while (!svp_feof(&readFil)) {
        c = svp_fread_u8(&readFil);
        if (c == ' ') {
          break;
        }
      }
    }

    readBuff[0] = 0;

    for(i = 0; (i < 2001) && !svp_feof(&readFil); i++) {
      readBuff[i] = svp_fread_u8(&readFil);
    }
    readBuff[i] = 0;

    gr2_set_str(argS->arg[1].val_s,readBuff, &sda_app_con);
    gr2_set_modified(argS->arg[1].val_s, &sda_app_con);

    return 1;
  }

  //#!##### Close file
  //#!    sys.fs.close();
  //#!Closes open file.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "close", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (fr_open) {
      svp_fclose(&readFil);
      fr_open = 0;
    }
    return 1;
  }

  //#!##### Remove file
  //#!    sys.fs.delete([str]fname);
  //#!Deletes file with fiven fname.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "delete", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_unlink(s->stringField + argS->arg[1].val_str);
    return 1;
  }

  //#!##### Move/rename file
  //#!    sys.fs.rename([str]oldPath, [str]newPath);
  //#!Moves/renames given file.
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
  //#!    for(findfil = sys.fs.find("txt", "."); findfil != ""; findfil = sys.fs.findNext();) {
  //#!      print("found: " + findfil);
  //#!    }


  //#!
  //#!#### Files as strings
  //#!

  //#!##### Reads file as string
  //#!    sys.fs.readStr([str]fname);
  //#!Reads text file to svs string buffer.
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


uint8_t sda_fs_csv_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {

  uint8_t argType[11];

  //#!#### SDA CSV files API

  //#!##### Open csv file
  //#!    sys.fs.csv.open([str]fname);
  //#!Opens csv file.
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "open", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    csv_open = 1;
    result->value.val_s = sda_files_csv_open(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Close csv file
  //#!    sys.fs.csv.close();
  //#!Closes csv file.
  //#!Return: None.
  if (sysFuncMatch(argS->callId, "close", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    csv_open = 0;
    result->value.val_s = svp_csv_close(&csvFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### New csv line
  //#!    sys.fs.csv.newLine([num]numberOfCells);
  //#!Adds new line to csv with given number of cells.
  //#!Return: None.
  if (sysFuncMatch(argS->callId, "newLine", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_csv_new(&csvFile, argS->arg[1].val_s);
    return 1;
  }

  //#!##### Get csv cell
  //#!    sys.fs.csv.getCell([num]cellNumber, [str]default);
  //#!Gets data from specified cell on current line.
  //#!Return: [str]cellContents
  if (sysFuncMatch(argS->callId, "getCell", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    uint8_t buff[512];

    svp_csv_get_cell(&csvFile, argS->arg[1].val_s, s->stringField + argS->arg[2].val_str, buff, sizeof(buff));
    result->value.val_u = strNew(buff, s);
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Set csv cell
  //#!    sys.fs.csv.setCell([num]cellNumber, [str]value);
  //#!Sets data of specified cell on current line.
  //#!Return: [str]cellContents
  if (sysFuncMatch(argS->callId, "setCell", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    svp_csv_set_cell(&csvFile, argS->arg[1].val_s, s->stringField + argS->arg[2].val_str);
    return 1;
  }

  //#!##### Feed line
  //#!    sys.fs.csv.lineFeed();
  //#!Moves to the next lone of csv file
  //#!Return: [num] 1 - ok, 0 - end of file
  if (sysFuncMatch(argS->callId, "lineFeed", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = svp_csv_next_line(&csvFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Remove line
  //#!    sys.fs.csv.removeLine();
  //#!Removes current line from csv
  //#!Return: None
  if (sysFuncMatch(argS->callId, "removeLine", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_csv_remove_line(&csvFile);
    return 1;
  }

  //#!##### Rewind file
  //#!    sys.fs.csv.rewind();
  //#!Rewinds file back on the start.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "rewind", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_csv_rewind(&csvFile);
    return 1;
  }

  return 0;
}


uint8_t sda_fs_conf_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {

  uint8_t argType[11];

  //#!#### Config files API

  //#!##### Open config file
  //#!    sys.fs.conf.open([str]fname);
  //#!Opens config file.
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "open", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    conf_open = 1;
    sda_strcp(s->stringField + argS->arg[1].val_str, conf_filename, sizeof(conf_filename));
    if (sda_conf_open(&conFile, s->stringField + argS->arg[1].val_str)) {
      result->value.val_s = 1;
    } else {
      result->value.val_s = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Close config file
  //#!    sys.fs.conf.close();
  //#!Close conf file.
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "close", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (conf_open == 1 && (sda_conf_close(&conFile) == 0)) {
      conf_open = 0;
      result->value.val_s = 1;
    } else {
      result->value.val_s = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Check if key exists
  //#!    sys.fs.conf.exists([str]key);
  //#!Checks if key exists in conf file
  //#!Return: [num] 1 if key exists.
  if (sysFuncMatch(argS->callId, "exists", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = sda_conf_key_exists(&conFile, s->stringField+argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Read key
  //#!    sys.fs.conf.read([str]key);
  //#!Reads key from config file as a string, 128 chars max.
  //#!Return: [str]Value
  if (sysFuncMatch(argS->callId, "read", s)) {
    uint8_t buff[512];
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }
    // TODO: Make loading in stream mode, with checking free string mem
    sda_conf_key_read(&conFile, s->stringField+argS->arg[1].val_str, buff, 512);
    buff[511] = 0;
    result->value.val_u = strNew(buff, s);
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Read Key as int
  //#!    sys.fs.conf.readInt([str]key, [num]default);
  //#!Reads key from config file as num (integrer). To be removed.
  //#!Return: [num]Value
  if (sysFuncMatch(argS->callId, "readInt", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    result->value.val_s = sda_conf_key_read_i32(&conFile, s->stringField + argS->arg[1].val_str,argS->arg[2].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Write key
  //#!    sys.fs.conf.write([str]key, [str]val);
  //#!Writes value in specified key.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "write", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    sda_conf_key_write(&conFile, s->stringField + argS->arg[1].val_str, s->stringField + argS->arg[2].val_str);
    return 1;
  }

  //#!##### Remove key
  //#!    sys.fs.conf.remove([str]key);
  //#!Removes given key.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "remove", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    sda_conf_key_remove(&conFile, s->stringField + argS->arg[1].val_str);
    return 1;
  }

  return 0;
}
