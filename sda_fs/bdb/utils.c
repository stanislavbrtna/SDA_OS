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

uint32_t sda_bdb_truncate_generic(sda_bdb *db, uint32_t position, uint32_t size, uint32_t end) {
  uint8_t buffer[512];

  uint32_t file_end = end;
  
  uint32_t copy_size = sizeof(buffer);

  if(position + size + sizeof(buffer) > file_end) {
    //printf("SHORT\n");
    svp_fseek(&(db->fil), position + size);
    copy_size = file_end - position;
    svp_fread(&(db->fil), &buffer, copy_size);
    svp_fseek(&(db->fil), position);
    svp_fwrite(&(db->fil), &buffer, copy_size);
    return 1;
  }

  for(uint32_t i = 0; position + size + (i)*sizeof(buffer) < file_end; i++) {
    //printf("LONG i: %u pos: %u size: %u\n", i, position + size + i*sizeof(buffer), size);
    svp_fseek(&(db->fil), position + size + i*sizeof(buffer));
    svp_fread(&(db->fil), &buffer, sizeof(buffer));
    svp_fseek(&(db->fil), position + i*sizeof(buffer));
    svp_fwrite(&(db->fil), &buffer, sizeof(buffer));
  }

  return 1;
}


uint32_t sda_bdb_truncate(sda_bdb *db, uint32_t position, uint32_t size) {
  uint32_t file_end = svp_get_size(&(db->fil));
  
  sda_bdb_truncate_generic(db, position, size, file_end);

  svp_fseek(&(db->fil), file_end - size);
  svp_truncate(&(db->fil));
  return 1;
}


// move table content
uint32_t sda_bdb_truncate_upto(sda_bdb *db, uint32_t position, uint32_t size) {
  uint32_t file_end = db->current_table.usedup_size + db->current_table_offset;
  sda_bdb_truncate_generic(db, position, size, file_end);
  return 1;
}


uint8_t sda_bdb_check_table_space(sda_bdb *db, uint32_t size) {
  //printf("size check: size: %u table-usedup: %u (%s)\n", size, db->current_table.table_size - db->current_table.usedup_size, db->current_table.name);
  
  if(db->current_table.table_size < db->current_table.usedup_size) {
    printf("ERROR: table_size < usedup_size\n");
  }
  
  // if we neel larger space than the available
  if(size > db->current_table.table_size - db->current_table.usedup_size) {
    while(1) {
      printf("inserting free block at %x\n", db->current_table_offset + db->current_table.table_size);
      sda_bdb_insert_freeblock(db, db->current_table_offset + db->current_table.table_size);
      db->current_table.table_size += SDA_BDB_BLOCKSIZE;
      svp_fsync(&(db->fil));
      
      if(size > SDA_BDB_BLOCKSIZE) {
        size -= SDA_BDB_BLOCKSIZE;
      } else {
        break;
      }
    }
  }

  return 1;
}


uint32_t sda_bdb_insert_space_indb(sda_bdb *db, uint32_t position, uint32_t size) {
  
  sda_bdb_check_table_space(db, size);
  
  uint32_t file_end = db->current_table_offset + db->current_table.usedup_size;
  sda_bdb_insert_free_generic(db, position, size, file_end);

  return 1;
}


uint32_t sda_bdb_insert_freeblock(sda_bdb *db, uint32_t position) {
  sda_bdb_insert_free_generic(db, position, SDA_BDB_BLOCKSIZE, svp_get_size(&(db->fil)));
  return 1;
}

uint32_t sda_bdb_insert_free_generic(
    sda_bdb *db, 
    uint32_t position,
    uint32_t size, 
    uint32_t file_end
){
  uint8_t buffer[512];
  //printf("eof at: %x, pos:%x, size: %u, %u should be moved\n", file_end, position, size, file_end - position);

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
    svp_fseek(&(db->fil), position);
    return 1;
  }

  uint32_t i = 0;
  for(i = 0; (file_end - (i + 1)*sizeof(buffer) > position) && (file_end > (i + 1)*sizeof(buffer)); i++) {
    svp_fseek(&(db->fil), file_end - (i + 1)*sizeof(buffer));   
    //printf("i: %u, bs/b: %u reading: %x writing:%x \n", i, SDA_BDB_BLOCKSIZE / sizeof(buffer), file_end - (i + 1)*sizeof(buffer), file_end + size - (i + 1)*sizeof(buffer));
    svp_fread(&(db->fil), buffer, sizeof(buffer));
    svp_fseek(&(db->fil), file_end + size - (i + 1)*sizeof(buffer));
    svp_fwrite(&(db->fil), buffer, sizeof(buffer));
  }

  if((file_end - (i)*sizeof(buffer) > position)) {
    //printf("moving rest... from: %x, size: %u (end: %u, ibuf:%u, pos:%u)\n", position, file_end - (i)*sizeof(buffer) - position, file_end, (i)*sizeof(buffer), position);
    svp_fseek(&(db->fil), position);
    svp_fread(&(db->fil), buffer, file_end - (i)*sizeof(buffer) - position);
    svp_fseek(&(db->fil), position + size);
    svp_fwrite(&(db->fil), buffer, file_end - (i)*sizeof(buffer) - position);
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
  uint32_t entry_size,
  uint8_t *value,
  uint8_t partial,
  uint8_t case_sensitive,
  sda_bdb *db
){
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
