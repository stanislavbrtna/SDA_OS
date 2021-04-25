#ifndef SDA_SCREEN_SLOTS_H
#define SDA_SCREEN_SLOTS_H

#include "../SDA_OS.h"

void sda_slot_init(uint8_t slot, uint16_t screen, gr2context *context, uint16_t valid, uint16_t top);

uint8_t sda_if_slot_on_top(uint8_t slot);
uint8_t sda_get_prev_top_screen_slot();

void sda_slot_set_valid(uint16_t slot);
void sda_slot_set_invalid(uint16_t slot);
uint16_t sda_slot_get_valid(uint16_t slot);
uint16_t sda_slot_get_screen(uint8_t slot);
void sda_slot_on_top(uint8_t slot);
uint8_t sda_get_top_slot();

#endif