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

#include "sda_bdb.h"

/* create/read header
Header data:
BDB header
32b  version
32b table 0 offset
---
table:
table size
table name 16B
row 0 offset
max_id
8b columns count
auto_id fields
  - enabled
  - max id
  - auto id column
column:
    16B column name
    8b  column type (Number(signed/unsigned)/string/float)
---
Row:
    32b next row offset
Entry:
    32b next enty offset
    32b data size
     nb data 

Function return:
 0 - error
 1 - ok

*/

// internal

#define START_OFFSET 4 + sizeof(uint32_t)

uint32_t sda_bdb_read_u32(svp_file *f) {
  uint32_t val = 0;
  svp_fread(f, &val, sizeof(uint32_t));
  return val;
}

void sda_bdb_write_u32(svp_file *f, uint32_t val) {
  svp_fwrite(f, &val, sizeof(uint32_t));
}


uint8_t sda_bdb_new(uint8_t *fname, sda_bdb *db) {

  if(svp_fexists(fname)) {
    printf("%s: file already exists\n", __FUNCTION__);
    return 0;
  }

  svp_fopen_rw(&(db->fil), fname);
  // header
  svp_fwrite_u8(&(db->fil), 'B');
  svp_fwrite_u8(&(db->fil), 'D');
  svp_fwrite_u8(&(db->fil), 'B');
  svp_fwrite_u8(&(db->fil), '0'); // padding
  // version
  sda_bdb_write_u32(&(db->fil), 1);
  // offset (now 0)
  svp_fwrite_u8(&(db->fil), 0);
  svp_fwrite_u8(&(db->fil), 0);
  svp_fwrite_u8(&(db->fil), 0);
  svp_fwrite_u8(&(db->fil), 0);

  db->current_table.valid = 0;
  db->valid = 1;
  db->start_offset = 0;
  svp_fsync(&(db->fil));

  return 1;
}


uint8_t sda_bdb_open(uint8_t *fname, sda_bdb *db) {
  if(!svp_fexists(fname)) {
    printf("%s: file \"%s\" does not exists\n", __FUNCTION__, fname);
    return 0;
  }

  svp_fopen_rw(&(db->fil), fname);

  //TODO: header check

  svp_fseek(&(db->fil), START_OFFSET);

  db->valid = 1;
  db->current_table.valid = 0;
  db->start_offset = sda_bdb_read_u32(&(db->fil));
  
  return 1;
}


uint8_t sda_bdb_close(sda_bdb *db) {
  if(db->current_table.valid) {
    sda_bdb_sync_table(db);
  }
  db->valid = 0;
  svp_fclose(&(db->fil));
}

// Misc internal

uint8_t sda_bdb_table_exists(uint8_t *name, sda_bdb *db) {
  // empty db
  if(db->start_offset == 0) {
    return 0;
  }

  // walk db
  uint32_t offset = 0;
  uint32_t offset_pre = 0;
  sda_bdb_table t;

  svp_fseek(&(db->fil), db->start_offset);

  while(svp_ftell(&(db->fil)) <= svp_get_size(&(db->fil))) {
    
    uint32_t table_start = svp_ftell(&(db->fil));
    svp_fread(&(db->fil), &t, sizeof(t));
    offset = t.table_size;

    if(svp_strcmp(name, t.name)) {
      return 1;
    }

    svp_fseek(&(db->fil), table_start + offset);
  }

  return 0;
}

//C(F)RUD

// Create
uint8_t sda_bdb_new_table(uint8_t *name, uint8_t no_columns, sda_bdb *db) {
  
  if(db->current_table.valid) {
    sda_bdb_sync_table(db);
  }

  // validate table name
  if(sda_bdb_table_exists(name, db)) {
    printf("%s: table \"%s\" already exists\n", __FUNCTION__, name);
    return 0;
  }

  // seek end
  uint32_t table_begin = svp_get_size(&(db->fil));
  svp_fseek(&(db->fil), table_begin);
  
  sda_strcp(name, db->current_table.name, SDA_BDB_NAME_LEN);
  db->current_table.auto_id = 0;
  db->current_table.auto_id_field = 0;
  db->current_table.max_id = 0;
  db->current_table.valid = 1;
  db->current_table.cloumn_count = no_columns;
  db->current_table.table_size = sizeof(sda_bdb_table) + no_columns*sizeof(sda_bdb_column);
  db->current_table.row_count = 0;
  db->current_table_offset = table_begin;

  // write header
  svp_fwrite(&(db->fil), &db->current_table, sizeof(sda_bdb_table));
  
  // reserve space for columns
  for (uint32_t i = 0; i < ((uint32_t)no_columns)*sizeof(sda_bdb_column); i++) {
    svp_fwrite_u8(&(db->fil), 0);
  }
  
  // update first table
  if(db->start_offset == 0) {
    svp_fseek(&(db->fil), START_OFFSET);
    sda_bdb_write_u32(&(db->fil), table_begin);
    db->start_offset = table_begin;
  }

  svp_fsync(&(db->fil));
  return 1;
}

