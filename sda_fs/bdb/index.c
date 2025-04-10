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

#include "index.h"
#include "utils.h"

uint8_t sda_bdb_set_column_indexing(uint8_t* col_name, uint8_t indexing, sda_bdb *db) {
  uint8_t id = sda_bdb_get_column_id(col_name, db);
  id--;

  return sda_bdb_set_column_indexing_id(id, indexing, db);
}

uint8_t sda_bdb_set_column_indexing_id(uint8_t id, uint8_t indexing, sda_bdb *db) {
  sda_bdb_column c;
  uint32_t offset;

  if(indexing != SDA_BDB_INDEXING_ROWS) {
    if(id >= db->current_table.cloumn_count) {
      printf("%s: column id is greater than the column count!\n", __FUNCTION__);
      return 0;
    }

    // mark as indexed
    svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table) + sizeof(sda_bdb_column) * id);
    svp_fread(&(db->fil), &c, sizeof(c));
    c.indexed = 1;
    c.index_dirty = 1;
    svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table) + sizeof(sda_bdb_column) * id);
    svp_fwrite(&(db->fil), &c, sizeof(sda_bdb_column));
  }

  // write index header
  
  // make some space
  offset = db->current_table_offset + sizeof(sda_bdb_table) + db->current_table.cloumn_count*sizeof(sda_bdb_column);
  sda_bdb_insert_space_indb(db, offset, sizeof(sda_bdb_index_header));
  svp_fseek(&(db->fil), offset);
  
  sda_bdb_index_header ind;

  ind.column = id;
  ind.size = 0;
  ind.indexing_type = indexing;
  sda_strcp(SDA_BDB_TAG_INDEX, ind.index_tag, SDA_BDB_TAG_SIZE);
  
  // write header
  svp_fwrite(&(db->fil), &ind, sizeof(ind));

  // mark offset
  db->current_table.first_row_offset += sizeof(sda_bdb_index_header);
  db->current_table.usedup_size += sizeof(sda_bdb_index_header);

  sda_bdb_sync_table(db);

  return 1;      
}

uint8_t sda_bdb_rebuild_index(uint8_t *column_name, sda_bdb *db) {
  uint8_t id = sda_bdb_get_column_id(column_name, db);
  id--;

  return sda_bdb_rebuild_index_id(id, db);
}

static uint32_t allocate_index_space(sda_bdb_index_header index_header, uint32_t header_offset, sda_bdb *db) {
  // allocate space
  uint32_t index_size = db->current_table.row_count*sizeof(sda_bdb_index);
  if(index_header.size < index_size) {
    //printf("rebuilding? index size: %u\n", index_size - index_header.size);
    sda_bdb_insert_space_indb(db, header_offset, index_size - index_header.size);

    db->current_table.first_row_offset += index_size - index_header.size;
    db->current_table.usedup_size += index_size - index_header.size;
  
    index_header.size = index_size;
    svp_fseek(&(db->fil), header_offset);
    svp_fwrite(&(db->fil), &index_header, sizeof(index_header));
  }
  return index_size;
}

uint8_t sda_bdb_rebuild_row_index(sda_bdb *db) {
  uint32_t offset = db->current_table_offset + sizeof(sda_bdb_table) + db->current_table.cloumn_count*sizeof(sda_bdb_column);
  sda_bdb_index_header index_header;

  //printf("rebuild row index called o:%u, s:%u\n", offset, (db->current_table_offset + db->current_table.first_row_offset));

  svp_fseek(&(db->fil), offset);
  // find index
  while(offset < (db->current_table_offset + db->current_table.first_row_offset)) {
    svp_fread(&(db->fil), &index_header, sizeof(index_header));
    //printf("found index type: %u\n", index_header.indexing_type);
    if(index_header.indexing_type == SDA_BDB_INDEXING_ROWS) {
      //printf("found row index\n");
      if (db->current_table.rows_indexed && db->current_table.row_index_dirty) {
        //printf("building row index (table: %s)...\n", db->current_table.name);
        // Rows indexing
        uint32_t index_size = db->current_table.row_count*sizeof(sda_bdb_index);
        if(index_header.size < index_size) {
          index_header.size = allocate_index_space(index_header, offset, db);
        }

        // walk rows, store indexes
        sda_bdb_select_row(0, db);
        uint32_t index_offset = offset + sizeof(index_header);
        for(uint32_t i = 0; i < db->current_table.row_count; i++) {
          sda_bdb_index index;
          index.value = i;
          index.row_offset = db->current_table.current_row_offset - (db->current_table_offset + db->current_table.first_row_offset);

          // write index
          svp_fseek(&(db->fil), index_offset);
          svp_fwrite(&(db->fil), &index, sizeof(index));
          index_offset += sizeof(index);
          sda_bdb_select_row_next(db);
        }
        db->current_table.row_index_dirty = 0;
      }
    }
    offset += index_header.size + sizeof(index_header);
  }
}


