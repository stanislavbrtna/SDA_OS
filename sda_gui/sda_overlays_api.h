#ifndef SDA_OVERLAYS_H
#define SDA_OVERLAYS_H
#include "../SDA_OS.h"

uint16_t setOverlayScreen(uint16_t val, gr2context * c);

void setOverlayX1(uint16_t val);
void setOverlayY1(uint16_t val);
void setOverlayX2(uint16_t val);
void setOverlayY2(uint16_t val);
void setOverlayPos(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

uint16_t getOverlayId();
uint16_t getOverlayScreen();

void overlayStore(sdaOverlayType *o);
void overlayRestore(sdaOverlayType *o);

void destroyOverlay(); // destroys current overlay, calls the destructor

void overlayDestructorDone();
void setOverlayDestructor(void (*destructor) ());

#endif
