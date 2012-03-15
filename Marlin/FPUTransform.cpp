#include "FPUTransform.h"

//-------------------- uM-FPU Register Definitions -----------------------------
//#define newPoint    16                  // uM-FPU register
//#define MasterTransform 20              // uM-FPU register
//#define Xdiff   81                      // uM-FPU register
//#define Ydiff   82                      // uM-FPU register
//#define ZdiffX  83                      // uM-FPU register
//#define ZdiffY  84                      // uM-FPU register
//#define Z1_value    12                  // uM-FPU register

//-------------------- uM-FPU Function Definitions -----------------------------
//#define loadMatrix  0                   // uM-FPU user function
//#define transformPoint  1               // uM-FPU user function
//#define transformDestination    2       // uM-FPU user function
//#define transformInit   3               // uM-FPU user function

#if defined(UMFPUSUPPORT) && (UMFPUSUPPORT > -1)

bool FPUEnabled;

//#FUNC 0 loadMatrix
void loadMatrix()
{
//#FUNCTION loadMatrix
//Xtheta EQU F75
//Ytheta EQU F76
//cosxtheta EQU f77
//sinxtheta EQU f78
//cosytheta EQU f79
//sinytheta EQU f80
//one EQU F11
//Transform1 EQU F37 //4x4matrix 37 - 52
//Transform2 EQU F53 //4x4matrix 53 - 68


//Xtheta = ZdiffX / Xdiff
//Ytheta = ZdiffY / Ydiff
//cosxtheta = cos(Xtheta)
//sinxtheta = sin(Xtheta)
//cosytheta = cos(Ytheta)
//sinytheta = sin(Ytheta)

//first rotate in Y using XZ 
//[cos(t), 0, -sin(t), 0]
//[0     , 1, 0      , 0]
//[sin{t}, 0,  cos(t), 0]
//[0     , 0, 0      , 1]

//SELECTMB(Transform1, 4, 4)
//MOP(LOADRB, cosxtheta, F0, -sinxtheta, F0, F0, one, F0, F0, sinxtheta, F0,  cosxtheta, //F0, F0, F0, F0, one)
//first rotate in X using YZ 
//[1,       0, 0     , 0]
//[0,  cos(t), sin(t), 0]
//[0, -sin{t}, cos(t), 0]
//[0,       0, 0     , 1]

//SELECTMC(Transform2, 4, 4)
//MOP(LOADRC, one, F0, F0, F0, F0, cosxtheta, sinxtheta, F0, F0,-sinxtheta, cosxtheta, //F0, F0, F0, F0, one)
//SELECTMA(MasterTransform, 4, 4)
//MOP(IDENTITY)
//MOP(MULTIPLY)

//now we have the object in a plane that is parallel to the x axes, but we do not have the z height correct.
//transform the x15 y15 position using our rotation matrix and use the result to determine where z0 is and
//add this translate function to the transform matrix.

//SELECTMA(Transform2, 4, 4)
//MOP(IDENTITY)
//transformPoint(X1, Y1, Z1_value)
//SELECTMA(newPoint, 4,1 )
//MOP(SAVEAR, F1, F2, F3, -F0)
//F65 = 15-F1     //f65 is reference into Transform2 for x translate value
//F66 = 15-F2     //f66 is reference into Transform2 for y translate value
//F67 = F3     //f67 is reference into Transform2 for z translate value

//SELECTMC(Transform2, 4, 4) //this is the new translate matrix

//SELECTMA(MasterTransform, 4, 4)  // copy this into Transform 1
//SELECTMB(Transform1, 4, 4)
//MOP(COPYAB)
//now everything is in place
//MOP(MULTIPLY) // MA = MB . MC

// We now have a way to translate from real-world coordinates to idealised coortdinates, //but what we actually want is 
// a way to transform from the idealised g-code coordinates to real world coordinates. //This is simply the inverse.
//MOP(COPYAB)
//now everything is in place
//INVERSE_4x4 // MA = inverse(MB)
//SELECTMB(MasterTransform, 4, 4)  // copy this into Transform 1
//MOP(COPYBA)
}

//#FUNC % transformDestination // input X=F1 Y=F2 Z=F3 out X'=F16 Y'=F17 Z'=18
void transformDestination(float *X, float *Y, float *Z)
{
//oldPoint EQU F69 //1x4 matrix 12 - 15
//SELECTMB(oldPoint, 1, 4)
//MOP(LOADRB, F85, F86, F87, one)

//SELECTMC(MasterTransform, 4, 4)

//SELECTMA(newPoint, 4,1 )

//MOP(MULTIPLY)

//SELECTMA(newPoint, 4,1 )
//MOP(SAVEAR, F85, F86, F87, F88)
}

//#FUNC % transformInit
void transformInit()
{
// It is important to ensure that if the bed levelling routine has not been called the printer 
// behaves as if the real world and idealised world are one and the same
//SELECTMA(MasterTransform, 4, 4)
//MOP(IDENTITY)
//one EQU F11
//one = 1

}

void FPUTransform_init()
{
if (FPUEnabled == true)
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
    SERIAL_ECHO("FPU Processing is not available");
    FPUEnabled = false;
    }
  }
else
  {
SERIAL_ECHO("FPU not enabled");
  }
}

void FPUEnable()
{
	FPUEnabled = true;
	FPUTransform_init();
}

void FPUReset()
{
	FPUTransform_init();
}

void FPUDisable()
{
	FPUEnabled = false;
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
if(FPUEnabled)
	{
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
if(FPUEnabled)
	{
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
	}    
modified_destination[X_AXIS] = XPoint;                           // float variable 
modified_destination[Y_AXIS] = YPoint;                           // float variable 
modified_destination[Z_AXIS] = ZPoint;                           // float variable 
}	

#endif //UMFPUSUPPORT
