#include "temperatureUtils.h"
#include "timerUtils.h"

namespace TemperatureLibrary
{
  unsigned long lastSampleTime = 0;
  const unsigned long sampleInterval = 1000; // 1 second

  int TemperatureReader::initTemperatureSensor()
  {
    SerialUSB.println("Initializing Internal Temperature Sensor...");
    tempZero.init();
    SerialUSB.println("Temperature Sensor Ready");
    return 0;
  }

  float TemperatureReader::calculateAverage()
  {
    // unsigned long now = millis();
    // bool secondElapsed
    // // Only sample once per second
    // if (now - lastSampleTime >= sampleInterval)
    // {
    // lastSampleTime = now;

    float temperature = tempZero.readInternalTemperature();
    tempStack.push(temperature);

    // Maintain max of 5 samples in stack
    if ((int)tempStack.size() > MAX_SAMPLES)
    {
      // Remove oldest: transfer to temporary stack to discard bottom
      std::stack<float> tempReversed;
      // Reverse order
      while (!tempStack.empty())
      {
        tempReversed.push(tempStack.top());
        tempStack.pop();
      }
      // Pop the oldest (which is now at top of reversed)
      if (!tempReversed.empty())
      {
        tempReversed.pop();
      }
      // Restore back
      while (!tempReversed.empty())
      {
        tempStack.push(tempReversed.top());
        tempReversed.pop();
      }
    }

    // Compute average without disturbing stack
    if (tempStack.empty())
    {
      return NAN; // No samples yet
    }

    float sum = 0.0;
    int count = 0;
    std::stack<float> copyStack = tempStack; // copy for iteration

    while (!copyStack.empty())
    {
      sum += copyStack.top();
      copyStack.pop();
      count++;
    }

    return sum / count;
  }
}
