#include "FPUTransform.h"

#if defined(UMFPUSUPPORT) && (UMFPUSUPPORT > -1)

#include "MatrixMath.h"

//-------------------- uM-FPU Register Definitions -----------------------------
//#define newPoint    16                  // uM-FPU register
//#define MasterTransform 20              // uM-FPU register
float MasterTransform[4][4]; // this is the transform that describes how to move from
							 // ideal coordinates to real world coords
//#define Xdiff   81                      // uM-FPU register
//#define Ydiff   82                      // uM-FPU register
//#define ZdiffX  83                      // uM-FPU register
//#define ZdiffY  84                      // uM-FPU register
//#define Z1_value    12                  // uM-FPU register

//-------------------- uM-FPU Function Definitions -----------------------------
// these are the private functions (todo: this aught to be c++ified)
//#define loadMatrix  0                   // uM-FPU user function
void loadMatrix(float X1, float Y1, float Z1, float Y2, float Z2, float X3, float Z3);
//#define transformPoint  1               // uM-FPU user function
//#define transformDestination    2       // uM-FPU user function
void transformDestination(float &X, float &Y, float &Z);
//#define transformInit   3               // uM-FPU user function

bool FPUEnabled; // this is a bypass switch so that with one command the FPU can be
				 // turned off

//#FUNC 0 loadMatrix
void loadMatrix(float X1, float Y1, float Z1, float Y2, float Z2, float X3, float Z3)
{
float Xdiff = X1 - X3;
float Ydiff = Y1 - Y2;
float ZdiffX = Z1 - Z3;
float ZdiffY = Z1 - Z2;

//Xtheta EQU F75
//Xtheta = ZdiffX / Xdiff
float Xtheta = ZdiffX / Xdiff;
//Ytheta EQU F76
//Ytheta = ZdiffY / Ydiff
float Ytheta = ZdiffY / Ydiff;
//cosxtheta EQU f77
//cosxtheta = cos(Xtheta)
float cosxtheta = cos(Xtheta);
//sinxtheta EQU f78
//sinxtheta = sin(Xtheta)
float sinxtheta = sin(Xtheta);
//cosytheta EQU f79
//cosytheta = cos(Ytheta)
float cosytheta = cos(Ytheta);
//sinytheta EQU f80
//sinytheta = sin(Ytheta)
float sinytheta = sin(Ytheta);
//one EQU F11

//first rotate in Y using XZ 
//[cos(t), 0, -sin(t), 0]
//[0     , 1, 0      , 0]
//[sin{t}, 0,  cos(t), 0]
//[0     , 0, 0      , 1]

//Transform1 EQU F37 //4x4matrix 37 - 52
//SELECTMB(Transform1, 4, 4)
//MOP(LOADRB, cosxtheta, F0, -sinxtheta, F0, F0, one, F0, F0, sinxtheta, F0,  cosxtheta, //F0, F0, F0, F0, one)
float Transform1[4][4] = {{cosxtheta, 0, -sinxtheta, 0},
						  {		   0, 1,          0, 0}, 
						  {sinxtheta, 0,  cosxtheta, 0}, 
						  {        0, 0,          0, 1}};

//first rotate in X using YZ 
//[1,       0, 0     , 0]
//[0,  cos(t), sin(t), 0]
//[0, -sin{t}, cos(t), 0]
//[0,       0, 0     , 1]

//Transform2 EQU F53 //4x4matrix 53 - 68
//SELECTMC(Transform2, 4, 4)
//MOP(LOADRC, one, F0, F0, F0, F0, cosxtheta, sinxtheta, F0, F0,-sinxtheta, cosxtheta, //F0, F0, F0, F0, one)
// NOTE: ^^^^^ there is a bug in my original code. This should be rotating in x so it 
// needs the y values!
float Transform2[4][4] = {{ 1,         0,         0, 0},
						  { 0, cosytheta, sinytheta, 0}, 
						  { 0,-sinytheta, cosytheta, 0}, 
						  { 0,         0,         0, 1}};
//SELECTMA(MasterTransform, 4, 4)
//MOP(IDENTITY)
//MOP(MULTIPLY)
float rotMatrix[4][4];
matrixMaths.MatrixMult((float*)Transform1, (float*)Transform2, 4, 4, 4, (float*)rotMatrix);

//now we have the object in a plane that is parallel to the x axes, but we do not have the z height correct.
//transform the x15 y15 position using our rotation matrix and use the result to determine where z0 is and
//add this translate function to the transform matrix.

//SELECTMA(Transform2, 4, 4)
//MOP(IDENTITY)
//transformPoint(X1, Y1, Z1_value)
float zError[1][4]={{X1,Y1,Z1,1}};
float zErrNew[4][1];
matrixMaths.MatrixMult((float*)zError, (float*)rotMatrix, 1, 4, 4, (float*)zErrNew);

//SELECTMA(newPoint, 4,1 )
//MOP(SAVEAR, F1, F2, F3, -F0)
//F65 = 15-F1     //f65 is reference into Transform2 for x translate value
//F66 = 15-F2     //f66 is reference into Transform2 for y translate value
//F67 = F3     //f67 is reference into Transform2 for z translate value
float zTranslate[4][4] = {{ 1, 0, 0, 0},
						  { 0, 1, 0, 0}, 
						  { 0, 0, 1, 0}, 
						  { 15-zErrNew[0][0],15-zErrNew[1][0],zErrNew[2][0], 1}};

//SELECTMC(Transform2, 4, 4) //this is the new translate matrix

//SELECTMA(MasterTransform, 4, 4)  // copy this into Transform 1
//SELECTMB(Transform1, 4, 4)
//MOP(COPYAB)
//now everything is in place
//MOP(MULTIPLY) // MA = MB . MC
matrixMaths.MatrixMult((float*)rotMatrix, (float*)zTranslate, 4, 4, 4, (float*)MasterTransform);

// We now have a way to translate from real-world coordinates to idealised coortdinates, //but what we actually want is 
// a way to transform from the idealised g-code coordinates to real world coordinates. //This is simply the inverse.
//MOP(COPYAB)
//now everything is in place
//INVERSE_4x4 // MA = inverse(MB)
//SELECTMB(MasterTransform, 4, 4)  // copy this into Transform 1
//MOP(COPYBA)
matrixMaths.MatrixInvert((float*)MasterTransform, 4);
}

