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
#include "selects.h"

uint8_t force_sequential_read;

uint8_t sda_bdb_select_row(uint32_t n, sda_bdb *db) {
  uint32_t offset = db->current_table_offset + db->current_table.first_row_offset;
  
  svp_fseek(&(db->fil), offset);

  if(n == 0 && db->current_table.row_count > 0) {
    db->current_table.current_row_offset = offset;
    db->current_table.current_row_valid  = 1;
    return 1;
  }

  //printf("selecting, dirty? %u\n", db->current_table.row_index_dirty);

  if(db->current_table.rows_indexed && !db->current_table.row_index_dirty) {
    uint32_t ind_offset = sda_bdb_get_row_index(n, db);
    if(ind_offset) {
      db->current_table.current_row_valid  = 1;
      db->current_table.current_row_offset = db->current_table_offset + db->current_table.first_row_offset + ind_offset;
      return 1;
    }
  }

  for(uint32_t i = 0; i < db->current_table.row_count; i++) {
    sda_bdb_row row;
    svp_fread(&(db->fil), &row, sizeof(sda_bdb_row));

    if(i == n) {
      db->current_table.current_row_offset = offset;
      db->current_table.current_row_valid  = 1;
      return 1;
    }

    offset += row.size + sizeof(sda_bdb_row);

    if(offset >= db->current_table_offset + db->current_table.usedup_size) {
      break;
    }
    svp_fseek(&(db->fil), offset);
  }

  db->current_table.current_row_valid = 0;
  db->last_entry_id_en = 0;
  return 0;
}

// Select next
uint8_t sda_bdb_select_row_next(sda_bdb *db) {
  uint32_t offset = db->current_table.current_row_offset;

  if(!db->current_table.current_row_valid) {
    printf("%s: Warn: Current row invalid, nothing to do...!\n", __FUNCTION__);
    return 0;
  }
  
  if(offset >= db->current_table_offset + db->current_table.usedup_size) {
    db->current_table.current_row_valid = 0;
    return 0;
  }
  
  svp_fseek(&(db->fil), offset);
  sda_bdb_row row;

  svp_fread(&(db->fil), &row, sizeof(sda_bdb_row));
  
  offset += row.size + sizeof(sda_bdb_row);
  
  if(offset < db->current_table_offset + db->current_table.usedup_size) {
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
    printf("%s: current row is invalid!\n", __FUNCTION__);
    return 0;
  }

  db->last_entry_id_en = 0;

  if (!force_sequential_read) {
    uint32_t ind_offset = sda_bdb_get_index(val, col_id, db);
    if(ind_offset) {
      db->current_table.current_row_valid  = 1;
      db->current_table.current_row_offset = db->current_table_offset + db->current_table.first_row_offset + ind_offset;
      return 1;
    }
  }
 
  if(offset >= db->current_table_offset + db->current_table.usedup_size) {
    db->current_table.current_row_valid = 0;
    return 0;
  }

  svp_fseek(&(db->fil), offset);

  for(uint32_t i = 0; i < db->current_table.row_count; i++) {
    sda_bdb_row row;
    svp_fread(&(db->fil), &row, sizeof(sda_bdb_row));

    uint32_t entry_offset = offset + sizeof(sda_bdb_row);
    for(uint8_t id = 0; id < db->current_table.cloumn_count; id++) {
      sda_bdb_entry e;
      svp_fread(&(db->fil), &e, sizeof(e));
      if(e.entry_column == col_id) {
        // read, compare
        uint32_t entry_id = 0;
        svp_fread(&(db->fil), &entry_id, sizeof(uint32_t));
        if(val == entry_id) {
          db->current_table.current_row_valid  = 1;
          db->current_table.current_row_offset = offset;
          return 1;
        }
      }
      
      entry_offset += e.entry_size + sizeof(sda_bdb_entry);
      svp_fseek(&(db->fil), entry_offset);
    }

    offset += row.size + sizeof(sda_bdb_row);

    if(offset >= db->current_table_offset + db->current_table.usedup_size) {
      break;
    }

    svp_fseek(&(db->fil), offset);
  }

  db->current_table.current_row_valid = 0;
  return 0;
}