// set column name and type
uint8_t sda_bdb_set_column(uint8_t id, uint8_t *name, uint8_t type, sda_bdb *db) {
  sda_bdb_column c;

  if(id >= db->current_table.cloumn_count) {
    printf("%s: column: \"%s\" id greater than column count!\n", __FUNCTION__, name);
    return 0;
  }

  sda_strcp(name, c.name, SDA_BDB_NAME_LEN);
  c.type = type;

  svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table) + sizeof(sda_bdb_column) * id);
  svp_fwrite(&(db->fil), &c, sizeof(sda_bdb_column));

  return 1;
}

// select table
uint8_t sda_bdb_select_table(uint8_t *name, sda_bdb *db) {
  // validate table name
  if(!sda_bdb_table_exists(name, db)) {
    printf("%s: table \"%s\" does not exist\n", __FUNCTION__, name);
    return 0;
  }

  if(db->current_table.valid) {
    sda_bdb_sync_table(db);
  }
  
  // walk db
  uint32_t offset = 0;
  uint32_t offset_pre = 0;
  sda_bdb_table t;

  svp_fseek(&(db->fil), db->start_offset);

  while(svp_ftell(&(db->fil)) <= svp_get_size(&(db->fil))) {
    
    uint32_t table_start = svp_ftell(&(db->fil));
    svp_fread(&(db->fil), &t, sizeof(t));
    offset = t.table_size;

    if(svp_strcmp(name, t.name)) {
      db->current_table_offset = table_start;
      break;
    }

    svp_fseek(&(db->fil), table_start + offset);
  }

  svp_fseek(&(db->fil), db->current_table_offset);
  svp_fread(&(db->fil), &(db->current_table), sizeof(sda_bdb_table));
  db->current_table.valid = 1;

  return 1;
}

// Todo: Remove table

uint32_t sda_bdb_truncate(sda_bdb *db, uint32_t position, uint32_t size) {
  uint32_t file_end = svp_get_size(&(db->fil));
  for(uint32_t i = position; i + size < file_end; i++) {
    svp_fseek(&(db->fil), i + size);
    uint8_t c = svp_fread_u8(&(db->fil));
    svp_fseek(&(db->fil), i);
    svp_fwrite_u8(&(db->fil), c);
  }
  svp_fseek(&(db->fil), file_end - size);
  svp_truncate(&(db->fil));
}


uint32_t sda_bdb_insert_space(sda_bdb *db, uint32_t position, uint32_t size) {
  uint32_t file_end = svp_get_size(&(db->fil));

  svp_fseek(&(db->fil), svp_get_size(&(db->fil)));

  if(position + size < file_end) { 
    for(uint32_t i = 0; i <= file_end - position; i++) {
      svp_fseek(&(db->fil), file_end - i);
      uint8_t c = svp_fread_u8(&(db->fil));
      svp_fseek(&(db->fil), file_end + size - i);
      svp_fwrite_u8(&(db->fil), c);
    }
  }
}

uint32_t sda_bdb_insert_data(sda_bdb *db, uint32_t position, void* data, uint32_t size) {
  sda_bdb_insert_space(db, position, size);
  svp_fseek(&(db->fil), position);
  svp_fwrite(&(db->fil), data, size);
  svp_fseek(&(db->fil), position + size);
}

void sda_bdb_sync_table(sda_bdb *db) {
  svp_fseek(&(db->fil), db->current_table_offset);
  svp_fwrite(&(db->fil), &(db->current_table), sizeof(sda_bdb_table));
}

