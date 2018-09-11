#ifndef SDA_OVERLAYS_H
#define SDA_OVERLAYS_H
#include "../SDA_OS.h"

uint16_t setOverlayScreen(uint16_t val, gr2context * c);

void setOverlayX1(uint16_t val);
void setOverlayY1(uint16_t val);
void setOverlayX2(uint16_t val);
void setOverlayY2(uint16_t val);

uint16_t getOverlayId();

void destroyOverlay(); // calls the destructor, used to destroy any overlay

void overlayDestructorDone();
void setOverlayDestructor(void (*destructor) ());

#endif
