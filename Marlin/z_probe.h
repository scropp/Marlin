#ifndef __Z_PROBEH

#define __Z_PROBEH
#include "Marlin.h"

#if defined(PROBE_PIN) && (PROBE_PIN > -1) 
  void probe_init();
  void probe_3points();
  void probe_1point();
  
#else //no probe pin
  FORCE_INLINE void probe_init() {};
  FORCE_INLINE void probe_3points() {};
  FORCE_INLINE void probe_1point() {};
#endif //PROBE_PIN

#endif
