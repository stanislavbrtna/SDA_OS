/*
Copyright (c) 2018-2023 Stanislav Brtna

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

#include "svp_csv_parse.h"

uint8_t svp_csv_open(svp_csvf *fc, uint8_t * fname) {
  if (svp_fopen_rw(&(fc->fil),fname) == 0) {
    // error
    return 0;
  }
  fc->separator = SDA_SEPARATOR;
  return 1;
}

void svp_csv_set_separator(svp_csvf *fc, uint8_t separator) {
  fc->separator = separator;
}

uint8_t svp_csv_close(svp_csvf *fc) {
  return svp_fclose(&(fc->fil));
}

uint8_t seek_line_start(svp_csvf *fc) {
  uint8_t c;

  while(svp_ftell(&(fc->fil)) > 1) {
    svp_fseek(&(fc->fil), svp_ftell(&(fc->fil)) - 1);
    c = svp_fread_u8(&(fc->fil));
#ifdef CSV_DBG
    printf("%u\n", c);
#endif
    if (c == SDA_ENDLINE) {
      return 1;
    }

    svp_fseek(&(fc->fil), svp_ftell(&(fc->fil)) - 1);
  }
#ifdef CSV_DBG
  printf("seek line start return 0\n");
#endif
  return 0;
}

uint32_t seek_separator(svp_csvf *fc, uint8_t index, uint8_t *ok) {
  uint8_t c = 0;
  // assuming ok
  *ok = 1;

  //then look for separator index-1
  if (index != 0) {
    c = svp_fread_u8(&(fc->fil));
    while((c != SDA_ENDLINE) && (1 != svp_feof(&(fc->fil)))) {
      if (c == fc->separator) {
        if (index > 1) {
          index--;
        } else {
          break;
        }
      }
      c = svp_fread_u8(&(fc->fil));
    }
    if ((c == SDA_ENDLINE) || (svp_feof(&(fc->fil)))) {
      *ok = 0;
    }
  }
  return svp_ftell(&(fc->fil));
}


uint16_t svp_csv_get_cell_svs(svp_csvf *fc, uint8_t index, uint8_t * def, svsVM *s) {
  uint32_t line_start;
  uint8_t c=0;
  uint8_t ok=0;

  seek_line_start(fc);

  line_start=svp_ftell(&(fc->fil));
#ifdef CSV_DBG
  printf("svp_csv_set_cell: line start found:%u\n", line_start );
#endif
  seek_separator(fc, index, &ok);

  if (ok!=1){
    return strNew(def, s);
  }

  strNewStreamInit(s);

  c=svp_fread_u8(&(fc->fil));

  while((c != fc->separator) && (c != SDA_ENDLINE)) {
    if (strNewStreamPush(c,s)) {
      break;
    }
  }

  svp_fseek(&(fc->fil), line_start);

  return strNewStreamEnd(s);
}

uint16_t svp_csv_get_cell(svp_csvf *fc, uint8_t index, uint8_t * def, uint8_t * buff, uint16_t len) {
  uint16_t i;
  uint32_t line_start;
  uint8_t c = 0;
  uint8_t ok = 0;

  seek_line_start(fc);

  line_start = svp_ftell(&(fc->fil));
#ifdef CSV_DBG
  printf("svp_csv_set_cell: line start found:%u\n", line_start );
#endif
  seek_separator(fc, index, &ok);

  if (ok == 0) {
    i = 0;
    while(def[i] != 0) {
      if (i < len) {
        buff[i] = def[i];
      } else {
        buff[i] = 0;
        break;
      }
      i++;
    }
    buff[i] = 0;
    return 0;
  }

  c = svp_fread_u8(&(fc->fil));

  i = 0;
  while((c != fc->separator) && (c != SDA_ENDLINE) && !svp_feof(&(fc->fil))) {
    if (i < len) {
      buff[i] = c;
    } else {
      buff[i] = 0;
      break;
    }
    c = svp_fread_u8(&(fc->fil));
    i++;
  }

  buff[i] = 0;

  svp_fseek(&(fc->fil), line_start);

  return 1;
}

uint8_t svp_csv_set_cell(svp_csvf *fc, uint8_t index, uint8_t * value) {
  uint16_t val_len;
  uint16_t key_len;
  uint16_t i;
  uint32_t start_pos, fsize, x, line_start;
  uint8_t c = 0;
  //seek on start of the line
  seek_line_start(fc);

  line_start = svp_ftell(&(fc->fil));
#ifdef CSV_DBG
  printf("svp_csv_set_cell: line start found:%u\n", line_start );
#endif
  //then look for separator index-1
  if (index != 0) {
    c = svp_fread_u8(&(fc->fil));
    while((c != SDA_ENDLINE) && (1 != svp_feof(&(fc->fil)))) {
      if (c == fc->separator) {
        if (index > 1) {
          index--;
        } else {
          break;
        }
      }
      c = svp_fread_u8(&(fc->fil));
    }
    if ((c == SDA_ENDLINE) || (svp_feof(&(fc->fil)))) {
      return 0;
    }
  }

  start_pos = svp_ftell(&(fc->fil));

#ifdef CSV_DBG
  printf("svp_csv_set_cell: separator found\n");
  printf("start pos: %u\n", start_pos);
#endif
  key_len = 0;

  c = svp_fread_u8(&(fc->fil));
  while((c != SDA_ENDLINE) && (1 != svp_feof(&(fc->fil)))) {
    if (c == fc->separator) {
        break;
    }
    key_len++;
    c = svp_fread_u8(&(fc->fil));
  }
#ifdef CSV_DBG
  printf("keylen:%u\n", key_len);
#endif
  svp_fseek(&(fc->fil), start_pos);

  val_len = 0;
  while(value[val_len] != 0) {
    val_len++;
  }

  if (key_len >= val_len) {
    for(i = 0; i < val_len; i++) {
      svp_fwrite_u8(&(fc->fil),value[i]);
    }

    fsize = svp_get_size(&(fc->fil));

    for (x = start_pos + key_len; x < fsize; x++) {
      svp_fseek(&(fc->fil), x);
      c = svp_fread_u8(&(fc->fil));
      svp_fseek(&(fc->fil), x-key_len + val_len);
      svp_fwrite_u8(&(fc->fil), c);
    }
    svp_fseek(&(fc->fil), fsize-key_len + val_len);

    svp_truncate(&(fc->fil));

    svp_fseek(&(fc->fil), line_start);

  } else {

    fsize = svp_get_size(&(fc->fil));
#ifdef CSV_DBG
    printf("fsize: %u\n", fsize);
#endif

    if(fsize > 0) {
      for (x = fsize - 1; x + 1 > start_pos; x--) {
        svp_fseek(&(fc->fil), x);
        c = svp_fread_u8(&(fc->fil));

        svp_fseek(&(fc->fil), x + val_len - key_len); //tím si nejsem 100% jist
        svp_fwrite_u8(&(fc->fil), c);
      }
    }
    svp_fseek(&(fc->fil), start_pos);

   for(i = 0; i < val_len; i++) {
     svp_fwrite_u8(&(fc->fil), value[i]);
   }

    svp_fseek(&(fc->fil), line_start);
  }
  return 1;
}

void svp_csv_remove_line(svp_csvf *fc) {
  uint16_t key_len = 0;
  uint32_t fsize, x, line_start;
  uint8_t cbuff = 0;

  //seek on start of the line
  seek_line_start(fc);

  line_start = svp_ftell(&(fc->fil));
#ifdef CSV_DBG
  printf("svp_csv_set_cell: line start found:%u\n", line_start );
#endif

  while(!svp_feof(&(fc->fil))) {
    if (svp_fread_u8(&(fc->fil)) == SDA_ENDLINE) {
      break;
    } else {
      key_len++;
    }
  }

  key_len++; // counting the newline

  //printf("keylen: %s %u\n", key, keylen);

  svp_fseek(&(fc->fil), line_start);

  fsize = svp_get_size(&(fc->fil));

  for (x = line_start + key_len; x < fsize; x++) {
    svp_fseek(&(fc->fil), x);
    cbuff = svp_fread_u8(&(fc->fil));
    svp_fseek(&(fc->fil), x - key_len);
    svp_fwrite_u8(&(fc->fil), cbuff);
  }

  if (fsize > key_len) {
    svp_fseek(&(fc->fil), fsize - key_len);
  } else {
    svp_fseek(&(fc->fil), 0);
  }
  svp_truncate(&(fc->fil));

}

uint8_t svp_csv_next_line(svp_csvf *fc){
  uint8_t c;
  uint32_t pracpos;

  c = svp_fread_u8(&(fc->fil));
  while((c != SDA_ENDLINE) && (1 != svp_feof(&(fc->fil)))) {
    c = svp_fread_u8(&(fc->fil));
  }
  if ((c == SDA_ENDLINE) && (1 != svp_feof(&(fc->fil)))) {
    pracpos = svp_ftell(&(fc->fil));

    svp_fread_u8(&(fc->fil));

    if (svp_feof(&(fc->fil))){
      return 0;
    } else {
      svp_fseek(&(fc->fil),pracpos);
      return 1;
    }
  } else {
    return 0;
  }

}

void svp_csv_new(svp_csvf *fc, uint8_t count) {
  uint8_t x;
  uint32_t startpos;
  // seek end
  if (svp_get_size(&(fc->fil)) != 0) {
    svp_fseek(&(fc->fil), svp_get_size(&(fc->fil)));
    svp_fwrite_u8(&(fc->fil), SDA_ENDLINE);
  }

  startpos = svp_ftell(&(fc->fil));

  // line is ended with separator, for consitency reasons...
  for(x = 0; x < count; x++) {
    svp_fwrite_u8(&(fc->fil), fc->separator);
  }

  svp_fseek(&(fc->fil), startpos);
}

void svp_csv_rewind(svp_csvf *fc) {
  svp_fseek(&(fc->fil), 0);
}

