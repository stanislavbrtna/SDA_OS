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
#include "utils.h"

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
    8b  indexing type
---
optional indexes
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

// Internal Defines
#define DATA_START_OFFSET 4 + sizeof(uint32_t)


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
  sda_bdb_write_u32(&(db->fil), SDA_BDB_VERSION);
  
  // table data
  // table count
  sda_bdb_write_u32(&(db->fil), 0);
  // first table offset (now 0)
  sda_bdb_write_u32(&(db->fil), 0);

  db->current_table.valid = 0;
  db->valid = 1;
  db->table_count = 0;
  db->start_offset = DATA_START_OFFSET + 2*sizeof(uint32_t);
  db->last_entry_id_en = 0;
  db->column_cache_valid = 0;
  svp_fsync(&(db->fil));

  return 1;
}


uint8_t sda_bdb_open(uint8_t *fname, sda_bdb *db) {
  if(!svp_fexists(fname)) {
    printf("%s: file \"%s\" does not exists\n", __FUNCTION__, fname);
    return 0;
  }

  svp_fopen_rw(&(db->fil), fname);

  if(svp_fread_u8(&(db->fil)) != 'B'
    || svp_fread_u8(&(db->fil)) != 'D'
    || svp_fread_u8(&(db->fil)) != 'B'
    || svp_fread_u8(&(db->fil)) != '0'
  ) {
    printf("%s: file \"%s\" is not a bdb file\n", __FUNCTION__, fname);
    return 0;
  }

  if(sda_bdb_read_u32(&(db->fil)) != SDA_BDB_VERSION) {
    printf("%s: file \"%s\" was created by different version!\n", __FUNCTION__, fname);
    return 0;
  }

  svp_fseek(&(db->fil), DATA_START_OFFSET);


  db->valid = 1;
  db->table_count = sda_bdb_read_u32(&(db->fil));
  db->current_table.valid = 0;
  db->last_entry_id_en = 0;
  db->column_cache_valid = 0;
  db->start_offset = sda_bdb_read_u32(&(db->fil));
  
  return 1;
}


uint8_t sda_bdb_close(sda_bdb *db) {
  if(db->current_table.valid) {
    sda_bdb_sync_table(db);
  }
  db->valid = 0;
  svp_fclose(&(db->fil));
  return 1;
}

uint8_t sda_bdb_sync(sda_bdb *db) {
  if(!db->valid) {
    printf("%s: Db is not valid\n", __FUNCTION__);
    return 0;
  }

  if(db->current_table.valid) {
    sda_bdb_sync_table(db);
  }

  svp_fsync(&(db->fil));

  return 1;
}

// Misc internal

