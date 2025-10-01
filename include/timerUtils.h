#ifndef TIMER_UTILS_H
#define TIMER_UTILS_H

#include <Arduino.h>

namespace TimerLibrary
{
  extern volatile bool tickFlag;  // set once per second by timer ISR

  void startTimer(int frequencyHz);
  void setTimerFrequency(TcCount16* TC, int frequencyHz);

  // Returns true if a new second tick occurred since last check
  bool secondElapsed();
}

#endif
