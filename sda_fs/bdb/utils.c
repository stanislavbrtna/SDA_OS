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

#include "utils.h"

uint32_t sda_bdb_read_u32(svp_file *f) {
  uint32_t val = 0;
  svp_fread(f, &val, sizeof(uint32_t));
  return val;
}

void sda_bdb_write_u32(svp_file *f, uint32_t val) {
  svp_fwrite(f, &val, sizeof(uint32_t));
}


uint32_t sda_bdb_new_offset(sda_bdb *db) {
  // empty db
  if(db->table_count == 0) {
    return db->start_offset;
  }

  // walk db
  uint32_t offset = db->start_offset;
  sda_bdb_table t;

  svp_fseek(&(db->fil), db->start_offset);

  for(uint32_t i = 0; i < db->table_count; i++) {
    svp_fread(&(db->fil), &t, sizeof(t));
    offset += t.table_size;
    
    svp_fseek(&(db->fil), offset);
  }

  return offset;
}


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
  return 1;
}


// move table content
uint32_t sda_bdb_truncate_upto(sda_bdb *db, uint32_t position, uint32_t size) {
  uint32_t file_end = db->current_table.usedup_size + db->current_table_offset;
  
  for(uint32_t i = position; i + size < file_end; i++) {
    svp_fseek(&(db->fil), i + size);
    uint8_t c = svp_fread_u8(&(db->fil));
    svp_fseek(&(db->fil), i);
    svp_fwrite_u8(&(db->fil), c);
  }
  return 1;
}


uint32_t sda_bdb_insert_space(sda_bdb *db, uint32_t position, uint32_t size) {
  uint32_t file_end = svp_get_size(&(db->fil));

  svp_fseek(&(db->fil), svp_get_size(&(db->fil)));

  for(uint32_t i = 0; i <= file_end - position; i++) {
    svp_fseek(&(db->fil), file_end - i);
    uint8_t c = svp_fread_u8(&(db->fil));
    svp_fseek(&(db->fil), file_end + size - i);
    svp_fwrite_u8(&(db->fil), c);
  }
  return 1;
}


uint8_t sda_bdb_check_table_space(sda_bdb *db, uint32_t size) {
  //printf("size check: size: %u table-usedup: %u\n", size, db->current_table.table_size - db->current_table.usedup_size);
  //TODO: why this fixes it?
  if(size > db->current_table.table_size - db->current_table.usedup_size) {
    //printf("inserting free block at %x\n", db->current_table_offset + db->current_table.table_size);
    //sda_bdb_insert_freeblock(db, db->current_table_offset + db->current_table.table_size);
    sda_bdb_insert_freeblock(db, db->current_table_offset + db->current_table.table_size);
    db->current_table.table_size += SDA_BDB_BLOCKSIZE;
    svp_fsync(&(db->fil));
  }
  return 1;
}


uint32_t sda_bdb_insert_space_indb(sda_bdb *db, uint32_t position, uint32_t size) {
  
  sda_bdb_check_table_space(db, size);
  
  uint32_t file_end = db->current_table_offset + db->current_table.usedup_size;
  svp_fseek(&(db->fil), svp_get_size(&(db->fil)));

  for(uint32_t i = 0; i <= file_end - position; i++) {
    svp_fseek(&(db->fil), file_end - i);
    uint8_t c = svp_fread_u8(&(db->fil));
    svp_fseek(&(db->fil), file_end + size - i);
    svp_fwrite_u8(&(db->fil), c);
  }
  return 1;
}


uint32_t sda_bdb_insert_freeblock(sda_bdb *db, uint32_t position) {
  uint32_t file_end = svp_get_size(&(db->fil));
  uint8_t buffer[512];

  uint32_t size = SDA_BDB_BLOCKSIZE;
  // TODO: block transfer
  //printf("eof at: %u, %u should be moved\n", file_end, file_end - position);

  if(file_end <= position) {
    //printf("writing at the eof\n");
    svp_fseek(&(db->fil), position + size);
    svp_fwrite_u8(&(db->fil), 0); // write something
    svp_fseek(&(db->fil), position);
    return 1;
  }

  if(file_end - position < sizeof(buffer)) {
    //printf("moving smaller than buffer \n");
    svp_fseek(&(db->fil), position);
    svp_fread(&(db->fil), buffer, file_end - position);
    svp_fseek(&(db->fil), position + size);
    svp_fwrite(&(db->fil), buffer, file_end - position);
    return 1;
  }

  for(uint32_t i = 0; i*sizeof(buffer) < file_end - position; i++) {
    
    uint32_t read_size = 0;
    svp_fseek(&(db->fil), file_end - (i + 1)*sizeof(buffer));

    if (sizeof(buffer) > file_end) {
      read_size = file_end;
    } else {
      read_size = sizeof(buffer);
    }

    //printf("i: %u, bs/b: %u reading: %u writing:%u \n", i, SDA_BDB_BLOCKSIZE / sizeof(buffer), file_end - (i + 1)*sizeof(buffer), file_end + size - (i + 1)*sizeof(buffer));

    svp_fread(&(db->fil), buffer, read_size);

    svp_fseek(&(db->fil), file_end + size - (i + 1)*sizeof(buffer));

    svp_fwrite(&(db->fil), buffer, read_size);

    //if (i > 100) break; //TODO:remove this
  }

  svp_fseek(&(db->fil), position);
  return 1;
}


uint32_t sda_bdb_write_data(sda_bdb *db, uint32_t position, void* data, uint32_t size) {
  svp_fseek(&(db->fil), position);
  svp_fwrite(&(db->fil), data, size);

  return 0;
}


void sda_bdb_sync_table(sda_bdb *db) {
  svp_fseek(&(db->fil), db->current_table_offset);
  svp_fwrite(&(db->fil), &(db->current_table), sizeof(sda_bdb_table));
}

// internal
uint8_t sda_bdb_entry_contains(
    uint32_t entry_size, uint8_t *value, uint8_t partial, uint8_t case_sensitive, sda_bdb *db) {
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
