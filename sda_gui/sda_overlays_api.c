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
  overlayX1 = 32 + 80*svpSGlobal.lcdLandscape;
  overlayY1 = 64;
  overlayX2 = 288 + 80*svpSGlobal.lcdLandscape;
  overlayY2 = 416;
}


// stores current overlay, resets overlay screen,
// so it is not destroyed by the next setOverlayScreen
void overlayStore(sdaOverlayType *o) {
  o->overlayScr  = overlayScr;
  o->overlayCont = overlayCont;
  o->overlayX1   = overlayX1;
  o->overlayY1   = overlayY1;
  o->overlayX2   = overlayX2;
  o->overlayY2   = overlayY2;
  o->ov_id       = ov_id;
  o->overlay_destructor = overlay_destructor;
  o->destructor_set = destructor_set;
  overlayScr = 0;
}


void overlayRestore(sdaOverlayType *o) {
  overlayScr  = o->overlayScr;
  overlayCont = o->overlayCont;
  overlayX1   = o->overlayX1;
  overlayY1   = o->overlayY1;
  overlayX2   = o->overlayX2;
  overlayY2   = o->overlayY2;
  ov_id       = o->ov_id;
  overlay_destructor = o->overlay_destructor;
  destructor_set = o->destructor_set;
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
  svpSGlobal.systemRedraw = 1;
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
      gr2_destroy(overlayScr, overlayCont);
    }
    overlayScr = 0;
    sx_set_touch_ev(EV_NONE, 0, 0);
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


void setOverlayPos(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  overlayX1 = x;
  overlayY1 = y;
  overlayX2 = x + w;
  overlayY2 = y + h;
}


uint16_t getOverlayId() {
  if (overlayScr != 0) {
    return ov_id;
  } else {
    return 0;
  }
}


uint16_t getOverlayScreen() {
  if (overlayScr != 0) {
    return overlayScr;
  } else {
    return 0;
  }
}