#ifndef SDA_P16_H
#define SDA_P16_H

typedef struct {
  uint8_t version;
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

void sda_p16_set_alpha(uint8_t enable, uint16_t color, uint16_t bg_color);
void sda_p16_set_pmc(uint8_t enable, uint16_t color);
uint8_t sda_draw_p16(uint16_t x, uint16_t y, uint8_t *filename);
uint8_t sda_draw_p16_scaled_up(uint16_t x, uint16_t y, uint16_t width_n, uint16_t height_n, uint8_t *filename);
uint16_t sda_p16_get_width(uint8_t *filename);

#endif