// new row -> creates new row and sets it as current 
uint32_t sda_bdb_new_row(sda_bdb *db) {
  uint32_t siz = db->current_table.cloumn_count*sizeof(sda_bdb_entry);
  uint32_t offset = db->current_table_offset + sizeof(sda_bdb_table) + db->current_table.cloumn_count*sizeof(sda_bdb_column);
  
  // get last row
  if(db->current_table.row_count != 0) {
    offset = db->current_table_offset + db->current_table.table_size;
  }

  // insert data
  sda_bdb_insert_data(
    db, 
    offset,
    &siz,
    sizeof(uint32_t)
  );
  
  // prepare entries
  sda_bdb_entry e;
  for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
    e.entry_column = i;
    e.entry_size = 0;
    sda_bdb_insert_data(
      db, 
      svp_ftell(&(db->fil)),
      &e,
      sizeof(e)
    );
  }

  // Update table size
  db->current_table.table_size += sizeof(uint32_t) + siz;
  db->current_table.row_count++;
  db->current_table.current_row_offset = offset;

  // increment autoId
  if(db->current_table.auto_id) {
    //printf("auto id increment: %u\n", db->current_table.max_id);
    sda_bdb_set_entry_id(db->current_table.auto_id_field, &(db->current_table.max_id), sizeof(uint32_t), db);
    db->current_table.max_id++;
  }

  return 1;
}

uint8_t sda_bdb_gc_type;

uint8_t sda_bdb_get_column_id(uint8_t *column_name, sda_bdb *db) {
  sda_bdb_column c;
  sda_strcp("", c.name, SDA_BDB_NAME_LEN);

  svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table));
  
  for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
    svp_fread(&(db->fil), &c, sizeof(sda_bdb_column));
    if(svp_strcmp(c.name, column_name)) {
      sda_bdb_gc_type = c.type;
      return i + 1;
    }
  }
  sda_bdb_gc_type = 0;
  return 0;
}


uint8_t sda_bdb_enable_auto_id(uint8_t *column_name, sda_bdb *db) {
  if(db->current_table.auto_id) {
    printf("%s: table already has an id column!\n", __FUNCTION__);
    return 0;
  }

  db->current_table.auto_id_field = sda_bdb_get_column_id(column_name, db) - 1;
  if (sda_bdb_gc_type != SDA_BDB_TYPE_NUM) {
    printf("%s: column \"%s\" is not type NUM!\n", __FUNCTION__, column_name);
    return 0;
  }

  db->current_table.auto_id = 1;
  db->current_table.max_id = 0;
  sda_bdb_sync_table(db);
  return 1;
}

// Update

// fill entry data
uint8_t sda_bdb_set_entry_id(uint8_t id, void* data, uint32_t size, sda_bdb *db) {

  // find if given row entry exists
  // seek on row start
  svp_fseek(&(db->fil), db->current_table.current_row_offset);
  uint32_t row_size = 0;
  svp_fread(&(db->fil), &row_size, sizeof(uint32_t));
  
  sda_bdb_entry e;
  
  // if row is not empty, then walk entries
  uint32_t offset = db->current_table.current_row_offset + sizeof(uint32_t);

  if(row_size != 0) {
    for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
      svp_fread(&(db->fil), &e, sizeof(sda_bdb_entry));
      if(e.entry_column == id) {
        // compare sizes
        if(size == e.entry_size) {
          svp_fwrite(&(db->fil), data, size);
          return 1;
        }

        // prodloužit/zkrátit soubor
        if(size > e.entry_size) {
          uint32_t entry_pos = svp_ftell(&(db->fil));
          sda_bdb_insert_space(db, entry_pos, size - e.entry_size);
          svp_fseek(&(db->fil), entry_pos);
          svp_fwrite(&(db->fil), data, size);
          row_size += size - e.entry_size;
          db->current_table.table_size += size - e.entry_size;
        } else if(size < e.entry_size) {
          uint32_t pos = svp_ftell(&(db->fil));
          sda_bdb_truncate(db, pos, e.entry_size - size);
          svp_fseek(&(db->fil), pos);    
          svp_fwrite(&(db->fil), data, size);
          row_size -= e.entry_size - size;
          db->current_table.table_size -= e.entry_size - size;
        }

        // update entry size
        e.entry_size = size;
        svp_fseek(&(db->fil), offset);
        svp_fwrite(&(db->fil), &e, sizeof(sda_bdb_entry));
        //printf("updating %u total size: %u\n", id, size + sizeof(sda_bdb_entry));

        // update row header
        svp_fseek(&(db->fil), db->current_table.current_row_offset);
        svp_fwrite(&(db->fil), &row_size, sizeof(uint32_t));
        return 1;
      }

      offset += e.entry_size + sizeof(sda_bdb_entry);

      if(offset > db->current_table.current_row_offset + row_size) {
        break;
      }
      svp_fseek(&(db->fil), offset);
    }
  }
  
  // if not: create and write
  e.entry_column = id;
  e.entry_size = size;
  uint32_t begin = svp_ftell(&(db->fil));
  // write header
  sda_bdb_insert_data(
    db, 
    db->current_table.current_row_offset + sizeof(uint32_t),
    &e,
    sizeof(sda_bdb_entry)
  );
  // write data
  sda_bdb_insert_data(
    db, 
    db->current_table.current_row_offset + sizeof(uint32_t) + sizeof(sda_bdb_entry),
    data,
    size
  );
  row_size += size + sizeof(sda_bdb_entry);
  //printf("size: %u, real: %u \n", size + sizeof(sda_bdb_entry), svp_ftell(&(db->fil)) - begin);

  // update row header
  svp_fseek(&(db->fil), db->current_table.current_row_offset);
  svp_fwrite(&(db->fil), &row_size, sizeof(uint32_t));

  // sync db
  db->current_table.table_size += size + sizeof(sda_bdb_entry);

  return 1;
}


