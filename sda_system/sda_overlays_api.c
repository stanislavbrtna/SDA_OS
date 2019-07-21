#include "sda_overlays_api.h"

extern uint16_t overlayScr; // if not zero, then overlay is valid
extern gr2context * overlayCont;
extern uint16_t overlayX1;
extern uint16_t overlayX2;
extern uint16_t overlayY1;
extern uint16_t overlayY2;

extern uint16_t ov_id;

void (*overlay_destructor) ();
uint8_t destructor_set;


void setOverlayDefault() {
  overlayX1 = 32;
  overlayY1 = 64;
  overlayX2 = 288;
  overlayY2 = 416;
}


uint16_t setOverlayScreen(uint16_t val, gr2context * c) {

  if (overlayScr != 0) {
    destroyOverlay();
  }
  ov_id++;
  destructor_set = 0;
  overlayScr = val;
  overlayCont = c;
  setOverlayDefault();
  return ov_id;
}


void setOverlayDestructor(void (*destructor) ()) {
  overlay_destructor = destructor;
  destructor_set = 1;
}


void overlayDestructorDone() {
  destructor_set = 2;
}


void destroyOverlay() {
  if (overlayScr != 0) {
    if (destructor_set == 1) {
      (*overlay_destructor) ();
    } else if (destructor_set == 2) {
      // destructor was called from somewhere else
      return;
    } else if (destructor_set == 0) {
      pscg_destroy(overlayScr, overlayCont);
    }
    overlayScr = 0;
  }
}


void setOverlayX1(uint16_t val) {
  overlayX1 = val;
}


void setOverlayY1(uint16_t val) {
  overlayY1 = val;
}


void setOverlayX2(uint16_t val) {
  overlayX2 = val;
}


void setOverlayY2(uint16_t val) {
  overlayY2 = val;
}


uint16_t getOverlayId() {
  if (overlayScr != 0) {
    return ov_id;
  } else {
    return 0;
  }
}
