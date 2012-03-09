#include "FPUTransform.h"

//-------------------- uM-FPU Register Definitions -----------------------------
#define newPoint    16                  // uM-FPU register
#define MasterTransform 20              // uM-FPU register
#define Xdiff   81                      // uM-FPU register
#define Ydiff   82                      // uM-FPU register
#define ZdiffX  83                      // uM-FPU register
#define ZdiffY  84                      // uM-FPU register
#define Z1_value    12                  // uM-FPU register

//-------------------- uM-FPU Function Definitions -----------------------------
#define loadMatrix  0                   // uM-FPU user function
#define transformPoint  1               // uM-FPU user function

#if defined(UMFPUSUPPORT) && (UMFPUSUPPORT > -1)

void FPUTransform_init()
{
    Fpu.begin(UMFPUSUPPORT_CS_PIN);
    if (Fpu.sync() == SYNC_CHAR)
      FpuSerial.printVersionln();
//      FPUT = FPUTransform
    else
      {
      SERIAL_ECHO("uM-FPU not detected");
      while(1) ; // stop if FPU not detected
      }
}

void FPUTransform_determineBedOrientation()
{
//-------------------- Variable Definitions ------------------------------------
float Z1;                               // float variable 
float Y2 = Y_MAX_LENGTH - 15;                               // float variable 
float Z2;                               // float variable 
float X3 = X_MAX_LENGTH - 15;                               // float variable 
float Z3;                               // float variable 

//get Z for X15 Y15, X15 Y(Y_MAX_LENGTH - 15) and X(X_MAX_LENGTH - 15) Y15
Z1 = Probe_Bed(15,15);
Z2 = Probe_Bed(15,Y2);
Z3 = Probe_Bed(X3,15);



//-------------------- Generated Code ------------------------------------------

    // Xdiff = X1 - X3
    Fpu.write(SELECTA, Xdiff, FSETI, 15);
    Fpu.write(FWRITE0);
    Fpu.writeFloat(X3);
    Fpu.write(FSUB0);
    // Ydiff = Y1 - Y2
    Fpu.write(SELECTA, Ydiff, FSETI, 15);
    Fpu.write(FWRITE0);
    Fpu.writeFloat(Y2);
    Fpu.write(FSUB0);
    // ZdiffX = Z1 - Z3
    Fpu.write(SELECTA, ZdiffX);
    Fpu.write(FWRITE0);
    Fpu.writeFloat(Z1);
    Fpu.write(FSET0, FWRITE0);
    Fpu.writeFloat(Z3);
    Fpu.write(FSUB0);
    // ZdiffY = Z1 - Z2
    Fpu.write(SELECTA, ZdiffY);
    Fpu.write(FWRITE0);
    Fpu.writeFloat(Z1);
    Fpu.write(FSET0, FWRITE0);
    Fpu.writeFloat(Z2);
    Fpu.write(FSUB0);
    // 
    // Z1_value EQU F12
    // Z1_value = Z1
    Fpu.write(FWRITE, Z1_value);
    Fpu.writeFloat(Z1);
    // 
    // loadMatrix
    Fpu.write(FCALL, loadMatrix);



}

void FPUTransform_transformPoint()
{

}

#endif //UMFPUSUPPORT
