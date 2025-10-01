#include "timerUtils.h"

namespace TimerLibrary
{
  volatile bool tickFlag = false;

  void setTimerFrequency(TcCount16 *TC, int frequencyHz)
  {
    int compareValue = (48000000 / (1024 * frequencyHz)) - 1;
    TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
    TC->CC[0].reg = compareValue;
    while (TC->STATUS.bit.SYNCBUSY)
      ;
  }

  // Timer setup
  void startTimer(int frequencyHz)
  {
    TcCount16 *TC;

      REG_GCLK_CLKCTRL = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3);
      while (GCLK->STATUS.bit.SYNCBUSY)
        ;
      TC = (TcCount16 *)TC3;
      NVIC_EnableIRQ(TC3_IRQn);

    TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;
    while (TC->STATUS.bit.SYNCBUSY)
      ;

    // 16-bit, MFRQ mode, prescaler 1024
    TC->CTRLA.reg = (TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_PRESCALER_DIV1024);
    while (TC->STATUS.bit.SYNCBUSY)
      ;

    setTimerFrequency(TC, frequencyHz);

    TC->INTENSET.reg = 0;
    TC->INTENSET.bit.OVF = 1; // use overflow interrupt instead of match interrupt

    TC->CTRLA.reg |= TC_CTRLA_ENABLE;
    while (TC->STATUS.bit.SYNCBUSY)
      ;
  }

  bool secondElapsed()
  {
    if (tickFlag)
    {
      tickFlag = false;
      return true;
    }
    return false;
  }
}

// Interrupt handlers
void TC3_Handler() {
  TcCount16* TC = (TcCount16*)TC3;
  if (TC->INTFLAG.bit.OVF) {
    TC->INTFLAG.bit.OVF = 1;
    // set flag once per second
    TimerLibrary::tickFlag = true;    
  }
}