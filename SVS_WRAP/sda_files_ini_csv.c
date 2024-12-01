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
uint8_t  conf_filename[64];
uint8_t  conf_open;

//Csv file
svp_csvf csvFile;
uint8_t  csv_filename[64];
uint8_t  csv_open;

//db
sda_bdb dbFile;
uint8_t db_filename[64];
uint8_t db_open;

// TODO: load/store db on app suspend


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

  // Default return
  result->value.val_s = 0;
  result->type = SVS_TYPE_NUM;

  //#!#### SDA CSV files API
  //#!
  //#!SDA_OS implements basic csv-like file api.

  //#!##### Open csv file
  //#!    sys.fs.csv.open([str]fname);
  //#!Opens csv file.
  //#!
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

  //#!##### Set separator
  //#!    sys.fs.csv.setSeparator([str]separator);
  //#!Sets the csv separator default is "|".
  //#!
  //#!Return: none.
  if (sysFuncMatch(argS->callId, "setSeparator", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    if (!csv_open) {
      errSoft((uint8_t *)"No CSV file openned!", s);
      return 0;
    }

    svp_csv_set_separator(&csvFile, s->stringField[argS->arg[1].val_str]);

    return 1;
  }

  //#!##### Close csv file
  //#!    sys.fs.csv.close();
  //#!Closes csv file.
  //#!
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "close", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    result->type = SVS_TYPE_NUM;
    result->value.val_s = 0;

    if (!csv_open) {
      printf((uint8_t *)"sys.fs.csv.close: WARN: No CSV file openned!\n");
      return 1;
    }

    if(svp_csv_close(&csvFile) == 0) {
      csv_open = 0;
      result->value.val_s = 1;
    } 
    
    return 1;
  }

  //#!##### New csv line
  //#!    sys.fs.csv.newLine([num]numberOfCells);
  //#!Adds new line to csv with given number of cells.
  //#!
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
  //#!
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
  //#!Cells are counted from 0.
  //#!
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
  //#!
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
  //#!
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
  //#!
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
  //#!
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
  //#!
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
  //#!
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
  //#!    sys.fs.conf.read([str]key, [str]default);
  //#!Reads key from config file as a string, 1024 chars max,
  //#!when no default value is provided and the key doesn't exist,
  //#!empty string is returnded.
  //#!
  //#!Return: [str]Value
  if (sysFuncMatch(argS->callId, "read", s)) {
    uint8_t buff[1024];
    uint8_t defaultProvided = 0;
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if (argS->usedup == 1) {
      if(sysExecTypeCheck(argS, argType, 1, s)) {
        return 0;
      }
    } else {
      if(sysExecTypeCheck(argS, argType, 2, s)) {
        return 0;
      }
      defaultProvided = 1;
    }
    
    if (!conf_open) {
      errSoft((uint8_t *)"Conf file not openned!", s);
      return 0;
    }

    // TODO: Make loading in stream mode, with checking free string mem
    if (sda_conf_key_read(&conFile, s->stringField+argS->arg[1].val_str, buff, sizeof(buff)) || defaultProvided == 0) {
      buff[sizeof(buff) - 1] = 0;
      result->value.val_u = strNew(buff, s);
      result->type = SVS_TYPE_STR;
      return 1;
    }

    result->value.val_u = argS->arg[2].val_str;
    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Read Key as int
  //#!    sys.fs.conf.readInt([str]key, [num]default);
  //#!Reads key from config file as num (integrer). To be removed.
  //#!
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
  //#!
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
  //#!
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

  //#!##### Get if key value matche
  //#!    sys.fs.conf.valMatch([str]key, [str]value, [num]caseSensitive);
  //#!Returns 1 if value matches portion of a value in a given key.
  //#!Case sensitive switch switches if the thing is case sensitive...
  //#!
  //#!Return: [num] isMatch (0 - no match, 1 - match, -1 - key not found)
  if (sysFuncMatch(argS->callId, "valMatch", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    argType[3] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 3, s)){
      return 0;
    }

    if (!conf_open) {
      errSoft((uint8_t *)"Conf file not openned!", s);
      return 0;
    }

    uint8_t r = sda_conf_key_contains(
      &conFile,
      s->stringField + argS->arg[1].val_str,
      s->stringField + argS->arg[2].val_str,
      (uint8_t)argS->arg[3].val_s
    );

    if (r == 0) result->value.val_s = 0;
    if (r == 1) result->value.val_s = 1;
    if (r == 2) result->value.val_s = -1;
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}


