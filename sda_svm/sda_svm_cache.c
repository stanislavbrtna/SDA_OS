/*
Copyright (c) 2023 Stanislav Brtna

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

#include "sda_svm_cache.h"

extern svsVM svm;

void svmInitRemoveCache(uint8_t *ext) {
  uint8_t buffer[255];
  uint8_t retval;

  retval = svp_extFind(buffer, 30, ext, (uint8_t *) ".");

  while (retval) {
    svp_unlink(buffer);
    retval = svp_extFindNext(buffer, 30);
  }
}

void svmRemoveCachedProc(uint16_t pid) {
  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS");
  svmRemoveCachedFile(pid, (uint8_t *) ".gr0");
  svmRemoveCachedFile(pid, (uint8_t *) ".gr1");
  svmRemoveCachedFile(pid, (uint8_t *) ".gr2");
  svmRemoveCachedFile(pid, (uint8_t *) ".met");
  svmRemoveCachedFile(pid, (uint8_t *) ".stc");
  svmRemoveCachedFile(pid, (uint8_t *) ".svm");
  svmRemoveCachedFile(pid, (uint8_t *) ".str");
}


void svmRemoveCachedFile(uint16_t pid, uint8_t * tail) {
  uint8_t cacheBuffer[256];
  uint8_t numbuff[25];

  sda_int_to_str(numbuff, (int32_t)pid, sizeof(numbuff));
  sda_strcp((uint8_t *) "cache/", cacheBuffer, sizeof(cacheBuffer));
  sda_str_add(cacheBuffer, numbuff);
  sda_str_add(cacheBuffer, tail);

  svp_unlink(cacheBuffer);
#ifdef SVM_DBG_ENABLED
  printf("unlinking: %s\n", cacheBuffer);
#endif
}

// Cleans the app cache
void svmRemovePrecache() {
  uint8_t dirbuf[258];
  svp_getcwd(dirbuf, 256);

  svp_switch_main_dir();
  svp_chdir((uint8_t *)"APPS/cache/pre");
  printf("Cleaning up precached apps.\n");
  svmInitRemoveCache((uint8_t *) "stc");
  svmInitRemoveCache((uint8_t *) "svm");
  svmInitRemoveCache((uint8_t *) "str");
  svmInitRemoveCache((uint8_t *) "sec");
  svp_chdir(dirbuf);
}


void svmPrecacheGetName(uint8_t* buffer, uint32_t len, int32_t crc, uint8_t* ext) {
   uint8_t crcBuffer[32];
  sda_int_to_str(crcBuffer, crc, sizeof(crcBuffer));
  sda_strcp((uint8_t *) "cache/pre/", buffer, len);
  sda_str_add(buffer, crcBuffer);
  sda_str_add(buffer, ext);
}


void svmPrecacheGetFingerprint(uint8_t * runtimeFingerprint, uint16_t len, int mtime, uint32_t siz) {
  uint8_t buff[20];
  for(int i = 0; i < len; i++){
    runtimeFingerprint[i] = 0;
  }

  sda_int_to_str(runtimeFingerprint, siz, len);
  sda_str_add(runtimeFingerprint, "b\n");
  
  sda_int_to_str(buff, mtime, sizeof(buff) - 1);
  sda_str_add(runtimeFingerprint, buff);
  sda_str_add(runtimeFingerprint, "\n");
  
  #ifdef PC
  sda_str_add(runtimeFingerprint, "Platform:PC\n");
  #else
  sda_str_add(runtimeFingerprint, "Platform:UMC\n");
  #endif
  sda_str_add(runtimeFingerprint, "OSver: "SDA_OS_VERSION);
  sda_str_add(runtimeFingerprint, "\nSVSver: "SVS_VERSION);
  sda_str_add(runtimeFingerprint, "\nR:"__DATE__"-"__TIME__"\n");
}


uint8_t svmPrecacheFile(uint8_t *fname) {
  if (svmGetValid()) {
    printf("ERROR: cannot precache if svm is already running!");
    return 1;
  }

  // check for pre-tokenized app
  uint8_t fileBuffer[256];

  // get app name crc
  int32_t crc = (int32_t) crc32b(fname);
  if (crc < 0) {
    crc *= -1;
  }

  svp_file fil;
  svp_fopen_rw(&fil, fname);
  uint32_t siz = svp_get_size(&fil);
  int32_t  tim = svp_get_mtime(fname);
  svp_fclose(&fil);

  if(svmPreCachedExists(crc, tim, siz)) {
    printf("%s: Already cached (%s)\n",__FUNCTION__, fname);
    return 0;
  }

  // token cache
  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".stc");

  if (svmTokenizeFile(fname, fileBuffer, 0) != 0) {
    return 1;
  }

  // svm
  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".svm");

  svp_file svmFile;

  if(svp_fopen_rw(&svmFile, fileBuffer) == 0) {
    printf("precacher: file open error\n");
    return 1;
  }
  svp_fwrite(&svmFile, &svm, sizeof(svm));
  
  svp_fclose(&svmFile);

  // str
  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".str");
  
  if(svp_fopen_rw(&svmFile, fileBuffer) == 0) {
    printf("precacher: file open error\n");
    return 1;
  }

  svp_fwrite(&svmFile, svm.stringField, svm.stringFieldLen+1);
  
  svp_fclose(&svmFile);

  SVScloseCache(&svm);

  // build fingerprint file
  // str
  
  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".sec");
  
  if(svp_fopen_rw(&svmFile, fileBuffer) == 0) {
    printf("precacher: file open error\n");
    return 1;
  }

  uint8_t runtimeFingerprint[256];
  svmPrecacheGetFingerprint(runtimeFingerprint, sizeof(runtimeFingerprint), tim, siz);
  
  svp_fwrite(&svmFile, runtimeFingerprint, sizeof(runtimeFingerprint));
  svp_fclose(&svmFile);

  printf("Precaching: %s\n", fileBuffer);

  return 0;
}


uint8_t svmPreCachedExists(int32_t crc, int32_t tim, uint32_t siz) {
  uint8_t fileBuffer[256];
  
  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".stc");

  if (!svp_fexists(fileBuffer)) {
    return 0;
  }

  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".svm");

  if (!svp_fexists(fileBuffer)) {
    return 0;
  }

  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".str");

  if (!svp_fexists(fileBuffer)) {
    return 0;
  }

  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".sec");

  if (!svp_fexists(fileBuffer)) {
    return 0;
  }

  svp_file svmFile;

  if(svp_fopen_rw(&svmFile, fileBuffer) == 0) {
    printf("fingerprint check: file open error\n");
    return 0;
  }

  uint8_t runtimeFingerprint[256];
  uint8_t myFingerprint[256];

  svp_fread(&svmFile, runtimeFingerprint, sizeof(runtimeFingerprint));
  svp_fclose(&svmFile);

  svmPrecacheGetFingerprint(myFingerprint, sizeof(myFingerprint), tim, siz);

  if(svp_strcmp(runtimeFingerprint, myFingerprint) == 0) {
    printf("%s: Error: OS Fingerprint mismatch! (%s)\n",__FUNCTION__, fileBuffer);
    return 0;
  }

  return 1;
}


uint8_t svmLoadPrecached(int32_t crc) {
  uint8_t fileBuffer[256];
  svp_file svmFile;

  svsReset(&svm);
  tokenizerReset(&svm);

  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".svm");

  //printf("%s: loading cached: %s\n",__FUNCTION__, fileBuffer);

  if(svp_fopen_read(&svmFile, fileBuffer) == 0) {
    printf("sdaSvmLoader: file open error (%s)\n", fileBuffer);
    return 1;
  }

  svp_fread(&svmFile, &svm, sizeof(svm));
  svp_fclose(&svmFile);

  svmPrecacheGetName(fileBuffer, sizeof(fileBuffer), crc, (uint8_t *) ".str");

  if(svp_fopen_read(&svmFile, fileBuffer) == 0) {
    printf("sdaSvmLoader: file open error (%s)\n", fileBuffer);
    return 1;
  }

  svp_fread(&svmFile, svm.stringField, svm.stringFieldLen+1);
  svp_fclose(&svmFile);

  SVSopenCache(&svm);

  return 0;
}
