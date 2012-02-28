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
#define transformDestination    2       // uM-FPU user function
#define transformInit   3               // uM-FPU user function

#if defined(UMFPUSUPPORT) && (UMFPUSUPPORT > -1)

void FPUTransform_init()
{
    Fpu.begin(UMFPUSUPPORT_CS_PIN);
    if (Fpu.sync() == SYNC_CHAR)
	  {
      FpuSerial.printVersionln();
    Fpu.wait();
    // initialise FPU
      Fpu.write(FCALL, transformInit);
	  }
    else
      {
      SERIAL_ECHO("uM-FPU not detected");
      while(1) ; // stop if FPU not detected
      }
}

void FPUTransform_determineBedOrientation()
{
float Z1;  
float Y2 = Y_MAX_LENGTH - 15;  
float Z2; 
float X3 = X_MAX_LENGTH - 15; 
float Z3;

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

void FPUTransform_transformDestination()
{
//-------------------- uM-FPU Register Definitions -----------------------------
// First attempt at this is just going to take destination and modify it directly.
// This is a bit of a hack, would be tidier to take destination as an argument and
// return destination'
float XPoint = destination[X_AXIS];                           // float variable 
float YPoint = destination[Y_AXIS];                           // float variable 
float ZPoint = destination[Z_AXIS];                           // float variable 
    
//-------------------- Generated Code---------------------------------
    Fpu.write(FWRITE, 85);
    Fpu.writeFloat(XPoint);
    // F86 = YPoint
    Fpu.write(FWRITE, 86);
    Fpu.writeFloat(YPoint);
    // F87 = ZPoint
    Fpu.write(FWRITE, 87);
    Fpu.writeFloat(ZPoint);
    // transformDestination
    Fpu.write(FCALL, transformDestination);
    // XPoint = F85
    FpuSerial.printFloat((byte)85, 63);
    Fpu.write(SELECTA, 85);
    Fpu.wait();
    Fpu.write(FREADA);
    XPoint = Fpu.readFloat();
    // YPoint = F86
    FpuSerial.printFloat((byte)86, 63);
    Fpu.write(SELECTA, 86);
    Fpu.wait();
    Fpu.write(FREADA);
    YPoint = Fpu.readFloat();
    // ZPoint = F87
    FpuSerial.printFloat((byte)87, 63);
    Fpu.write(SELECTA, 87);
    Fpu.wait();
    Fpu.write(FREADA);
    ZPoint = Fpu.readFloat();
//-------------------- Generated Code (end)---------------------------------
    
modified_destination[X_AXIS] = XPoint;                           // float variable 
modified_destination[Y_AXIS] = YPoint;                           // float variable 
modified_destination[Z_AXIS] = ZPoint;                           // float variable 
}	

#endif //UMFPUSUPPORT