uint32_t sda_bdb_table_exists(uint8_t *name, sda_bdb *db) {
  // empty db
  if(db->table_count == 0) {
    return 0;
  }

  // walk db
  uint32_t offset = db->start_offset;
  sda_bdb_table t;

  svp_fseek(&(db->fil), offset);
  //printf("seek start %x\n", offset);

  for(uint32_t i = 0; i < db->table_count; i++) {
    svp_fread(&(db->fil), &t, sizeof(t));
    
    if(svp_strcmp(name, t.name)) {
      return offset;
    }
    offset += t.table_size;
    //printf("seek continue %x\n", offset);

    svp_fseek(&(db->fil), offset);
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
  uint32_t table_begin = sda_bdb_new_offset(db);

  svp_fseek(&(db->fil), table_begin);
  
  sda_strcp(SDA_BDB_TAG_TABLE, db->current_table.table_tag, SDA_BDB_TAG_SIZE);
  sda_strcp(name, db->current_table.name, SDA_BDB_NAME_LEN);
  db->current_table.auto_id = 0;
  db->current_table.auto_id_field = 0;
  db->current_table.max_id = 1;
  db->current_table.valid = 1;
  db->column_cache_valid = 0;
  db->current_table.cloumn_count = no_columns;
  db->current_table.table_size = sizeof(sda_bdb_table) + no_columns*sizeof(sda_bdb_column);
  db->current_table.usedup_size = sizeof(sda_bdb_table) + no_columns*sizeof(sda_bdb_column);
  db->current_table.row_count = 0;
  db->current_table_offset = table_begin;
  db->current_table.current_row_valid = 0;
  db->current_table.first_row_offset = sizeof(sda_bdb_table) + no_columns*sizeof(sda_bdb_column);
  db->current_table.rows_indexed = 0;
  db->current_table.row_index_dirty = 1;

  // write header
  svp_fwrite(&(db->fil), &db->current_table, sizeof(sda_bdb_table));
  
  // reserve space for columns
  for (uint32_t i = 0; i < ((uint32_t)no_columns)*sizeof(sda_bdb_column); i++) {
    svp_fwrite_u8(&(db->fil), 0);
  }
  
  // update first table
  db->table_count++;
  svp_fseek(&(db->fil), DATA_START_OFFSET);
  sda_bdb_write_u32(&(db->fil), db->table_count);
  if(db->table_count == 1) { // on table zero, but since we incremented it, tha magic val is 1
    sda_bdb_write_u32(&(db->fil), table_begin);
    db->start_offset = table_begin;
  }

  db->last_entry_id_en = 0;
  svp_fsync(&(db->fil));
  //printf("creating table %s offset: %u\n", name, table_begin);
  return 1;
}

// set column name and type
uint8_t sda_bdb_set_column(uint8_t id, uint8_t *name, uint8_t type, sda_bdb *db) {
  sda_bdb_column c;

  if(id >= db->current_table.cloumn_count) {
    printf("%s: column: \"%s\" id is greater than the column count!\n", __FUNCTION__, name);
    return 0;
  }

  sda_strcp(name, c.name, SDA_BDB_NAME_LEN);
  c.type = type;
  c.indexed = 0;
  c.index_dirty = 1;

  svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table) + sizeof(sda_bdb_column) * id);
  svp_fwrite(&(db->fil), &c, sizeof(sda_bdb_column));

  return 1;
}

// select table
uint8_t sda_bdb_select_table(uint8_t *name, sda_bdb *db) {

  // store current table, so we can validate if the table-to-be-selected exists
  if(db->current_table.valid) {
    // selecting currently selected table, we do nothing (just rewind the table)
    if(svp_strcmp(db->current_table.name, name)) {
      sda_bdb_select_row(0, db);
      return 1;
    }

    sda_bdb_sync_table(db);
    //printf("Select: prev: %s, offset:%u size:%u end: %x\n", db->current_table.name, db->current_table_offset, db->current_table.table_size, db->current_table_offset + db->current_table.table_size);
  }

  // validate table name
  if(!sda_bdb_table_exists(name, db)) {
    printf("%s: table \"%s\" does not exist\n", __FUNCTION__, name);
    return 0;
  }
  
  // walk db
  uint32_t offset = db->start_offset;
  sda_bdb_table t;

  svp_fseek(&(db->fil), offset);

  for(uint32_t i = 0; i < db->table_count; i++) {
    
    svp_fread(&(db->fil), &t, sizeof(t));
    if(svp_strcmp(name, t.name)) {
      db->current_table_offset = offset;
      break;
    }

    offset += t.table_size;
    svp_fseek(&(db->fil), offset);
  }

  svp_fseek(&(db->fil), db->current_table_offset);
  svp_fread(&(db->fil), &(db->current_table), sizeof(sda_bdb_table));
  db->current_table.valid = 1;
  db->last_entry_id_en = 0;
  db->column_cache_valid = 0;
  //printf("selected: %s, offset:%u\n", db->current_table.name, db->current_table_offset);
  // to set the current row offset correct
  sda_bdb_select_row(0, db);
  return 1;
}


// drops current table
uint8_t sda_bdb_drop_table(sda_bdb *db) {
  if(!db->current_table.valid) {
    return 0;
  }

  // drop table
  sda_bdb_truncate(db, db->current_table_offset, db->current_table.table_size);

  db->table_count--;

  // update table count
  svp_fseek(&(db->fil), DATA_START_OFFSET);
  sda_bdb_write_u32(&(db->fil), db->table_count);
  // update first?
  if(db->table_count == 0) {
    sda_bdb_write_u32(&(db->fil), 0);
    db->start_offset = 0;
  }

  db->current_table.valid = 0;
  db->current_table.current_row_valid = 0;

  return 1;
}

