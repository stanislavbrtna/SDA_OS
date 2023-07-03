/*
Copyright (c) 2023 Stanislav Brtna

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

//Conf file
sda_conf conFile;
uint8_t conf_filename[64];
uint8_t conf_open;

//Csv file
svp_csvf csvFile;
uint8_t csv_filename[64];
uint8_t csv_open;


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

void sda_files_close_conf_csv() {
  if (conf_open) {
    conf_open = 0;
    sda_conf_close(&conFile);
  }

  if (csv_open) {
    csv_open = 0;
    svp_csv_close(&csvFile);
  }
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
    if (!csv_open) {
      errSoft((uint8_t *)"No CSV file openned!", s);
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
    if (!csv_open) {
      errSoft((uint8_t *)"No CSV file openned!", s);
      return 0;
    }
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
    if (!csv_open) {
      errSoft((uint8_t *)"No CSV file openned!", s);
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
    if (!csv_open) {
      errSoft((uint8_t *)"No CSV file openned!", s);
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
    if (!csv_open) {
      errSoft((uint8_t *)"No CSV file openned!", s);
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
    if (!csv_open) {
      errSoft((uint8_t *)"No CSV file openned!", s);
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

    if (!conf_open) {
      errSoft((uint8_t *)"Conf file not openned!", s);
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

    if (!conf_open) {
      errSoft((uint8_t *)"Conf file not openned!", s);
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

    if (!conf_open) {
      errSoft((uint8_t *)"Conf file not openned!", s);
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

    if (!conf_open) {
      errSoft((uint8_t *)"Conf file not openned!", s);
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
    
    if (!conf_open) {
      errSoft((uint8_t *)"Conf file not openned!", s);
      return 0;
    }

    sda_conf_key_remove(&conFile, s->stringField + argS->arg[1].val_str);
    return 1;
  }

  return 0;
}
