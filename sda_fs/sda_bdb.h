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
} sda_bdb;

uint8_t sda_bdb_set_entry_id(uint8_t id, void* data, uint32_t size, sda_bdb *db);

#endif