// drops data from current table
uint8_t sda_bdb_drop_data(sda_bdb *db) {
  if(!db->current_table.valid) {
    return 0;
  }

  // wont truncate, just remove the usedup
  db->current_table.max_id = 1;
  db->current_table.valid  = 1;
  db->column_cache_valid   = 0;
  db->current_table.usedup_size = sizeof(sda_bdb_table) + db->current_table.cloumn_count*sizeof(sda_bdb_column);
  db->current_table.first_row_offset = db->current_table.usedup_size;
  db->current_table.row_count   = 0;
  db->current_table.current_row_valid = 0;

  sda_bdb_set_all_indexes_dirty(db);

  // Note: Index will be overwritten

  sda_bdb_sync(db);

  return 1;
}

// new row -> creates new row and sets it as current 
uint32_t sda_bdb_new_row(sda_bdb *db) {
  sda_bdb_row row;
  
  row.size = db->current_table.cloumn_count*sizeof(sda_bdb_entry);
  sda_strcp(SDA_BDB_TAG_ROW, row.row_tag, SDA_BDB_TAG_SIZE);

  uint32_t offset;
  
  //printf("creating new row with %u columns\n", db->current_table.cloumn_count);
  //printf("old offset behind: %x\n", db->current_table_offset + db->current_table.table_size);

  // get last row
  offset = db->current_table_offset + db->current_table.usedup_size;
  
  sda_bdb_check_table_space(db, sizeof(sda_bdb_row) + row.size);

  svp_fseek(&(db->fil), offset);

  // Write row header
  svp_fwrite(
    &(db->fil), 
    &row,
    sizeof(sda_bdb_row)
  );
  
  // write empty entries
  sda_bdb_entry e;
  for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
    e.entry_column = i;
    e.entry_size = 0;
    svp_fwrite(
      &(db->fil), 
      &e,
      sizeof(e)
    );
  }

  // Update table size
  //printf("table size: %u row_size: %u\n", db->current_table.table_size, sizeof(sda_bdb_row) + siz);

  db->current_table.usedup_size += sizeof(sda_bdb_row) + row.size;
  db->current_table.row_count++;
  db->current_table.current_row_offset = offset;
  db->current_table.current_row_valid = 1;
  // increment autoId
  if(db->current_table.auto_id) {
    //printf("auto id increment: %u\n", db->current_table.max_id);
    sda_bdb_set_entry_id(db->current_table.auto_id_field, &(db->current_table.max_id), sizeof(uint32_t), db);
    db->current_table.max_id++;
  }

  sda_bdb_set_all_indexes_dirty(db);

  //printf("table size after auto id: %u\n", db->current_table.table_size);
  //printf("new offset behind: %x\n", db->current_table_offset + db->current_table.table_size);

  return 1;
}

// Drops current row
uint8_t sda_bdb_drop_row(sda_bdb *db) {
  sda_bdb_row row; 
  
  if(!db->current_table.current_row_valid) {
    printf("%s: selected row is not valid!\n", __FUNCTION__);
    return 0;
  }

  // get row size
  svp_fseek(&(db->fil), db->current_table.current_row_offset);
  svp_fread(&(db->fil), &row, sizeof(sda_bdb_row));

  // truncate file
  sda_bdb_truncate_upto(db, db->current_table.current_row_offset, row.size + sizeof(sda_bdb_row));

  // set db data size
  db->current_table.row_count--;
  db->current_table.usedup_size -= row.size + sizeof(sda_bdb_row);

  if (db->current_table.current_row_offset >= db->current_table_offset + db->current_table.usedup_size) {
    db->current_table.current_row_valid = 0;
  }
  sda_bdb_set_all_indexes_dirty(db);

  sda_bdb_sync(db);
  return 1;
}

uint8_t sda_bdb_gc_type;
uint8_t sda_bdb_gc_indexed;

