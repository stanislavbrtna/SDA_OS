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

#ifndef SDA_BINARY_DB_UTILS_H
#define SDA_BINARY_DB_UTILS_H
#include "../svp_fs.h"

uint32_t sda_bdb_read_u32(svp_file *f);
void sda_bdb_write_u32(svp_file *f, uint32_t val);
uint32_t sda_bdb_new_offset(sda_bdb *db);
uint32_t sda_bdb_truncate(sda_bdb *db, uint32_t position, uint32_t size);
uint32_t sda_bdb_truncate_upto(sda_bdb *db, uint32_t position, uint32_t size);
uint32_t sda_bdb_insert_space(sda_bdb *db, uint32_t position, uint32_t size);
uint8_t sda_bdb_check_table_space(sda_bdb *db, uint32_t size);
uint32_t sda_bdb_insert_space_indb(sda_bdb *db, uint32_t position, uint32_t size);
uint32_t sda_bdb_insert_freeblock(sda_bdb *db, uint32_t position);
uint32_t sda_bdb_write_data(sda_bdb *db, uint32_t position, void* data, uint32_t size);
void sda_bdb_sync_table(sda_bdb *db);


#endif
