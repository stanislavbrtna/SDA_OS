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

#ifndef SVP_FS_H
#define SVP_FS_H

#include "../SDA_OS.h"

uint8_t svp_fopen_read(svp_file *fp, uint8_t *fname);
uint8_t svp_fopen_rw(svp_file *fp, uint8_t *fname);

uint8_t svp_fread_u8(svp_file *fp);
void svp_fwrite_u8(svp_file *fp, uint8_t val);

uint8_t svp_fread(svp_file *fp, void *target, uint32_t size);
void svp_fwrite(svp_file *fp, void *target, uint32_t size);

uint8_t svp_feof(svp_file *fp);
uint8_t svp_fclose(svp_file *fp);
uint8_t svp_fseek(svp_file *fp, uint32_t offset);

void svp_truncate(svp_file *fp);

uint8_t svp_getcwd(uint8_t* buf, uint16_t len);
uint8_t svp_chdir(uint8_t* path);

uint8_t svp_is_dir(uint8_t* path);
uint8_t svp_mkdir(uint8_t* path);

uint8_t svp_strcmp(uint8_t *a, uint8_t *b);
uint16_t svp_strlen(uint8_t *str);

uint8_t svp_fexists(uint8_t *fname);
uint32_t svp_get_size(svp_file *fp);
uint32_t svp_ftell(svp_file *fp);

uint8_t svp_unlink(uint8_t* path);
uint8_t svp_rmdir(uint8_t* path);
void svp_fsync(svp_file *fp);

uint8_t svp_extFindNext(uint8_t *outStr, uint16_t len);
uint8_t svp_extFind(uint8_t *outStr, uint16_t len, uint8_t *extension, uint8_t *directory);

uint8_t svp_rename(uint8_t *source, uint8_t *dest);

uint8_t svp_mount();
void svp_umount();
uint8_t svp_getMounted();
void svp_setMounted(uint8_t val);

#endif
