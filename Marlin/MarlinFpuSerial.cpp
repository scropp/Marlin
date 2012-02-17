/*
  FpuSerial.cpp - FPU serial print library
  Copyright (c) 2008-2011 Cam Thompson.

  @author Cam Thompson, Micromega Corporation <www.micromegacorp.com>

  @version
    October 2, 2011
    - updated for new SPI and FpuSerial libraries
    December 15, 2008
    - original version

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Marlin.h"
#include "print.h"
#include "MarlinFpu.h"
#include "MarlinFpuSerial.h"



//------------------- printVersion ------------------------------------------

void UMFPUSerial::printVersion(void)
{
  Fpu.write(VERSION);
  printString();
}

void UMFPUSerial::printVersionln(void)
{
  Fpu.write(VERSION);
  printStringln();
}

//------------------- printFloat --------------------------------------------

void UMFPUSerial::printFloat(byte format)
{
  Fpu.write(FTOA, format);
  printString();
}

void UMFPUSerial::printFloatln(byte format)
{
  Fpu.write(FTOA, format);
  printStringln();
}

void UMFPUSerial::printFloat(byte reg, byte format)
{
  Fpu.write(SELECTA, reg);
  printFloat(format);
}

void UMFPUSerial::printFloatln(byte reg, byte format)
{
  Fpu.write(SELECTA, reg);
  printFloatln(format);
}

void UMFPUSerial::printFloat(float fval, byte format)
{
  Fpu.write(LEFT, FWRITEA);
  Fpu.writeFloat(fval);
  printFloat(format);
  Fpu.write(RIGHT);
}

void UMFPUSerial::printFloatln(float fval, byte format)
{
  printFloat(fval, format);
  SerialMgr.cur()->println();
}

//------------------- printLong ---------------------------------------------

void UMFPUSerial::printLong(byte format)
{
  Fpu.write(LTOA, format);
  printString();
}

void UMFPUSerial::printLongln(byte format)
{
  Fpu.write(LTOA, format);
  printStringln();
}


void UMFPUSerial::printLong(byte reg, byte format)
{
  Fpu.write(SELECTA, reg);
  printLong(format);
}

void UMFPUSerial::printLongln(byte reg, byte format)
{
  Fpu.write(SELECTA, reg);
  printLongln(format);
}

//------------------- printString -------------------------------------------

void UMFPUSerial::printString(void)
{
  printString(READSTR);
}

void UMFPUSerial::printStringln(void)
{
  printString(READSTR);
  SerialMgr.cur()->println();
}

void UMFPUSerial::printString(byte opcode)
{
  byte data;

  if (opcode != READSEL) opcode = READSTR;
  Fpu.wait();
  Fpu.write(opcode);
  Fpu.readDelay();

  while (1)
  {
    data = Fpu.read();
    if (data == 0) break;
    SerialMgr.cur()->print(data);
  }
}

void UMFPUSerial::printStringln(byte opcode)
{
	printString(opcode);
	SerialMgr.cur()->println();
}

//---------- preinstantiate FPU object --------------------------------------

UMFPUSerial FpuSerial = UMFPUSerial();
