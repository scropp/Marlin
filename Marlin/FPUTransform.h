/*
  FPUTransform.h - dynamic bed leveling
  
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
 #ifndef __FPUTRANSFORM

#define __FPUTRANSFORM
#include "Marlin.h"
#include "z_probe.h"

#if defined(UMFPUSUPPORT) && (UMFPUSUPPORT > -1) 
  extern bool FPUEnabled;
  void FPUTransform_init();
  void FPUEnable();
  void FPUReset();
  void FPUDisable();
  void FPUTransform_determineBedOrientation(float Z1_in, float Z2_in, float Z4_in);
  void FPUTransform_transformDestination(); 

#else //no UMFPU SUPPORT
  FORCE_INLINE void FPUTransform_init() {};

#endif //UMFPUSUPPORT

#endif //__FPUTRANSFORM