//#FUNC % transformDestination // input X=F1 Y=F2 Z=F3 out X'=F16 Y'=F17 Z'=18
void transformDestination(float &X, float &Y, float &Z)
{
//oldPoint EQU F69 //1x4 matrix 12 - 15
float oldPoint[1][4]={{X, Y, Z, 1}};
float newPoint[4][1]={{0},{0},{0},{0}};

//SELECTMB(oldPoint, 1, 4)
//MOP(LOADRB, F85, F86, F87, one)

//SELECTMC(MasterTransform, 4, 4)

//SELECTMA(newPoint, 4,1 )

//MOP(MULTIPLY)
matrixMaths.MatrixMult((float*)oldPoint, (float*)MasterTransform, 1, 4, 4, (float*)newPoint);

//SELECTMA(newPoint, 4,1 )
//MOP(SAVEAR, F85, F86, F87, F88)
X=newPoint[0][0];
Y=newPoint[1][0];
Z=newPoint[2][0];
}

//#FUNC % transformInit
//void transformInit()
//{
// It is important to ensure that if the bed levelling routine has not been called the printer 
// behaves as if the real world and idealised world are one and the same
//SELECTMA(MasterTransform, 4, 4)
//MOP(IDENTITY)
//one EQU F11
//one = 1
//matrixMaths.MatrixIdentity(MasterTransform,4,4);
//}

void FPUTransform_init()
{
if (FPUEnabled == true)
  {
//  Fpu.begin(UMFPUSUPPORT_CS_PIN);
//  if (Fpu.sync() == SYNC_CHAR)
//    {
//    FpuSerial.printVersionln();
//    Fpu.wait();
//    // initialise FPU
//    Fpu.write(FCALL, transformInit);
// It is important to ensure that if the bed levelling routine has not been called the printer 
// behaves as if the real world and idealised world are one and the same
matrixMaths.MatrixIdentity((float*)MasterTransform,4,4);
//	}
//  else
//    {
//    SERIAL_ECHO("uM-FPU not detected");
//    SERIAL_ECHO("FPU Processing is not available");
//    FPUEnabled = false;
//    }
  }
else
  {
  SERIAL_ECHO("transorm correction not enabled");
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

    // loadMatrix
//    Fpu.write(FCALL, loadMatrix);
loadMatrix(15, 15, Z1, Y2, Z2, X3, Z3);
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
//    Fpu.write(FWRITE, 85);
//    Fpu.writeFloat(XPoint);
//    // F86 = YPoint
//    Fpu.write(FWRITE, 86);
//    Fpu.writeFloat(YPoint);
//    // F87 = ZPoint
//    Fpu.write(FWRITE, 87);
//    Fpu.writeFloat(ZPoint);
//    // transformDestination
//    Fpu.write(FCALL, transformDestination);
//    // XPoint = F85
//    FpuSerial.printFloat((byte)85, 63);
//    Fpu.write(SELECTA, 85);
//    Fpu.wait();
//    Fpu.write(FREADA);
//    XPoint = Fpu.readFloat();
//    // YPoint = F86
//    FpuSerial.printFloat((byte)86, 63);
//    Fpu.write(SELECTA, 86);
//    Fpu.wait();
//    Fpu.write(FREADA);
//    YPoint = Fpu.readFloat();
//    // ZPoint = F87
//    FpuSerial.printFloat((byte)87, 63);
//    Fpu.write(SELECTA, 87);
//    Fpu.wait();
//    Fpu.write(FREADA);
//    ZPoint = Fpu.readFloat();
//-------------------- Generated Code (end)---------------------------------
transformDestination(XPoint, YPoint, ZPoint);
	}    
modified_destination[X_AXIS] = XPoint;                           // float variable 
modified_destination[Y_AXIS] = YPoint;                           // float variable 
modified_destination[Z_AXIS] = ZPoint;                           // float variable 
}	

#endif //UMFPUSUPPORT