uint8_t sda_bdb_set_entry(uint8_t* name, void* data, uint32_t size, sda_bdb *db) {
  // get column id
  uint8_t id = sda_bdb_get_column_id(name, db);

  if(!id) {
    printf("%s: column \"%s\" does not exist\n", __FUNCTION__, name);
    return 0;
  }
  id--;

  return sda_bdb_set_entry_id(id, data, size, db);
}

uint8_t sda_bdb_store_string(uint8_t* column_name, uint8_t* str, sda_bdb *db) {
  // todo: somewhat fix the +1 error

  uint8_t id = sda_bdb_get_column_id(column_name, db);

  if(!id) {
    printf("%s: column \"%s\" does not exist\n", __FUNCTION__, column_name);
    return 0;
  }
  id--;

  if(sda_bdb_gc_type != SDA_BDB_TYPE_STR) {
    printf("%s: column \"%s\" type mismatch! (%u, %u)\n", __FUNCTION__, column_name, sda_bdb_gc_type, SDA_BDB_TYPE_STR);
    return 0;
  }

  return sda_bdb_set_entry_id(id, str, sda_strlen(str) + 1, db);
}

// Read
uint32_t sda_bdb_get_entry_size(uint8_t* name, sda_bdb *db) {
  uint8_t id = sda_bdb_get_column_id(name, db);

  if(!id) {
    printf("%s: column \"%s\" does not exist\n", __FUNCTION__, name);
    return 0;
  }
  id--;

  svp_fseek(&(db->fil), db->current_table.current_row_offset);
  uint32_t row_size = 0;
  svp_fread(&(db->fil), &row_size, sizeof(uint32_t));

  if(row_size == 0) {
    printf("%s: Row is empty!", __FUNCTION__);
    return 0;
  }
  
  sda_bdb_entry e;
  
  // if row is not empty, then walk entries
  uint32_t offset = db->current_table.current_row_offset + sizeof(uint32_t);

  if(row_size != 0) {
    for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
      svp_fread(&(db->fil), &e, sizeof(sda_bdb_entry));
      if(e.entry_column == id) {
        return e.entry_size;
      }
      offset += e.entry_size + sizeof(sda_bdb_entry);
      if(offset > db->current_table.current_row_offset + row_size) {
        break;
      }
      svp_fseek(&(db->fil), offset);
    }
  }

  return 0;
}

