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

#ifndef SVP_CSV_PARSE
#define SVP_CSV_PARSE
#include "../SDA_OS.h"

//if you want some debug output, just define
//define CSV_DBG

typedef struct {
  svp_file fil;
  uint8_t  separator;
} svp_csvf;

uint8_t svp_csv_open(svp_csvf *fc, uint8_t * fname);
void svp_csv_set_separator(svp_csvf *fc, uint8_t separator);
uint8_t svp_csv_close(svp_csvf *fc);
void svp_csv_new(svp_csvf *fc, uint8_t count); //aka newline
uint16_t svp_csv_get_cell(svp_csvf *fc, uint8_t index, uint8_t * def, uint8_t * buff, uint16_t len);
uint8_t svp_csv_set_cell(svp_csvf *fc, uint8_t index, uint8_t * value);
// vrací nula pokud endline
uint8_t svp_csv_next_line(svp_csvf *fc);

void svp_csv_remove_line(svp_csvf *fc);
void svp_csv_rewind(svp_csvf *fc);

#endif
