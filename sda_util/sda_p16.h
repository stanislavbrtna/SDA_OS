#ifndef SDA_P16_H
#define SDA_P16_H

typedef struct {
  uint16_t imageWidth;
  uint16_t imageHeight;
  uint8_t  storageMode;
  uint8_t  dataOffset;
} p16Header;

typedef struct {
  uint32_t init;
  uint32_t fpos;
  uint16_t prevVal;
  uint16_t repeat;
} p16State;

#endif