uint8_t sda_bdb_get_column_id(uint8_t *column_name, sda_bdb *db) {
  sda_bdb_column c;

  if(db->column_cache_valid) {
    if(svp_strcmp(db->cached_column, column_name)){
      //printf("col_id loaded from cache\n");
      sda_bdb_gc_type    = db->cached_column_type;
      sda_bdb_gc_indexed = db->cached_column_indexed; 
      return db->cached_column_id + 1;
    }
  }

  svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table));
  
  for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
    svp_fread(&(db->fil), &c, sizeof(sda_bdb_column));
    if(svp_strcmp(c.name, column_name)) {
      sda_bdb_gc_type = c.type;
      sda_bdb_gc_indexed = c.indexed;
      if (!db->column_cache_valid) {
        db->column_cache_valid = 1;
        sda_strcp(c.name, db->cached_column, SDA_BDB_NAME_LEN);
        db->cached_column_id = i;
        db->cached_column_type = c.type;
        db->cached_column_indexed = c.indexed;
      }
      
      return i + 1;
    }
  }
  sda_bdb_gc_type   = 0;
  sda_bdb_gc_indexed = 0;

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
  db->current_table.max_id = 1;
  sda_bdb_sync_table(db);
  return 1;
}

// Update

// fill entry data
uint8_t sda_bdb_set_entry_id(uint8_t id, void* data, uint32_t size, sda_bdb *db) {
  // find if given row entry exists
  if(!db->current_table.current_row_valid) {
    printf("%s: Row not valid!\n", __FUNCTION__);
    return 0;
  }

  sda_bdb_set_index_dirty(id, 1, db);

  // seek on row start
  svp_fseek(&(db->fil), db->current_table.current_row_offset);
  sda_bdb_row row;
  svp_fread(&(db->fil), &row, sizeof(sda_bdb_row));
  
  sda_bdb_entry e;
  
  // if row is not empty, then walk entries
  uint32_t offset = db->current_table.current_row_offset + sizeof(sda_bdb_row);

  if(row.size != 0) {
    for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
      svp_fread(&(db->fil), &e, sizeof(sda_bdb_entry));
      //printf("setEntry offse: %x, i: %u id: %u size: %u\n", offset, i, e.entry_column, e.entry_size);
      if(e.entry_column == id) {
        // compare sizes
        if(size == e.entry_size) {
          svp_fwrite(&(db->fil), data, size);
          return 1;
        }

        if(size > e.entry_size) {
          uint32_t entry_pos = svp_ftell(&(db->fil));
          //printf("size new: %u, size old: %u, diff: %u\n", size, e.entry_size, size - e.entry_size);
          sda_bdb_insert_space_indb(db, entry_pos, size - e.entry_size);
          svp_fseek(&(db->fil), entry_pos);
          svp_fwrite(&(db->fil), data, size);
          row.size += size - e.entry_size;
          db->current_table.usedup_size += size - e.entry_size;
        } else if(size < e.entry_size) {
          uint32_t pos = svp_ftell(&(db->fil));
          sda_bdb_truncate_upto(db, pos, e.entry_size - size);
          svp_fseek(&(db->fil), pos);    
          svp_fwrite(&(db->fil), data, size);
          row.size -= e.entry_size - size;
          db->current_table.usedup_size -= e.entry_size - size;
        }

        // update entry size
        e.entry_size = size;
        svp_fseek(&(db->fil), offset);
        svp_fwrite(&(db->fil), &e, sizeof(sda_bdb_entry));
        //printf("updating %u total size: %u\n", id, size + sizeof(sda_bdb_entry));

        // update row header
        svp_fseek(&(db->fil), db->current_table.current_row_offset);
        svp_fwrite(&(db->fil), &row, sizeof(sda_bdb_row));
        return 1;
      }

      offset += e.entry_size + sizeof(sda_bdb_entry);

      if(offset >= db->current_table.current_row_offset + row.size + sizeof(sda_bdb_row)) {
        break;
      }
      svp_fseek(&(db->fil), offset);
    }
  }
  
  //TODO: fix this... it needs to prealocate the entry size in the file... 
  printf("Warn: creating new row entry, this should not happen!\n");
  // if not: create and write
  e.entry_column = id;
  e.entry_size = size;
  
  sda_bdb_check_table_space(db, size + sizeof(sda_bdb_entry));

  // write header
  sda_bdb_write_data(
    db, 
    db->current_table.current_row_offset + sizeof(sda_bdb_row),
    &e,
    sizeof(sda_bdb_entry)
  );

  // write data
  sda_bdb_write_data(
    db, 
    db->current_table.current_row_offset + sizeof(sda_bdb_row) + sizeof(sda_bdb_entry),
    data,
    size
  );
  row.size += size + sizeof(sda_bdb_entry);

  // update row header
  svp_fseek(&(db->fil), db->current_table.current_row_offset);
  svp_fwrite(&(db->fil), &row, sizeof(sda_bdb_row));

  // sync db
  db->current_table.usedup_size += size + sizeof(sda_bdb_entry);

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
  sda_bdb_row row;
  svp_fread(&(db->fil), &row, sizeof(sda_bdb_row));

  if(row.size == 0) {
    printf("%s: Row is empty!\n", __FUNCTION__);
    return 0;
  }
  
  sda_bdb_entry e;
  
  // if row is not empty, then walk entries
  uint32_t offset = db->current_table.current_row_offset + sizeof(sda_bdb_row);

  if(row.size != 0) {
    for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
      svp_fread(&(db->fil), &e, sizeof(sda_bdb_entry));
      if(e.entry_column == id) {
        return e.entry_size;
      }
      offset += e.entry_size + sizeof(sda_bdb_entry);

      if(offset >= db->current_table.current_row_offset + row.size + sizeof(sda_bdb_row)) {
        break;
      }
      
      svp_fseek(&(db->fil), offset);
    }
  }

  return 0;
}

