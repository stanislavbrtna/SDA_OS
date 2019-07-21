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

#include "sda_wrapper.h"

//file
svp_file readFil;
uint8_t fr_filename[64];
uint8_t fr_open;

//Conf file
svp_conf conFile;
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
    svp_conf_close(&conFile);
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
  conf_open = svp_conf_open(&conFile, fname);
  return conf_open;
}

uint8_t sda_files_csv_open(uint8_t * fname) {
  sda_strcp(fname, csv_filename, sizeof(csv_filename));
  csv_open = svp_csv_open(&csvFile, fname);
  return csv_open;
}

//#!### SDA Files

uint8_t sda_files_sub_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {

  uint8_t argType[11];

  static uint8_t readBuff[2048];

  //#!##### Open file
  //#!    sys.pFrOpen([str]fname);
  //#!    sys.fOpen([str]fname);
  //#!Opens text file for read or write.
  //#!Return: 1 on success, 0 on failure
  if (sysFuncMatch(argS->callId, "pFrOpen", s) ||
      sysFuncMatch(argS->callId, "fOpen", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_u = sda_fr_fname_open(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Read given number of chars
  //#!    sys.fReadChars([num] bytes);
  //#!Reads given number of chars from file.
  //#!Return: [str] result
  if (sysFuncMatch(argS->callId, "fReadChars", s)) {
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
  //#!    sys.fWriteChars([str] string);
  //#!Writes given string to file.
  //#!Return: 1 - ok, 0 - fail
  if (sysFuncMatch(argS->callId, "fWriteChars", s)) {
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
  //#!    sys.fReadByte();
  //#!Reads byte from file.
  //#!Return: [num] result: 0 to 255 - ok, -1 - error
  if (sysFuncMatch(argS->callId, "fReadByte", s)) {
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
  //#!    sys.fWriteByte([num] byte (0 - 255));
  //#!Writes byte to file.
  //#!Return: [num] 0 - fail, 1 - ok
  if (sysFuncMatch(argS->callId, "fWriteByte", s)) {
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
  //#!    sys.fSeek([num] pos_from_start);
  //#!Writes byte to file.
  //#!Return: [num] 0 - fail, 1 - ok
  if (sysFuncMatch(argS->callId, "fSeek", s)) {
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
  //#!    sys.fTruncate();
  //#!Truncate currently opened file at the position of write pointer.
  //#!Return: [num] 0 - fail, 1 - ok
  if (sysFuncMatch(argS->callId, "fTruncate", s)) {
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
  //#!    sys.fIsDir([str] path);
  //#!Gets if path is a directory or not.
  //#!Return: [num] 0 - file, 1 - dir
  if (sysFuncMatch(argS->callId, "fIsDir", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = (int32_t)svp_is_dir(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Create directory
  //#!    sys.fMkDir([str] name);
  //#!Creates new directory
  //#!Return: [num] 1 - ok, 0 - fail
  if (sysFuncMatch(argS->callId, "fMkDir", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = (int32_t)svp_mkdir(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Change working directory
  //#!    sys.fChDir([str] pathInData);
  //#!Changes working directory.
  //#!call sys.fChDir(); to get to the DATA directory
  //#!call sys.fChDir(1); to get to the APPS directory
  //#!Return: None
  if (sysFuncMatch(argS->callId, "fChDir", s)) {

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
  //#!    sys.fCopySource([str]source);
  //#!Selects source file for copy operation.
  //#!Return: [num] 1 - ok, 0 - failed
  if (sysFuncMatch(argS->callId, "fCopySource", s)) {
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
  //#!    sys.fCopyStart([str]dest, [num]ChunkSize);
  //#!Starts copy operation, chunksize of bytes will be copyed each cycle.
  //#!Return: [num] 1 - ok, 0 - failed
  if (sysFuncMatch(argS->callId, "fCopyStart", s)) {
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
  //#!    sys.fCopyStat([num]opt);
  //#! opt: 0 - status ret: [num]0 - nothing, 1 - source selected, 2 - copy in progress
  //#! opt: 1 - size of source [num]bytes
  //#! opt: 2 - remaining bytes [num]bytes
  if (sysFuncMatch(argS->callId, "fCopyStat", s)) {
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
  //#!    sys.fCopyStop();
  //#!Stops current copy operation.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "fCopyStop", s)) {
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
  //#!    sys.fTell();
  //#!Returns current write pointer position in the file.
  //#!Return: [num] pos
  if (sysFuncMatch(argS->callId, "fTell", s)) {

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
  //#!    sys.pFrSize();
  //#!Returns size of openned file.
  //#!Return: Size of openned file
  if (sysFuncMatch(argS->callId, "pFrSize", s)) {
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
  //#!    sys.pFrExists([str]fname);
  //#!Checks if file exists.
  //#!Return: 1 if file exists, otherwise 0
  if (sysFuncMatch(argS->callId, "pFrExists", s)) {
    argType[1] = 1;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_u =svp_fexists(s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;

    return 1;
  }

  //#!##### Seek in file and fill text field
  //#!    sys.pFrSeek([num]text_id, [num]filePos);
  //#!Fills pscg element with text from filePos position.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "pFrSeek", s)) {
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

    pscg_set_str(argS->arg[1].val_s,readBuff, &sda_app_con);
    pscg_set_modified(argS->arg[1].val_s, &sda_app_con);

    return 1;
  }

  //#!##### Close file
  //#!    sys.pFrClose();
  //#!    sys.fClose();
  //#!Closes open file.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "pFrClose", s)
      || sysFuncMatch(argS->callId, "fClose", s)) {
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
  //#!    sys.fDelete([str]fname);
  //#!Deletes file with fiven fname.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "fDelete", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_unlink(s->stringField + argS->arg[1].val_str);
    return 1;
  }

  //#!##### Move/rename file
  //#!    sys.fRename([str]oldPath, [str]newPath);
  //#!Moves/renames given file.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "fRename", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    svp_rename(s->stringField + argS->arg[1].val_str, s->stringField + argS->arg[2].val_str);
    return 1;
  }

  //#!#### SDA CSV files API

  //#!##### Open csv file
  //#!    sys.csvOpen([str]fname);
  //#!Opens csv file.
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "csvOpen", s)) {
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
  //#!    sys.csvClose();
  //#!Closes csv file.
  //#!Return: None.
  if (sysFuncMatch(argS->callId, "csvClose", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    csv_open = 0;
    result->value.val_s = svp_csv_close(&csvFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### New csv line
  //#!    sys.csvNewLine([num]numberOfCells);
  //#!Adds new line to csv with given number of cells.
  //#!Return: None.
  if (sysFuncMatch(argS->callId, "csvNewLine", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_csv_new(&csvFile, argS->arg[1].val_s);
    return 1;
  }

  //#!##### Get csv cell
  //#!    sys.csvGetCell([num]cellNumber, [str]default);
  //#!Gets data from specified cell on current line.
  //#!Return: [str]cellContents
  if (sysFuncMatch(argS->callId, "csvGetCell", s)) {
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
  //#!    sys.csvSetCell([num]cellNumber, [str]value);
  //#!Sets data of specified cell on current line.
  //#!Return: [str]cellContents
  if (sysFuncMatch(argS->callId, "csvSetCell", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    svp_csv_set_cell(&csvFile, argS->arg[1].val_s, s->stringField + argS->arg[2].val_str);
    return 1;
  }

  //#!##### Feed line
  //#!    sys.csvLineFeed();
  //#!Moves to the next lone of csv file
  //#!Return: [num] 1 - ok, 0 - end of file
  if (sysFuncMatch(argS->callId, "csvLineFeed", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->value.val_s = svp_csv_next_line(&csvFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Remove line
  //#!    sys.csvRemoveLine();
  //#!Removes current line from csv
  //#!Return: None
  if (sysFuncMatch(argS->callId, "csvRemoveLine", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_csv_remove_line(&csvFile);
    return 1;
  }

  //#!##### Rewind file
  //#!    sys.csvRewind();
  //#!Rewinds file back on the start.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "csvRewind", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    svp_csv_rewind(&csvFile);
    return 1;
  }

  //#!#### Config files API

  //#!##### Open config file
  //#!    sys.cOpen([str]fname);
  //#!Opens config file.
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "cOpen", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    conf_open = 1;
    sda_strcp(s->stringField + argS->arg[1].val_str, conf_filename, sizeof(conf_filename));
    result->value.val_s = svp_conf_open(&conFile, s->stringField + argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Close config file
  //#!    sys.cClose();
  //#!Close conf file.
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "cClose", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    conf_open = 0;
    result->value.val_s = svp_conf_close(&conFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Check if key exists
  //#!    sys.cKeyExists([str]key);
  //#!Checks if key exists in conf file
  //#!Return: [num] 1 if key exists.
  if (sysFuncMatch(argS->callId, "cKeyExists", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    result->value.val_s = svp_conf_key_exists(&conFile, s->stringField+argS->arg[1].val_str);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Read key
  //#!    sys.cKeyRead([str]key);
  //#!Reads key from config file as a string, 128 chars max.
  //#!Return: [str]Value
  if (sysFuncMatch(argS->callId, "cKeyRead", s)) {
    uint8_t buff[512];
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)){
      return 0;
    }
    // TODO: Make loading in stream mode, with checking free string mem
    svp_conf_key_read(&conFile, s->stringField+argS->arg[1].val_str, buff, 512);
    buff[511] = 0;
    result->value.val_u = strNew(buff, s);
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Read Key as int
  //#!    sys.cKeyReadInt([str]key, [num]default);
  //#!Reads key from config file as num (integrer). To be removed.
  //#!Return: [num]Value
  if (sysFuncMatch(argS->callId, "cKeyReadInt", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    result->value.val_s = svp_conf_key_read_i32(&conFile, s->stringField + argS->arg[1].val_str,argS->arg[2].val_s);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Write key
  //#!    sys.cKeyWrite([str]key, [str]val);
  //#!Writes value in specified key.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "cKeyWrite", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }
    svp_conf_key_write(&conFile, s->stringField + argS->arg[1].val_str, s->stringField + argS->arg[2].val_str);
    return 1;
  }

  //#!##### Remove key
  //#!    sys.cKeyRemove([str]key);
  //#!Removes given key.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "cKeyRemove", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    svp_conf_key_remove(&conFile, s->stringField + argS->arg[1].val_str);
    return 1;
  }

  //#!
  //#!#### Find files
  //#!

  //#!##### Find begin
  //#!    sys.fFind([str]extension, [str]directory);
  //#!Inits file find operation, returns first result.
  //#!Return: [str]filename or "" if none
  if (sysFuncMatch(argS->callId, "fFind", s)) {
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
  //#!    sys.fFindNext();
  //#!Next iteration of file find operation.
  //#!Return: [str]filename or "" if none
  if (sysFuncMatch(argS->callId, "fFindNext", s)) {
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
  //#!    for(findfil = sys.fFind("txt", "."); findfil != ""; findfil = sys.fFindNext();) {
  //#!      print("found: " + findfil);
  //#!    }


  //#!
  //#!#### Files as strings
  //#!

  //#!##### Reads file as string
  //#!    sys.fReadStr([str]fname);
  //#!Reads text file to svs string buffer.
  //#!Return: [str]FileContents
  if (sysFuncMatch(argS->callId, "fReadStr", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    result->value.val_u = svp_load_file_to_svs(s->stringField + argS->arg[1].val_str, s);
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Write string as file
  //#!    sys.fWriteStr([str]str, [str]fname);
  //#!Writes svs string to file.
  //#!Return: None
  if (sysFuncMatch(argS->callId, "fWriteStr", s)) {
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
