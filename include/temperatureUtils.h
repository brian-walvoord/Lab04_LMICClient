#ifndef TEMPERATURE_LIBRARY_H
#define TEMPERATURE_LIBRARY_H
#include <Arduino.h> // gives uint8_t, uint16_t, uint32_t, etc.
#include <TemperatureZero.h>
#undef min
#undef max
#include <stack>

// TemperatureZero tempZero = TemperatureZero();

namespace TemperatureLibrary
{
  class TemperatureReader
  {
  public:
    int initTemperatureSensor();
    float calculateAverage();

  private:
    TemperatureZero tempZero = TemperatureZero();
    std::stack<float> tempStack;
    static const int MAX_SAMPLES = 5; // 5 second
  };
}

#endif