// returns entry size (bytes read)
uint32_t sda_bdb_get_entry_id(uint8_t id, void* buffer, uint32_t buff_size, sda_bdb *db) {
  if(!db->current_table.current_row_valid) {
    return 0;
  }

  svp_fseek(&(db->fil), db->current_table.current_row_offset);
  sda_bdb_row row;
  svp_fread(&(db->fil), &row, sizeof(sda_bdb_row));

  if(row.size == 0) {
    printf("%s: Row is empty!\n", __FUNCTION__);
    return 0;
  }
  
  sda_bdb_entry e;
  
  // if row is not empty, then walk entries
  uint32_t offset = db->current_table.current_row_offset + sizeof(sda_bdb_row);

  if(row.size != 0) {
    for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
      svp_fread(&(db->fil), &e, sizeof(sda_bdb_entry));
      if(e.entry_column == id) {
        if(e.entry_size > buff_size) {
          printf("%s: column \"%u\" is larger than the given buffer\n", __FUNCTION__, id);
          return 0;
        }
        
        svp_fread(&(db->fil), buffer, e.entry_size);

        return e.entry_size;
      }

      offset += e.entry_size + sizeof(sda_bdb_entry);

      if(offset >= db->current_table.current_row_offset + row.size + sizeof(sda_bdb_row)) {
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
    printf("%s: column id \"%u\" does not exist\n", __FUNCTION__, id);
    return 0;
  }
  id--;

  return sda_bdb_get_entry_id(id, buffer, buff_size, db);
}


// misc
// gets row count for the selected table
uint32_t sda_bdb_get_row_count(sda_bdb *db) {
  if(db->valid) {
    return db->current_table.row_count;
  }
  return 0;
}


// TODO: insert column
// TODO: drop column
// TODO: column range (float/signed/unsigned)


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
    sda_bdb_row row;
    svp_fread(&(db->fil), &row, sizeof(sda_bdb_row));
    printf("Row %u, (size:%u):\n", i, row.size);
    
    uint32_t entry_offset = offset + sizeof(sda_bdb_row);
    for(uint8_t id = 0; id < db->current_table.cloumn_count; id++) {
      sda_bdb_entry e;
      svp_fread(&(db->fil), &e, sizeof(e));
      printf("  Entry: id:%u, size:%u (%u total)\n", e.entry_column, e.entry_size, e.entry_size + sizeof(sda_bdb_entry));
      entry_offset += e.entry_size + sizeof(sda_bdb_entry);
      svp_fseek(&(db->fil), entry_offset);
    }

    offset += row.size + sizeof(sda_bdb_row);
    svp_fseek(&(db->fil), offset);
  } 
}