// returns entry size (bytes read)
uint32_t sda_bdb_get_entry(uint8_t* name, void* buffer, uint32_t buff_size, sda_bdb *db) {
  uint8_t id = sda_bdb_get_column_id(name, db);

  if(!id) {
    printf("%s: column \"%s\" does not exist\n", __FUNCTION__, name);
    return 0;
  }
  id--;

  svp_fseek(&(db->fil), db->current_table.current_row_offset);
  uint32_t row_size = 0;
  svp_fread(&(db->fil), &row_size, sizeof(uint32_t));

  if(row_size == 0) {
    printf("%s: Row is empty!", __FUNCTION__);
    return 0;
  }
  
  sda_bdb_entry e;
  
  // if row is not empty, then walk entries
  uint32_t offset = db->current_table.current_row_offset + sizeof(uint32_t);

  if(row_size != 0) {
    for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
      svp_fread(&(db->fil), &e, sizeof(sda_bdb_entry));
      if(e.entry_column == id) {
        if(e.entry_size > buff_size) {
          printf("%s: column \"%s\" is larger than the given buffer\n", __FUNCTION__, name);
          return 0;
        }
        
        svp_fread(&(db->fil), buffer, e.entry_size);

        return e.entry_size;
      }

      offset += e.entry_size + sizeof(sda_bdb_entry);

      if(offset > db->current_table.current_row_offset + row_size) {
        break;
      }
      svp_fseek(&(db->fil), offset);
    }
  }

  return 0;
}

// misc
// gets row count for the selected table
uint32_t sda_bdb_get_row_count(sda_bdb *db) {
  if(db->valid) {
    return db->current_table.row_count;
  }
  return 0;
}

// Find
/*
Select row:
nth, next

Find row:
  find_setup
  find_next

column match (num/str)

column range (float/signed/unsigned)

text contains

*/

// TODO: insert column
// TODO: drop column

// Delete
// TODO: drop table
// TODO: drop all rows
// TODO: drop row

// Select row
uint8_t sda_bdb_select_row(uint32_t n, sda_bdb *db) {
  uint32_t offset = db->current_table_offset + sizeof(sda_bdb_table) + db->current_table.cloumn_count*sizeof(sda_bdb_column);
    
  svp_fseek(&(db->fil), offset);

  for(uint32_t i = 0; i < db->current_table.row_count; i++) {
    uint32_t row_size = 0;
    svp_fread(&(db->fil), &row_size, sizeof(uint32_t));

    if(i == n) {
      db->current_table.current_row_offset = offset;
      db->current_table.current_row_valid  = 1;
      return 1;
    }

    offset += row_size + sizeof(uint32_t);
    svp_fseek(&(db->fil), offset);
  }

  db->current_table.current_row_valid = 0;
  return 0;
}

// Select next
uint8_t sda_bdb_select_row_next(sda_bdb *db) {
  uint32_t offset = db->current_table.current_row_offset;
  svp_fseek(&(db->fil), offset);
  uint32_t row_size = 0;

  svp_fread(&(db->fil), &row_size, sizeof(uint32_t));
  
  offset += row_size + sizeof(uint32_t);
  
  if(offset < db->current_table_offset + db->current_table.table_size) {
    svp_fseek(&(db->fil), offset);
    db->current_table.current_row_offset = offset;
    db->current_table.current_row_valid  = 1;
    return 1;
  }

  db->current_table.current_row_valid = 0;
  return 0;
}

// if entry is not found, current row is kept the same
uint8_t sda_bdb_select_row_num_generic(uint8_t col_id, uint32_t val, sda_bdb *db) {
  uint32_t offset = db->current_table.current_row_offset;
  
  if(!db->current_table.current_row_valid) {
    return 0;
  }

  svp_fseek(&(db->fil), offset);

  for(uint32_t i = 0; i < db->current_table.row_count; i++) {
    uint32_t row_size = 0;
    svp_fread(&(db->fil), &row_size, sizeof(uint32_t));

    uint32_t entry_offset = offset + sizeof(uint32_t);
    for(uint8_t id = 0; id < db->current_table.cloumn_count; id++) {
      sda_bdb_entry e;
      svp_fread(&(db->fil), &e, sizeof(e));
      if(e.entry_column == col_id) {
        // read, compare
        uint32_t entry_id = 0;
        svp_fread(&(db->fil), &entry_id, sizeof(uint32_t));
        if(val == entry_id) {
          db->current_table.current_row_offset = offset;
          return 1;
        }
      }
      
      entry_offset += e.entry_size + sizeof(sda_bdb_entry);
      svp_fseek(&(db->fil), entry_offset);
    }

    offset += row_size + sizeof(uint32_t);
    svp_fseek(&(db->fil), offset);
  }
  return 0;
}


