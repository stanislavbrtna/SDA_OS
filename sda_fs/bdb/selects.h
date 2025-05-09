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

#ifndef SDA_BINARY_DB_SELECTS_H
#define SDA_BINARY_DB_SELECTS_H
#include "../svp_fs.h"

uint8_t sda_bdb_select_row(uint32_t n, sda_bdb *db);
uint8_t sda_bdb_select_row_next(sda_bdb *db);
uint8_t sda_bdb_select_row_num_generic(uint8_t col_id, uint32_t val, sda_bdb *db);
uint8_t sda_bdb_select_row_id(uint32_t id, sda_bdb *db);
uint8_t sda_bdb_next_row_match_num(uint8_t *column_name, uint32_t val, sda_bdb *db);

uint8_t sda_bdb_select_row_str(
  uint8_t* column_name, 
  uint8_t *str, 
  uint8_t full_string, 
  uint8_t case_sensitive, 
  sda_bdb *db
);

#endif