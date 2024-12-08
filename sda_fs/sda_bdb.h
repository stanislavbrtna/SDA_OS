/*
Copyright (c) 2024 Stanislav Brtna

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

#ifndef SDA_BINARY_DB_H
#define SDA_BINARY_DB_H
#include "svp_fs.h"

#define SDA_BDB_NAME_LEN 16

#define SDA_BDB_TYPE_NUM 0
#define SDA_BDB_TYPE_STR 1
#define SDA_BDB_TYPE_FLT 2

typedef struct {
  uint8_t  name[SDA_BDB_NAME_LEN];
  uint32_t cloumn_offset;
  uint8_t  cloumn_count;
  uint8_t  auto_id;
  uint8_t  auto_id_field;
  uint32_t max_id;
  uint8_t  valid;
  uint32_t table_size;
  uint32_t current_row_offset;
  uint8_t  current_row_valid;
  uint32_t row_count;
} sda_bdb_table;

typedef struct {
  uint32_t entry_size;
  uint8_t  entry_column;
} sda_bdb_entry;


typedef struct {
  uint8_t  name[SDA_BDB_NAME_LEN];
  uint8_t  type;
} sda_bdb_column;


typedef struct {
  svp_file      fil;
  uint8_t       valid;
  uint32_t      start_offset;
  sda_bdb_table current_table;
  uint32_t      current_table_offset;
  uint8_t       last_entry_id_en;
  uint32_t      last_entry_id;
  uint8_t       column_cache_valid;
  uint8_t       cached_column[SDA_BDB_NAME_LEN];
  uint8_t       cached_column_id;
  uint8_t       cached_column_type;

  uint32_t      table_count;
} sda_bdb;

// DB file
uint8_t sda_bdb_new(uint8_t *fname, sda_bdb *db);
uint8_t sda_bdb_open(uint8_t *fname, sda_bdb *db);
uint8_t sda_bdb_close(sda_bdb *db);
uint8_t sda_bdb_sync(sda_bdb *db);

// Tables
// Create new table
uint8_t sda_bdb_new_table(uint8_t *name, uint8_t no_columns, sda_bdb *db);
// Sets column name and type
uint8_t sda_bdb_set_column(uint8_t id, uint8_t *name, uint8_t type, sda_bdb *db);
// Select existing table
uint8_t sda_bdb_select_table(uint8_t *name, sda_bdb *db);

// Drop selected table
uint8_t sda_bdb_drop_table(sda_bdb *db);

// Drop all data from a table
uint8_t sda_bdb_drop_data(sda_bdb *db);

// Enables auto-incremented ID field
uint8_t sda_bdb_enable_auto_id(uint8_t *column_name, sda_bdb *db);

// Creates new row
uint32_t sda_bdb_new_row(sda_bdb *db);

// Drops currently selected row
uint8_t sda_bdb_drop_row(sda_bdb *db);

// Current table row count
uint32_t sda_bdb_get_row_count(sda_bdb *db);

// Select given row
uint8_t sda_bdb_select_row(uint32_t n, sda_bdb *db);

// Select next row
uint8_t sda_bdb_select_row_next(sda_bdb *db);

// Select row with given id
uint8_t sda_bdb_select_row_id(uint32_t id, sda_bdb *db);

// Select next row with given numeric value
uint8_t sda_bdb_next_row_match_num(uint8_t *column_name, uint32_t val, sda_bdb *db);

// Select row with given string value
uint8_t sda_bdb_select_row_str(
  uint8_t* column_name, 
  uint8_t  *str, 
  uint8_t  partial, 
  uint8_t  case_sensitive, 
  sda_bdb  *db
);

// Set entry
uint8_t sda_bdb_set_entry(uint8_t* name, void* data, uint32_t size, sda_bdb *db);

uint8_t sda_bdb_set_entry_id(uint8_t id, void* data, uint32_t size, sda_bdb *db);

uint8_t sda_bdb_store_string(uint8_t* column_name, uint8_t* str, sda_bdb *db);

// Read db entry
uint32_t sda_bdb_get_entry(uint8_t* name, void* buffer, uint32_t buff_size, sda_bdb *db);

#endif