uint8_t sda_fs_bdb_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {

  uint8_t argType[11];

  //#!#### Binary DB API

  //#!##### Create new db file
  //#!    sys.fs.db.new([str]fname);
  //#!Creates new db file.
  //#!
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "new", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    sda_strcp(s->stringField + argS->arg[1].val_str, db_filename, sizeof(db_filename));
    if (sda_bdb_new(s->stringField + argS->arg[1].val_str, &dbFile)) {
      result->value.val_s = 1;
      db_open = 1;
    } else {
      result->value.val_s = 0;
      db_open = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Open db file
  //#!    sys.fs.db.open([str]fname);
  //#!Opens existing db file.
  //#!
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "open", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }
    sda_strcp(s->stringField + argS->arg[1].val_str, db_filename, sizeof(db_filename));
    if (sda_bdb_open(s->stringField + argS->arg[1].val_str, &dbFile)) {
      result->value.val_s = 1;
      db_open = 1;
    } else {
      result->value.val_s = 0;
      db_open = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Close config file
  //#!    sys.fs.db.close();
  //#!Close db file.
  //#!
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "close", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }
    if (db_open == 1 && (sda_bdb_close(&dbFile) == 0)) {
      db_open = 0;
      result->value.val_s = 1;
    } else {
      result->value.val_s = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Create new table
  //#!    sys.fs.db.newTable([str]name, [num] columns);
  //#!Creates new table
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "newTable", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_new_table(s->stringField+argS->arg[1].val_str, s->stringField+argS->arg[2].val_u, &dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Set column type
  //#!    sys.fs.db.setColumn([num] id, [str]name, [num] type);
  //#!Sets name and type of given column
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "setColumn", s)) {
    argType[1] = SVS_TYPE_NUM;
    argType[2] = SVS_TYPE_STR;
    argType[3] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 3, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_set_column(
      argS->arg[1].val_u, 
      s->stringField+argS->arg[2].val_str, 
      argS->arg[3].val_u,
      &dbFile
    );

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Enable ID field
  //#!    sys.fs.db.idEnable([str]fieldName);
  //#!Sets given column as an id field.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "idEnable", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_enable_id( 
      s->stringField+argS->arg[1].val_str,
      &dbFile
    );

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Select table
  //#!    sys.fs.db.selectTable([str]name);
  //#!Selects existing table
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "selectTable", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_select_table(s->stringField+argS->arg[1].val_str, &dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### New row
  //#!    sys.fs.db.newRow();
  //#!Adds new row to the selected table.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "newRow", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_new_row(&dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Select row
  //#!    sys.fs.db.selectRow([num]row_n);
  //#!Select row with given number (not an id).
  //#!Usefull for selecting row 0 and then using *sys.fs.db.nextRow();*
  //#!to read the full table line by line.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "selectRow", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_select_row(argS->arg[1].val_s, &dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Select row by id
  //#!    sys.fs.db.selectRowId([num]id);
  //#!Select row with given id. (Id field must be enabled)
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "selectRowId", s)) {
    argType[1] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_select_row_id(argS->arg[1].val_u ,&dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Next row
  //#!    sys.fs.db.nextRow();
  //#!Selects next available row.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "nextRow", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_select_row_next(&dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Write text entry
  //#!    sys.fs.db.setEntryStr([str]col_name, [str]value);
  //#!Sets db entry
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "setEntryStr", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_store_string(s->stringField+argS->arg[1].val_str,s->stringField+argS->arg[2].val_str, &dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Write numeric entry
  //#!    sys.fs.db.setEntryNum([str]col_name, [num]value);
  //#!Sets db entry
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "setEntryNum", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_set_entry(
      s->stringField+argS->arg[1].val_str, 
      &(argS->arg[2].val_s), 
      sizeof(argS->arg[2].val_s), 
      &dbFile
    );
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Read text entry
  //#!    sys.fs.db.getEntryStr([str]col_name, [str]default);
  //#!Gets db entry
  //#!
  //#!Return: [str] entry or default
  if (sysFuncMatch(argS->callId, "getEntryStr", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }
    //TODO: streaming mode

    uint8_t buffer[1024];
    if(sda_bdb_get_entry(s->stringField+argS->arg[1].val_str, &buffer, sizeof(buffer), &dbFile)) {
      result->value.val_str = strNew(buffer, s);
    } else {
      result->value.val_str = strNew(s->stringField+argS->arg[2].val_str, s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Read num entry
  //#!    sys.fs.db.getEntryNum([str]col_name, [num]default);
  //#!Gets db entry
  //#!
  //#!Return: [num] entry or default
  if (sysFuncMatch(argS->callId, "getEntryNum", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }
    //TODO: streaming mode
    int32_t val; 
    if(sda_bdb_get_entry(s->stringField+argS->arg[1].val_str, &val, sizeof(val), &dbFile)) {
      result->value.val_s = val;
    } else {
      result->value.val_s = argS->arg[2].val_s;
    }

    result->type = SVS_TYPE_NUM;
    return 1;
  }


  return 0;
}
