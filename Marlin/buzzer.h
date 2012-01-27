#ifndef __BUZZERH
#define __BUZZERH

#include "Marlin.h"

#if (defined(BUZZER_PIN) && (BUZZER_PIN > -1))  
  void buzzer_init();
  void playTone(int tone, int duration);
  void playNote(char note, int duration);
  
#else 
  FORCE_INLINE void buzzer_init() {};
  FORCE_INLINE void playTone(int tone, int duration) {};
  FORCE_INLINE void playNote(char note, int duration) {};
#endif 

#endif
