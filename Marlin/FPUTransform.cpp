/*
  FPUTransform.c - dynamic bed leveling
  
 Copyright (C) 2012 Stephen Cropp
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

 #include "FPUTransform.h"

#if defined(UMFPUSUPPORT) && (UMFPUSUPPORT > -1)

#include "MatrixMath.h"

float MasterTransform[4][4]; // this is the transform that describes how to move from
							 // ideal coordinates to real world coords

// private functions
void loadMatrix(float Xdiff, float Ydiff, float ZdiffX, float ZdiffY);
void transformDestination(float &X, float &Y, float &Z);

bool FPUEnabled; // this is a bypass switch so that with one command the FPU can be
				 // turned off

void loadMatrix(float Xdiff, float Ydiff, float ZdiffX, float ZdiffY)
{
//modified to take advantage of small angle trig.
float Xtheta = ZdiffX / Xdiff;
float Ytheta = ZdiffY / Ydiff;
float cosxtheta = 1-(Xtheta*Xtheta)/2;
float sinxtheta = Xtheta;
float cosytheta = 1-(Ytheta*Ytheta)/2;
float sinytheta = Ytheta;

//rotate in Y using XZ 
float TransformY[4][4] = {{cosxtheta, 0.0, sinxtheta, 0.0},
						  {      0.0, 1.0,        0.0, 0.0}, 
						  {-sinxtheta, 0.0,  cosxtheta, 0.0}, 
						  {      0.0, 0.0,        0.0, 1.0}};
//rotate in X using YZ 
float TransformX[4][4] = {{ 1.0,         0.0,         0.0, 0.0},
						  { 0.0, cosytheta, sinytheta, 0.0}, 
						  { 0.0,sinytheta, cosytheta, 0.0}, 
						  { 0.0,         0.0,         0.0, 1.0}};


float MatrixStage4[4][4];
matrixMaths.MatrixMult((float*)TransformX, (float*)TransformY, 4, 4, 4, (float*)MasterTransform);
matrixMaths.MatrixPrint((float*)MasterTransform, 4, 4, "MasterTransform (pre-invert)");

// We now have a way to translate from real-world coordinates to idealised coortdinates, 
// but what we actually want is a way to transform from the idealised g-code coordinates
// to real world coordinates. 
// This is simply the inverse.
matrixMaths.MatrixInvert((float*)MasterTransform, 4);
matrixMaths.MatrixPrint((float*)MasterTransform, 4, 4, "MasterTransform");
}

void transformDestination(float &X, float &Y, float &Z)
{
float oldPoint[4][1]={{X}, {Y}, {Z}, {1.0}};
float newPoint[1][4]={{0.0,0.0,0.0,0.0}};
matrixMaths.MatrixMult((float*)MasterTransform, (float*)oldPoint, 4, 4, 1, (float*)newPoint);
X=newPoint[0][0];
Y=newPoint[0][1];
Z=newPoint[0][2];
}

void FPUTransform_init()
{
if (FPUEnabled == true)
  {
  // It is important to ensure that if the bed levelling routine has not been called the 
  // printer behaves as if the real world and idealised world are one and the same
  matrixMaths.MatrixIdentity((float*)MasterTransform,4,4);
  SERIAL_ECHO("transform configured to identity");
  }
else
  {
  SERIAL_ECHO("transform correction not enabled");
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

void FPUTransform_determineBedOrientation(float Z1_in, float Z2_in, float Z4_in)
{
int X1 = 15;
int Y1 = 15;
float Y2 = Y_MAX_LENGTH - 25;  
float Y3 = (Y1 + Y2) / 2;
float X4 = X_MAX_LENGTH - 25; 
float Z1;
float Z2; 
float Z3;
float Z4;

//get Z for X15 Y15, X15 Y(Y_MAX_LENGTH - 15) and X(X_MAX_LENGTH - 15) Y15
Z1 = Z1_in ? Z1_in : Probe_Bed(X1,Y1,PROBE_N);
Z2 = Z2_in ? Z2_in : Probe_Bed(X1,Y2,PROBE_N);
Z3 = (Z1 + Z2) / 2; //this cannot be measured with the bed spring probe.
Z4 = Z4_in ? Z4_in : Probe_Bed(X4,Y3,PROBE_N);

float Xdiff = X4 - X1;
	serialPrintFloat(Xdiff);
	SERIAL_ECHOLN("");
float Ydiff = Y2 - Y1;
	serialPrintFloat(Ydiff);
	SERIAL_ECHOLN("");
//clockwise
float ZdiffX = Z3 - Z4;
	serialPrintFloat(ZdiffX);
	SERIAL_ECHOLN("");
//anti clockwise
float ZdiffY = Z1 - Z2;
	serialPrintFloat(ZdiffY);
	SERIAL_ECHOLN("");

if(FPUEnabled)
	{
	loadMatrix(Xdiff, Ydiff, ZdiffX, ZdiffY);
	}
}

void FPUTransform_transformDestination()
{
float XPoint = destination[X_AXIS];          // float variable 
float YPoint = destination[Y_AXIS];          // float variable 
float ZPoint = destination[Z_AXIS];          // float variable 
if(FPUEnabled)
	{
	transformDestination(XPoint, YPoint, ZPoint);
	}    
modified_destination[X_AXIS] = XPoint;       // float variable 
modified_destination[Y_AXIS] = YPoint;       // float variable 
modified_destination[Z_AXIS] = ZPoint;       // float variable 
}	

#endif //UMFPUSUPPORT
