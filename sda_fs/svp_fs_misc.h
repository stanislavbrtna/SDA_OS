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

#ifndef SVP_FS_MISC
#define SVP_FS_MISC
#include "svp_fs.h"
#include "../SVS/svs_basics.h"
#include "../SVS/svs_limits.h"
#include "../SVS/svs_types.h"
uint16_t svp_load_file_to_svs(uint8_t *fname, svsVM *s);
uint16_t svp_store_svs_to_file(uint8_t *fname, uint8_t *str, svsVM *s);

uint8_t sda_fs_check_and_create_dir(uint8_t * fname);
uint8_t sda_fs_touch(uint8_t * fname);

uint8_t sda_fs_copy_blocking(uint8_t* source, uint8_t* dest);

#endif
