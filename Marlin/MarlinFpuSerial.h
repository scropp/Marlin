/*
  FpuSerial.h - FPU serial print library
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

#ifndef FpuSerial_h
#define FpuSerial_h

#include "WConstants.h"

//-------------------- function prototypes ----------------------------------

class UMFPUSerial
{
  public:
    void printVersion(void);
    void printVersionln(void);
    void printFloat(byte);
    void printFloatln(byte);
    void printFloat(byte, byte);
    void printFloatln(byte, byte);
    void printFloat(float, byte);
    void printFloatln(float, byte);
    void printLong(byte);
    void printLongln(byte);
    void printLong(byte, byte);
    void printLongln(byte, byte);
    void printString(void);
    void printStringln(void);
    void printString(byte);
    void printStringln(byte);
};

extern UMFPUSerial FpuSerial;

#endif
