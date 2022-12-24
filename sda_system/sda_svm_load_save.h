#ifndef SDA_SVM_LOAD_SAVE
#define SDA_SVM_LOAD_SAVE
#include "sda_svm.h"

uint8_t sdaSvmLoader(uint16_t id, uint8_t * tail, void *target, uint32_t size);
void sdaSvmSaver(uint16_t id, uint8_t * tail, void *target, uint32_t size);
#endif