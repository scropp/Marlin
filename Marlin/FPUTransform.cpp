#include "FPUTransform.h"

#if defined(UMFPUSUPPORT) && (UMFPUSUPPORT > -1)

void FPUTransform_init()
{
    Fpu.begin(UMFPUSUPPORT_CS);
    if (Fpu.sync() == SYNC_CHAR)
      FpuSerial.printVersionln();
    else
      {
      SERIAL_ECHO("uM-FPU not detected");
      while(1) ; // stop if FPU not detected
      }
}
#endif //UMFPUSUPPORT
