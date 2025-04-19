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

#ifndef SDA_BDB_INDEX_H
#define SDA_BDB_INDEX_H
#include "sda_bdb.h"

// Indexing

// Enable column index
uint8_t sda_bdb_set_column_indexing(uint8_t* col_name, sda_bdb *db);
uint8_t sda_bdb_set_column_indexing_id(uint8_t id, uint8_t indexing, sda_bdb *db);
uint32_t sda_bdb_get_index(uint32_t val, uint8_t column_id, sda_bdb *db);

// Rebuild index
uint8_t sda_bdb_rebuild_index(uint8_t *column_name, sda_bdb *db);
uint8_t sda_bdb_rebuild_index_id(uint8_t column_id, sda_bdb *db);
uint8_t sda_bdb_get_column_id(uint8_t *column_name, sda_bdb *db);

// Row indexing
uint8_t sda_bdb_enable_row_index(uint8_t val, sda_bdb *db);
uint32_t sda_bdb_get_row_index(uint32_t val, sda_bdb *db);
uint8_t sda_bdb_rebuild_row_index(sda_bdb *db);

// Internal
void sda_bdb_set_index_dirty(uint8_t column_id, uint8_t val, sda_bdb *db);
void sda_bdb_set_all_indexes_dirty(sda_bdb *db);

#endif