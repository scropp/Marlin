/*
  Fpu.cpp - uM-FPU V3.1 floating point coprocessor library
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

#include "marlin.h"
#include "MarlinFpu.h"
#include "MarlinSPI.h"

//---------- constructor ----------------------------------------------------

UMFPU::UMFPU()
{
}

//------------------- begin -------------------------------------------------

void UMFPU::begin(void)
{
  begin(SS_PIN);
}

void UMFPU::begin(byte pin)
{
  // initialize the chip select
  _cs = pin;
  digitalWrite(_cs, HIGH);
  pinMode(_cs, OUTPUT);
  reset();
}

//------------------- reset -------------------------------------------------

void UMFPU::reset()
{
  digitalWrite(_cs, LOW);

  // disable SPI.Master
  SPI.end();

  // reset the FPU
  digitalWrite(MOSI_PIN, HIGH);
  for (byte i = 0; i < 80; i++)
  {
    digitalWrite(SCK_PIN, HIGH);
    digitalWrite(SCK_PIN, LOW);
  }
  digitalWrite(MOSI_PIN, LOW);

  delay(10);

  // enable SPI.Master
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.begin();

  digitalWrite(_cs, HIGH);
}

//------------------- sync --------------------------------------------------

byte UMFPU::sync()
{
  write(SYNC);
  return read();
}

//------------------- wait --------------------------------------------------

void UMFPU::wait(void)
{
  digitalWrite(_cs, LOW);
  delayMicroseconds(1);
  while (digitalRead(MISO_PIN)) ;
  digitalWrite(_cs, HIGH);
}

//------------------- write -------------------------------------------------

void UMFPU::write(byte b1)
{
  digitalWrite(_cs, LOW);
  SPI.transfer(b1);
  digitalWrite(_cs, HIGH);
}

void UMFPU::write(byte b1, byte b2)
{
  digitalWrite(_cs, LOW);
  SPI.transfer(b1);
  SPI.transfer(b2);
  digitalWrite(_cs, HIGH);
}

void UMFPU::write(byte b1, byte b2, byte b3)
{
  digitalWrite(_cs, LOW);
  SPI.transfer(b1);
  SPI.transfer(b2);
  SPI.transfer(b3);
  digitalWrite(_cs, HIGH);
}

void UMFPU::write(byte b1, byte b2, byte b3, byte b4)
{
  digitalWrite(_cs, LOW);
  SPI.transfer(b1);
  SPI.transfer(b2);
  SPI.transfer(b3);
  SPI.transfer(b4);
  digitalWrite(_cs, HIGH);
}

void UMFPU::write(byte b1, byte b2, byte b3, byte b4, byte b5)
{
  digitalWrite(_cs, LOW);
  SPI.transfer(b1);
  SPI.transfer(b2);
  SPI.transfer(b3);
  SPI.transfer(b4);
  SPI.transfer(b5);
  digitalWrite(_cs, HIGH);
}

void UMFPU::write(byte b1, byte b2, byte b3, byte b4, byte b5, byte b6)
{
  digitalWrite(_cs, LOW);
  SPI.transfer(b1);
  SPI.transfer(b2);
  SPI.transfer(b3);
  SPI.transfer(b4);
  SPI.transfer(b5);
  SPI.transfer(b6);
  digitalWrite(_cs, HIGH);
}

void UMFPU::write(byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7, byte b8, byte b9, byte ba, byte bb, byte bc, byte bd, byte be, byte bf, byte bg, byte bh, byte bi, byte bj)
{
  digitalWrite(_cs, LOW);
  SPI.transfer(b1);
  SPI.transfer(b2);
  SPI.transfer(b3);
  SPI.transfer(b4);
  SPI.transfer(b5);
  SPI.transfer(b6);
  SPI.transfer(b7);
  SPI.transfer(b8);
  SPI.transfer(b9);
  SPI.transfer(ba);
  SPI.transfer(bb);
  SPI.transfer(bc);
  SPI.transfer(bd);
  SPI.transfer(be);
  SPI.transfer(bf);
  SPI.transfer(bg);
  SPI.transfer(bh);
  SPI.transfer(bi);
  SPI.transfer(bj);
  digitalWrite(_cs, HIGH);
}


void UMFPU::writeWord(int wval)
{
  digitalWrite(_cs, LOW);
  _u.wval[0] = wval;
  SPI.transfer(_u.bval[1]);
  SPI.transfer(_u.bval[0]);
  digitalWrite(_cs, HIGH);
}

void UMFPU::writeLong(long lval)
{
  digitalWrite(_cs, LOW);
  _u.lval = lval;
  SPI.transfer(_u.bval[3]);
  SPI.transfer(_u.bval[2]);
  SPI.transfer(_u.bval[1]);
  SPI.transfer(_u.bval[0]);
  digitalWrite(_cs, HIGH);
}

void UMFPU::writeFloat(float fval)
{
  digitalWrite(_cs, LOW);
  _u.fval = fval;
  SPI.transfer(_u.bval[3]);
  SPI.transfer(_u.bval[2]);
  SPI.transfer(_u.bval[1]);
  SPI.transfer(_u.bval[0]);
  digitalWrite(_cs, HIGH);
}

void UMFPU::writeString(char *s)
{
  digitalWrite(_cs, LOW);
  while (*s) SPI.transfer(*s++);
  SPI.transfer(0);
  digitalWrite(_cs, HIGH);
}


//------------------- read --------------------------------------------------

byte UMFPU::read(void)
{
  digitalWrite(_cs, LOW);
  readDelay();
  _u.bval[0] = SPI.transfer(0);
  digitalWrite(_cs, HIGH);
  return _u.bval[0];
}

int UMFPU::readWord(void)
{
  digitalWrite(_cs, LOW);
  readDelay();
  _u.bval[1] = SPI.transfer(0);
  delayMicroseconds(1);
  _u.bval[0] = SPI.transfer(0);
  digitalWrite(_cs, HIGH);
  return _u.wval[0];
}

long UMFPU::readLong(void)
{
  digitalWrite(_cs, LOW);
  readDelay();
  _u.bval[3] = SPI.transfer(0);
  delayMicroseconds(1);
  _u.bval[2] = SPI.transfer(0);
  delayMicroseconds(1);
  _u.bval[1] = SPI.transfer(0);
  delayMicroseconds(1);
  _u.bval[0] = SPI.transfer(0);
  digitalWrite(_cs, HIGH);
  return _u.lval;
}

float UMFPU::readFloat(void)
{
  digitalWrite(_cs, LOW);
  readDelay();
  _u.bval[3] = SPI.transfer(0);
  delayMicroseconds(1);
  _u.bval[2] = SPI.transfer(0);
  delayMicroseconds(1);
  _u.bval[1] = SPI.transfer(0);
  delayMicroseconds(1);
  _u.bval[0] = SPI.transfer(0);
  digitalWrite(_cs, HIGH);
  return _u.fval;
}

char *UMFPU::readString(char *s)
{
	return readString(s, READSTR);
}

char *UMFPU::readString(char *s, byte opcode)
{
  char *sb, c;

  wait();
  if (opcode != READSEL) opcode = READSTR;
  write(opcode);
  readDelay();

  digitalWrite(_cs, LOW);
  readDelay();
  sb = s;
  while (1) {
    c = SPI.transfer(0);
    if (!c) break;
    *s++ = c;
  }
  *s = '\0';
  digitalWrite(_cs, HIGH);
  return sb;
}

byte UMFPU::readStatus(void)
{
  wait();
  write(READSTATUS);
  return read();
}

//------------------- readDelay ---------------------------------------------

void UMFPU::readDelay(void)
{
  delayMicroseconds(15);
}

//---------- preinstantiate FPU object --------------------------------------

UMFPU Fpu = UMFPU();
