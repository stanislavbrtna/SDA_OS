#ifndef SVP_CSV_PARSE
#define SVP_CSV_PARSE
#include "../SDA_OS.h"

//if you want some debug output, just define
//define CSV_DBG

typedef struct {
  svp_file fil;
} svp_csvf;

uint8_t svp_csv_open(svp_csvf *fc, uint8_t * fname);
uint8_t svp_csv_close(svp_csvf *fc);
void svp_csv_new(svp_csvf *fc, uint8_t count); //aka newline
uint16_t svp_csv_get_cell(svp_csvf *fc, uint8_t index, uint8_t * def, uint8_t * buff, uint16_t len);
uint8_t svp_csv_set_cell(svp_csvf *fc, uint8_t index, uint8_t * value);
// vrací nula pokud endline
uint8_t svp_csv_next_line(svp_csvf *fc);

void svp_csv_remove_line(svp_csvf *fc);
void svp_csv_rewind(svp_csvf *fc);

#endif