uint8_t sda_bdb_select_row_id(uint32_t id, sda_bdb *db) {
  if(!db->current_table.auto_id) {
    printf("%s: auto ID not enabled on table \"%s\"\n", __FUNCTION__, db->current_table.name);
    return 0;
  }
  // rewinds table
  sda_bdb_select_row(0, db);

  return sda_bdb_select_row_num_generic(db->current_table.auto_id_field, id, db);
}


// finds next matching row
uint8_t sda_bdb_next_row_match_num(uint8_t *column_name, uint32_t val, sda_bdb *db) {

  uint8_t col_id = sda_bdb_get_column_id(column_name, db); 

  if(!col_id) {
    printf("%s: no column named \"%s\"\n", __FUNCTION__, column_name);
    return 0;
  }

  if(sda_bdb_gc_type != SDA_BDB_TYPE_NUM) {
    printf("%s: column \"%s\" is not type NUM\n", __FUNCTION__, column_name);
    return 0;
  }

  col_id--;
  return sda_bdb_select_row_num_generic(col_id, val, db);
}

// internal
uint8_t sda_bdb_entry_contains(uint32_t entry_size, uint8_t *value, uint8_t partial, uint8_t case_sensitive, sda_bdb *db) {
  uint32_t x = 0;
  uint32_t pos = 0;

  while (pos < entry_size) {
    uint8_t c = svp_fread_u8(&(db->fil));
    uint8_t cmp = 0;
    pos++;

    if (case_sensitive == 1) {
      cmp = value[x] == c;
    } else {
      cmp = sda_str_lower(value[x]) == sda_str_lower(c);
    }

    if (cmp) {
      x++;
    } else {
      if (partial) {
        // rewind
        if (x != 0) {
          x = 0;
        }
      } else {
        return 0;
      }
    }
    if (value[x] == 0) {
      return 1;
    }
  }
  return 0;
}


// select row based on its string value
uint8_t sda_bdb_select_row_str(
  uint8_t* column_name, 
  uint8_t *str, 
  uint8_t partial, 
  uint8_t case_sensitive, 
  sda_bdb *db
){

  uint8_t col_id = sda_bdb_get_column_id(column_name, db); 

  if(!col_id) {
    printf("%s: no column named \"%s\"\n", __FUNCTION__, column_name);
    return 0;
  }

  if(sda_bdb_gc_type != SDA_BDB_TYPE_STR) {
    printf("%s: column \"%s\" is not type NUM\n", __FUNCTION__, column_name);
    return 0;
  }

  if(!db->current_table.current_row_valid) {
    return 0;
  }

  col_id--;

  uint32_t offset = db->current_table.current_row_offset;
  svp_fseek(&(db->fil), offset);

  for(uint32_t i = 0; i < db->current_table.row_count; i++) {
    uint32_t row_size = 0;
    svp_fread(&(db->fil), &row_size, sizeof(uint32_t));

    uint32_t entry_offset = offset + sizeof(uint32_t);
    for(uint8_t id = 0; id < db->current_table.cloumn_count; id++) {
      sda_bdb_entry e;
      svp_fread(&(db->fil), &e, sizeof(e));
      if(e.entry_column == col_id) {
        // read, compare
        if(sda_bdb_entry_contains(e.entry_size, str, partial, case_sensitive, db)) {
          db->current_table.current_row_offset = offset;
          return 1;
        }
      }
      entry_offset += e.entry_size + sizeof(sda_bdb_entry);
      svp_fseek(&(db->fil), entry_offset);
    }

    offset += row_size + sizeof(uint32_t);
    svp_fseek(&(db->fil), offset);
  } 
  return 0;
}

// ok, search:
//uint8_t sda_bdb_select_row_match(uint8_t *col_name, void* data, uint32_t size)
/*
next row match
- skips rows until one of them matches
- can be called until 0 is returned, that signals end of table

seek start
loop(match) {
  get value, do some logic, push value in an array
  next_row()
} -> at the end of the loop, all matching entries are processed/added pushed in an array

process results

*/

// DBG:
// print_db