uint8_t sda_bdb_rebuild_index_id(uint8_t column_id, sda_bdb *db) {
  uint32_t offset = db->current_table_offset + sizeof(sda_bdb_table) + db->current_table.cloumn_count*sizeof(sda_bdb_column);
  sda_bdb_index_header index_header;

  svp_fseek(&(db->fil), offset);
  // find index
  while(offset < (db->current_table_offset + db->current_table.first_row_offset)) {
    svp_fread(&(db->fil), &index_header, sizeof(index_header));
    //printf("found index: col:%u type:%u\n", index_header.column, index_header.indexing_type);    
    if(index_header.column == column_id) {
      if(index_header.indexing_type == SDA_BDB_INDEXING_VALUE) {
        // allocate space
        uint32_t index_size = db->current_table.row_count*sizeof(sda_bdb_index);
        if(index_header.size < index_size) {
          index_header.size = allocate_index_space(index_header, offset, db);
        }

        // walk rows, store indexes
        sda_bdb_select_row(0, db);
        uint32_t index_offset = offset + sizeof(index_header);
        for(uint32_t i = 0; i < db->current_table.row_count; i++) {
            uint32_t val;
            sda_bdb_index index;
            sda_bdb_get_entry_id(column_id, &val, sizeof(val), db);
            index.value = val;
            index.row_offset = db->current_table.current_row_offset - (db->current_table_offset + db->current_table.first_row_offset);

            // write index
            svp_fseek(&(db->fil), index_offset);
            svp_fwrite(&(db->fil), &index, sizeof(index));
            index_offset += sizeof(index);
            sda_bdb_select_row_next(db);
        }  
      }
    }
    offset += index_header.size + sizeof(index_header);
  }
  sda_bdb_set_index_dirty(column_id, 0, db);
}

void sda_bdb_set_index_dirty(uint8_t column_id, uint8_t val, sda_bdb *db) {
  sda_bdb_column c;

  svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table) + sizeof(sda_bdb_column) * column_id);
  svp_fread(&(db->fil), &c, sizeof(c));
  c.index_dirty = val;
  svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table) + sizeof(sda_bdb_column) * column_id);
  svp_fwrite(&(db->fil), &c, sizeof(c));
}

void sda_bdb_set_all_indexes_dirty(sda_bdb *db) {
  for(uint8_t i = 0; i < db->current_table.cloumn_count; i++) {
    sda_bdb_set_index_dirty(i, 1, db);
  }
  
  //if(db->current_table.row_index_dirty == 0)
  //  printf("Setting index dirty, table: %s\n", db->current_table.name);

  db->current_table.row_index_dirty = 1;
}

uint32_t sda_bdb_get_index(uint32_t val, uint8_t column_id, sda_bdb *db) {
  uint32_t offset = db->current_table_offset + sizeof(sda_bdb_table) + db->current_table.cloumn_count*sizeof(sda_bdb_column);
  sda_bdb_index_header index_header;

  sda_bdb_column c;

  svp_fseek(&(db->fil), db->current_table_offset + sizeof(sda_bdb_table) + sizeof(sda_bdb_column) * column_id);
  svp_fread(&(db->fil), &c, sizeof(c));

  if(c.index_dirty) {
    return 0;
  }

  svp_fseek(&(db->fil), offset);
  // find index
  while(offset < (db->current_table_offset + db->current_table.first_row_offset)) {
    svp_fread(&(db->fil), &index_header, sizeof(index_header));
    if(index_header.column == column_id && index_header.indexing_type != SDA_BDB_INDEXING_ROWS) {
      // dirty, return
      for(uint32_t i = 0; i < db->current_table.row_count; i++) {
        if(index_header.indexing_type == SDA_BDB_INDEXING_VALUE) {
          sda_bdb_index index;
          // read index
          svp_fread(&(db->fil), &index, sizeof(index));
          if(index.value == val) {
            //printf("returning indexed offset! (column index)\n");
            return index.row_offset;
          }
        }  
      }
    }
    offset += index_header.size + sizeof(index_header);
  }
  return 0;
}

uint32_t sda_bdb_get_row_index(uint32_t val, sda_bdb *db) {
  uint32_t offset = db->current_table_offset + sizeof(sda_bdb_table) + db->current_table.cloumn_count*sizeof(sda_bdb_column);
  sda_bdb_index_header index_header;

  svp_fseek(&(db->fil), offset);
  // find index
  while(offset < (db->current_table_offset + db->current_table.first_row_offset)) {
    svp_fread(&(db->fil), &index_header, sizeof(index_header));
    if(index_header.indexing_type == SDA_BDB_INDEXING_ROWS) {
      for(uint32_t i = 0; i < db->current_table.row_count; i++) {
        sda_bdb_index index;
        // read index
        svp_fread(&(db->fil), &index, sizeof(index));
        if(index.value == val) {
          //printf("returning indexed offset! (row index)\n");
          return index.row_offset;
        }
      }
    }
    offset += index_header.size + sizeof(index_header);
  }

  return 0;
}

uint8_t sda_bdb_enable_row_index(uint8_t val, sda_bdb *db) {
  db->current_table.rows_indexed = val;
  db->current_table.row_index_dirty = 1;
  if(val == 1) {
    sda_bdb_set_column_indexing_id(0, SDA_BDB_INDEXING_ROWS, db);
  }
  return 1;
}
