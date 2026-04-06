/*
Copyright (c) 2025 Stanislav Brtna

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

extern sdaSvmMetadata svmMeta;

extern sda_bdb dbFile;
extern uint8_t db_filename[64];
extern volatile uint8_t db_open;

uint8_t * sda_get_db_fname() {
  if (db_open) {
    return db_filename;
  } else {
    return 0;
  }
}

uint8_t sda_fs_bdb_wrapper(varRetVal *result, argStruct *argS, svsVM *s) {

  uint8_t argType[11];

  //#!#### Binary DB API
  //#!
  //#!Sda supports its own binary data format. It is quick, universal
  //#!but not easily readable or recoverable when data corruption occurs.  
  //#!

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
    sda_strcp("", svmMeta.openDbTable, sizeof(svmMeta.openDbTable));
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

  //#!##### Sync file
  //#!    sys.fs.db.sync();
  //#!Writes all the file changes to the card.
  //#!Same as when file is closed, but can be triggered on demand.
  //#!
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "sync", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (db_open == 1) {
      sda_bdb_sync(&dbFile);
      result->value.val_s = 1;
    } else {
      result->value.val_s = 0;
    }
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Close db file
  //#!    sys.fs.db.close();
  //#!Close db file.
  //#!
  //#!Return: [num]1 on succes.
  if (sysFuncMatch(argS->callId, "close", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    sda_strcp("", svmMeta.openDbTable, sizeof(svmMeta.openDbTable));
    if (db_open == 1) {
      if(!sda_bdb_close(&dbFile)) {
        errSoft((uint8_t *)"Failed to close DB!", s);
      }
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

    sda_strcp(s->stringField + argS->arg[1].val_str, svmMeta.openDbTable, sizeof(svmMeta.openDbTable));
    result->value.val_s = sda_bdb_new_table(s->stringField+argS->arg[1].val_str, argS->arg[2].val_u, &dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Drop Data
  //#!    sys.fs.db.dropAllRows();
  //#!Drops all data from currently selected table.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "dropAllRows", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_drop_data(&dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Drop table
  //#!    sys.fs.db.dropTable();
  //#!Drops currently selected table.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "dropTable", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_drop_table(&dbFile);
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

  //#!##### Enable table row index
  //#!    sys.fs.db.setRowIndex([num] val);
  //#!Sets row indexing. (1 - enabled, 0 - disabled)
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "setRowIndex", s)) {
    argType[1] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_enable_row_index(
      argS->arg[1].val_u,
      &dbFile
    );

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Rebuild table row index
  //#!    sys.fs.db.buildRowIndex();
  //#!Builds row index.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "buildRowIndex", s)) {

    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_rebuild_row_index(
      &dbFile
    );

    result->type = SVS_TYPE_NUM;
    return 1;
  }


  //#!##### Set column index type
  //#!    sys.fs.db.setIndex([str] column_name);
  //#!Sets index type of a given column. Column type is inferred from the column type.
  //#!String columns are indexed as hashes, float and num columns are indexed as value.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "setIndex", s)) {
    argType[1] = SVS_TYPE_STR;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_set_column_indexing(
      s->stringField+argS->arg[1].val_str,
      &dbFile
    );

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Build column index
  //#!    sys.fs.db.buildIndex([str] column_name);
  //#!Builds/rebuilds index for a given column.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "buildIndex", s)) {
    argType[1] = SVS_TYPE_STR;

    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_rebuild_index(
      s->stringField+argS->arg[1].val_str,
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

    result->value.val_s = sda_bdb_enable_auto_id( 
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
    
    sda_strcp(s->stringField + argS->arg[1].val_str, svmMeta.openDbTable, sizeof(svmMeta.openDbTable));
    result->value.val_s = sda_bdb_select_table(s->stringField+argS->arg[1].val_str, &dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### New row
  //#!    sys.fs.db.newRow();
  //#!Adds new row to the selected table.
  //#!New row is selected automatically.
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

  //#!##### Drop row
  //#!    sys.fs.db.dropRow();
  //#!Drops current row from the slected table.
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "dropRow", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_drop_row(&dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Get Row count
  //#!    sys.fs.db.getRowCount();
  //#!Gets row count of the selected table.
  //#!
  //#!Return: [num] row count.
  if (sysFuncMatch(argS->callId, "getRowCount", s)) {
    if(sysExecTypeCheck(argS, argType, 0, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_get_row_count(&dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Select row
  //#!    sys.fs.db.selectRow([num]row_n);
  //#!Select row with given number (not an id).
  //#!Usefull for selecting row 0 and then using *sys.fs.db.nextRow();*
  //#!to read the full table line by line.
  //#!Note: When app is suspended, selected row is forgotten.
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

  //#!##### Select next matching row
  //#!    sys.fs.db.selectRowNum([str]column, [num]val);
  //#!Selectcs next row where given column has given value
  //#!
  //#!Return: [num] 1 if ok.
  if (sysFuncMatch(argS->callId, "selectRowNum", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_NUM;
    if(sysExecTypeCheck(argS, argType, 2, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_next_row_match_num(s->stringField+argS->arg[1].val_str, argS->arg[2].val_u, &dbFile);
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Select next row matching string
  //#!    sys.fs.db.selectRowStr([str]column, [str]string, [num]full_string, [num]case_sensitive);
  //#!Selectcs next row where given column has given value.
  //#!
  //#! | Parameter      | Value | Meaning                                 |
  //#! | ---            | ---   | ---                                     |
  //#! | full_string    |   1   | full string must be contained           |
  //#! |                |   0   | string can be only a part odf the entry |
  //#! | case_sensitive |   1   | strings are matched case-sensitive      |
  //#! |                |   0   | strings are matched non case-sensitive  |
  //#!
  //#!Return: [num] 1 if entry was found.
  if (sysFuncMatch(argS->callId, "selectRowStr", s)) {
    argType[1] = SVS_TYPE_STR;
    argType[2] = SVS_TYPE_STR;
    argType[3] = SVS_TYPE_NUM;
    argType[4] = SVS_TYPE_NUM;

    if(sysExecTypeCheck(argS, argType, 4, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    result->value.val_s = sda_bdb_select_row_str(
      s->stringField+argS->arg[1].val_str, 
      s->stringField+argS->arg[2].val_str, 
      (uint8_t)argS->arg[3].val_u, 
      (uint8_t)argS->arg[4].val_u, 
      &dbFile
    );
    result->type = SVS_TYPE_NUM;
    return 1;
  }

  //#!##### Write entry
  //#!    sys.fs.db.setEntry([str]col_name, [str/num]value);
  //#!Sets db entry. Produces error when the write operation fails.
  //#!
  //#!Return: None.
  if (sysFuncMatch(argS->callId, "setEntry", s)) {
    argType[1] = SVS_TYPE_STR;
    
    if(argS->usedup != 2) {
      errSoft((uint8_t *)"sys.fs.db.setEntry: Wrong count of arguments.", s);
      return 0;
    }

    if(argS->argType[1] != SVS_TYPE_STR) {
      errSoft((uint8_t *)"sys.fs.db.setEntry: Wrong type of argument col_name, only string is accepted.", s);
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }

    uint8_t columnType = sda_bdb_get_column_type(s->stringField+argS->arg[1].val_str, &dbFile);
    if (columnType == 8) {
      errSoft((uint8_t *)"sys.fs.db.setEntry: Column does not exist!", s);
      return 0;
    } else if (columnType != argS->argType[2]) {
      errSoft((uint8_t *)"sys.fs.db.setEntry: Argument type does not match the column type!", s);
      return 0;
    }

    uint8_t res = 0;

    if (argS->argType[2] == SVS_TYPE_STR) {
      res = sda_bdb_store_string(
        s->stringField + argS->arg[1].val_str,
        s->stringField + argS->arg[2].val_str,
        &dbFile
      );
    } else if (argS->argType[2] == SVS_TYPE_NUM) {
      res = sda_bdb_set_entry(
        s->stringField + argS->arg[1].val_str, 
        &(argS->arg[2].val_s), 
        sizeof(argS->arg[2].val_s), 
        &dbFile
      );
    } else {
      errSoft((uint8_t *)"sys.fs.db.setEntry: Unrecognized type to store.", s);
      return 0;
    }

    if (!res) {
      errSoft((uint8_t *)"sys.fs.db.setEntry: Write failed.", s);
      return 0;
    }

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

    result->value.val_s = sda_bdb_store_string(s->stringField+argS->arg[1].val_str, s->stringField+argS->arg[2].val_str, &dbFile);
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

  //#!##### Read entry
  //#!    sys.fs.db.getEntry([str]col_name);
  //#!Gets db entry, returns type according to the entry type.
  //#!Produces error when the read fails.
  //#!
  //#!Return: [str or num] entry
  if (sysFuncMatch(argS->callId, "getEntry", s)) {
    argType[1] = SVS_TYPE_STR;
    if(sysExecTypeCheck(argS, argType, 1, s)) {
      return 0;
    }

    if (!db_open) {
      errSoft((uint8_t *)"DB file not openned!", s);
      return 0;
    }
    uint8_t entryType = sda_bdb_get_column_type(s->stringField+argS->arg[1].val_str, &dbFile);

    if (entryType == SVS_TYPE_STR) {
      //TODO: streaming mode
      uint8_t buffer[1024];
      if(sda_bdb_get_entry(s->stringField+argS->arg[1].val_str, &buffer, sizeof(buffer), &dbFile)) {
        result->value.val_str = strNew(buffer, s);
        result->type = SVS_TYPE_STR;
        return 1;
      } else {
        errSoft("sys.fs.db.getEntry: Read failed.", s);
        return 0;
      }
    } else if (entryType == SVS_TYPE_NUM) {
      int32_t val; 
      if(sda_bdb_get_entry(s->stringField+argS->arg[1].val_str, &val, sizeof(val), &dbFile)) {
        result->value.val_s = val;
      } else {
        errSoft("sys.fs.db.getEntry: Read failed.", s);
        return 0;
      }
      result->type = SVS_TYPE_NUM;
      return 1;
    }
    printf("type: %u\n", entryType);
    errSoft("sys.fs.db.getEntry: Read failed, unrecognized type.", s);
    return 0;
  }

  //#!##### Read text entry
  //#!    sys.fs.db.getEntryStr([str]col_name, [str]default);
  //#!Gets db entry of type string, returns default when error occurs.
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
      printf(
        "WARN: sys.fs.db.getEntryStr: get_entry failed (col: %s), returning default.\n",
        s->stringField+argS->arg[1].val_str
      );
      result->value.val_str = strNew(s->stringField+argS->arg[2].val_str, s);
    }

    result->type = SVS_TYPE_STR;
    return 1;
  }

  //#!##### Read num entry
  //#!    sys.fs.db.getEntryNum([str]col_name, [num]default);
  //#!Gets db entry of type Num, returns default when error occurs.
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
      printf(
        "WARN: sys.fs.db.getEntryNum: get_entry failed (col: %s), returning default.\n",
        s->stringField+argS->arg[1].val_str
      );
      result->value.val_s = argS->arg[2].val_s;
    }

    result->type = SVS_TYPE_NUM;
    return 1;
  }

  return 0;
}