void sda_bdb_list_table(sda_bdb *db) {
  printf("Selected table: %s \nColumns:\n", db->current_table.name);

  svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table));
  for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
    sda_bdb_column c;
    svp_fread(&(db->fil), &c, sizeof(sda_bdb_column));
    printf("%u: %s\n", i, c.name);
  }

  printf("%u rows\n", db->current_table.row_count);
  
  uint32_t offset = db->current_table_offset + sizeof(sda_bdb_table) + db->current_table.cloumn_count*sizeof(sda_bdb_column);
    
  for(uint32_t i = 0; i < db->current_table.row_count; i++) {
    uint32_t row_size = 0;
    svp_fread(&(db->fil), &row_size, sizeof(uint32_t));
    printf("Row %u, (size:%u):\n", i, row_size);
    
    uint32_t entry_offset = offset + sizeof(uint32_t);
    for(uint8_t id = 0; id < db->current_table.cloumn_count; id++) {
      sda_bdb_entry e;
      svp_fread(&(db->fil), &e, sizeof(e));
      printf("  Entry: id:%u, size:%u (%u total)\n", e.entry_column, e.entry_size, e.entry_size + sizeof(sda_bdb_entry));
      entry_offset += e.entry_size + sizeof(sda_bdb_entry);
      svp_fseek(&(db->fil), entry_offset);
    }

    offset += row_size + sizeof(uint32_t);
    svp_fseek(&(db->fil), offset);
  }
  
}

void svs_bdb_test() {
  sda_bdb test;
  if(svp_fexists("test.bdb")) {
    sda_bdb_open("test.bdb", &test);
  } else {
    sda_bdb_new("test.bdb", &test);
  }
  
  if (sda_bdb_new_table("table1", 5, &test)) {
    sda_bdb_set_column(0, "sloupec_1", 0, &test);
    sda_bdb_set_column(1, "id", 0, &test);
    sda_bdb_set_column(2, "sloupec_2", 0, &test);
    sda_bdb_set_column(3, "sloupec_3", 0, &test);
  } else {
    sda_bdb_select_table("table1", &test);
  }

  sda_bdb_enable_auto_id("id", &test);

  sda_bdb_select_table("table1", &test);
  for(int i = 0; i < 5; i++) {
    sda_bdb_new_row(&test);
    sda_bdb_set_entry("sloupec_1", "TestData123", 12, &test);
    sda_bdb_set_entry("sloupec_2", "TestData2", 14, &test);
    sda_bdb_set_entry("sloupec_3", "TestData3", 10, &test);
  }

  //todo:fix this
  //sda_bdb_list_table(&test);

  sda_bdb_select_row_id(5, &test);
  sda_bdb_set_entry("sloupec_1", "id 5 rowdata", 14, &test);

  uint8_t buff[32];
  sda_bdb_select_row(0, &test);
  uint32_t id = 0;
  while(1) {
    sda_bdb_get_entry("id", &id, sizeof(id), &test);
    sda_bdb_get_entry("sloupec_1", buff, sizeof(buff), &test);
    printf("id: %u, entry1: %s ",id, buff);
    sda_bdb_get_entry("sloupec_2", buff, sizeof(buff), &test);
    printf("entry2: %s ", buff);
    sda_bdb_get_entry("sloupec_3", buff, sizeof(buff), &test);
    printf("entry3: %s \n", buff);
    if(!sda_bdb_select_row_next(&test)) {
      break;
    }
  }

/*
  sda_bdb_new_table("table2", 5, &test);
  sda_bdb_new_table("table3", 2, &test);

  sda_bdb_new_table("table4", 6, &test);

  sda_bdb_select_table("table1", &test);
  sda_bdb_set_column(2, "id", 0, &test);
  
  sda_bdb_select_table("table1", &test);
  
  sda_bdb_new_row(&test);
  
  sda_bdb_set_entry("sloupec_2", "TestData123", 13, &test);
  sda_bdb_set_entry("sloupec_1", "karlos", 8, &test);

  printf("entry size: %u\n", sda_bdb_get_entry_size("sloupec_1", &test));

  uint8_t buff[32];

  uint32_t siz = sda_bdb_get_entry("sloupec_1", buff, sizeof(buff), &test);

  printf("entry: %s (%u)\n", buff, siz);
  
  sda_bdb_new_row(&test);
  sda_bdb_set_entry("sloupec_1", "TestData123", 12, &test);
  
  sda_bdb_new_row(&test);
  sda_bdb_set_entry("sloupec_1", "TestData123", 12, &test);
  sda_bdb_set_entry("sloupec_1", "TestData", 10, &test);
  sda_bdb_set_entry("sloupec_1", "TestData", 14, &test);
  sda_bdb_set_entry("sloupec_2", "TestData", 10, &test);
  */

  sda_bdb_list_table(&test);

  sda_bdb_close(&test);
}