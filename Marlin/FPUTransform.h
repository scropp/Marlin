#ifndef __FPUTRANSFORM

#define __FPUTRANSFORM
#include "Marlin.h"
#include "MarlinSPI.h"
#include "MarlinFpu.h"
#include "MarlinFpuSerial.h"
#include "z_probe.h"

#if defined(UMFPUSUPPORT) && (UMFPUSUPPORT > -1) 
  void FPUTransform_init();
  void FPUTransform_determineBedOrientation();
  void FPUTransform_transformDestination(); 

#else //no UMFPU SUPPORT
  FORCE_INLINE void FPUTransform_init() {};

#endif //UMFPUSUPPORT

#endif //__FPUTRANSFORM