uint8_t sda_bdb_select_row_id(uint32_t id, sda_bdb *db) {
  uint8_t val = 0;

  if(!db->current_table.auto_id) {
    printf("%s: auto ID not enabled on table \"%s\"\n", __FUNCTION__, db->current_table.name);
    return 0;
  }

  // id is before cursor, we must read the table from the start
  if(db->last_entry_id_en == 0 || db->last_entry_id > id || db->current_table.current_row_valid == 0) {
    sda_bdb_select_row(0, db);
    force_sequential_read = 0;
    val = sda_bdb_select_row_num_generic(db->current_table.auto_id_field, id, db);
  } else {
    if (db->last_entry_id < id + 30) {
      force_sequential_read = 1;
    }
    val = sda_bdb_select_row_num_generic(db->current_table.auto_id_field, id, db);
  }

  force_sequential_read = 0;
  if(val) {
    db->last_entry_id = id;
    db->last_entry_id_en = 1;
  } else {
    db->last_entry_id_en = 0;
  }

  return val;
}

extern uint8_t sda_bdb_gc_type;
extern uint8_t sda_bdb_gc_indexed;

// finds next matching row
uint8_t sda_bdb_next_row_match_num(uint8_t *column_name, uint32_t val, sda_bdb *db) {

  uint8_t col_id = sda_bdb_get_column_id(column_name, db); 
  
  db->last_entry_id_en = 0;

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

Note:
if you are droping row in a search loop, next row after the one dropped is selected.
TODO: this should perhaps be fixed some day

loop(match) {
  drop_row()
  // next row is now redundant
}

*/


// select row based on its string value
uint8_t sda_bdb_select_row_str(
  uint8_t* column_name, 
  uint8_t *str, 
  uint8_t full_string, 
  uint8_t case_sensitive, 
  sda_bdb *db
) {

  uint8_t col_id = sda_bdb_get_column_id(column_name, db); 

  if(!col_id) {
    printf("%s: no column named \"%s\"\n", __FUNCTION__, column_name);
    return 0;
  }

  col_id--;

  if(sda_bdb_gc_type != SDA_BDB_TYPE_STR) {
    printf("%s: column \"%s\" is not type STR\n", __FUNCTION__, column_name);
    return 0;
  }

  if(!db->current_table.current_row_valid) {
    printf("%s: current row is invalid!\n", __FUNCTION__);
    return 0;
  }

  if(full_string && case_sensitive) {
    uint32_t ind_offset = sda_bdb_get_index(crc32b_len(str, sda_strlen(str) + 1), col_id, db);
    if(ind_offset) {
      db->current_table.current_row_valid  = 1;
      db->current_table.current_row_offset = db->current_table_offset + db->current_table.first_row_offset + ind_offset;
      return 1;
    }
  }
  
  db->last_entry_id_en = 0;

  uint32_t offset = db->current_table.current_row_offset;
  svp_fseek(&(db->fil), offset);

  for(uint32_t i = 0; i < db->current_table.row_count; i++) {
    sda_bdb_row row;
    svp_fread(&(db->fil), &row, sizeof(sda_bdb_row));

    uint32_t entry_offset = offset + sizeof(sda_bdb_row);
    for(uint8_t id = 0; id < db->current_table.cloumn_count; id++) {
      sda_bdb_entry e;
      svp_fread(&(db->fil), &e, sizeof(e));
      if(e.entry_column == col_id) {
        // read, compare
        if(sda_bdb_entry_contains(e.entry_size, str, full_string, case_sensitive, db)) {
          db->current_table.current_row_offset = offset;
          return 1;
        }
      }
      entry_offset += e.entry_size + sizeof(sda_bdb_entry);
      svp_fseek(&(db->fil), entry_offset);
    }

    offset += row.size + sizeof(sda_bdb_row);
    
    if(offset >= db->current_table_offset + db->current_table.usedup_size) {
      break;
    }

    svp_fseek(&(db->fil), offset);
  } 
  return 0;
}