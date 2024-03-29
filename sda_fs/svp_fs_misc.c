/*
Copyright (c) 2018 Stanislav Brtna

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

#include "svp_fs_misc.h"

uint16_t svp_load_file_to_svs(uint8_t * fname, svsVM *s) {
  svp_file fil;
  uint8_t c = 0;

  if (svp_fexists(fname)) {
    strNewStreamInit(s);
    svp_fopen_rw(&fil, fname);
    while (!svp_feof(&fil)) {
      c = svp_fread_u8(&fil);
      if (strNewStreamPush(c, s)) {
        break;
      }
    }
    svp_fclose(&fil);
    return strNewStreamEnd(s);
  }
  return strNew((uint8_t *)"FILE INPUT ERROR", s);
}

uint16_t svp_store_svs_to_file(uint8_t * fname, uint8_t* str, svsVM *s) {
  svp_file fil;
  uint16_t i = 0;
  (void) (s);
  svp_fopen_rw(&fil, fname);
  while (str[i] != 0) {
    svp_fwrite_u8(&fil, str[i]);
    i++;
  }
  svp_truncate(&fil);
  svp_fclose(&fil);
  return 0;
}

uint8_t sda_fs_check_and_create_dir(uint8_t * fname) {
  if(svp_fexists(fname) && svp_is_dir(fname)) {
    return 0;
  }

  if(svp_fexists(fname) && !svp_is_dir(fname)) {
    printf ("%s: Error: Given name is not a directory. (%s)\n", __FUNCTION__, fname);
    return 1;
  }

  if(!svp_fexists(fname)) {
    
    svp_mkdir(fname);

    if(svp_fexists(fname) && svp_is_dir(fname)) {
      printf ("%s: Info: Created directory. (%s)\n", __FUNCTION__, fname);
      return 0;
    }
    printf ("%s: ERROR: Directory creation failed. (%s)\n", __FUNCTION__, fname);
    return 1;
  }
  return 1;
}

uint8_t sda_fs_touch(uint8_t * fname) {
  svp_file fp;
  if(svp_fopen_rw(&fp, fname) == 0) {
    return 1;
  }
  svp_fclose(&fp);
  return 0;
}

uint8_t sda_fs_copy_blocking(uint8_t* source, uint8_t* dest) {
  svp_file fp_source;
  svp_file fp_dest;

  if (!svp_fopen_read(&fp_source, source)) {
    printf("%s: ERROR: Source file cannot be openned: %s\n", __FUNCTION__, source);
    return 1;
  }
  
  if (!svp_fopen_rw(&fp_dest, dest)) {
    printf("%s: ERROR: Destination File cannot be openned: %s\n", __FUNCTION__, source);
    return 1;
  }

  uint8_t c;

  while (!svp_feof(&fp_source)) {
    c = svp_fread_u8(&fp_source);
    svp_fwrite(&fp_dest, &c, 1);
  }
  svp_fclose(&fp_source);
  svp_fclose(&fp_dest);

  return 0;
}
