#include "watchdogTimer.h"
#include "flashStoragelib.h"
#include "errorCodes.h"
#include <Arduino.h>

namespace WatchDogTimerLibrary {

    volatile ErrorLog WatchDogTimer::lastError = {0, 0, 0};
    FlashStorageLibrary::FlashStorage flashStorage;

    int WatchDogTimer::initWatchDogTimer(float period_ms) {
        SerialUSB.println("Begin initialize WatchdogTimer");
        if (period_ms < 1000) {
            SerialUSB.println("Error: WDT period too short!");
            return -1;
        }

        // Set up GCLK2 = 32 kHz clock
        GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(3);
        GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) |
                            GCLK_GENCTRL_GENEN |
                            GCLK_GENCTRL_SRC_OSCULP32K |
                            GCLK_GENCTRL_DIVSEL;
        while (GCLK->STATUS.bit.SYNCBUSY);

        // Connect WDT to GCLK2
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
                            GCLK_CLKCTRL_CLKEN |
                            GCLK_CLKCTRL_GEN_GCLK2;

        // Disable WDT before configuration
        WDT->CTRL.reg = 0;
        while (WDT->STATUS.bit.SYNCBUSY);

        // Configure timeout period
        WDT->CONFIG.bit.PER = getWatchDogPredivider(period_ms);

        // Enable Early Warning interrupt
        WDT->INTENSET.bit.EW = 1;
        NVIC_EnableIRQ(WDT_IRQn);

        // Disable window mode
        WDT->CTRL.bit.WEN = 0;

        // Enable WDT
        WDT->CTRL.bit.ENABLE = 1;
        while (WDT->STATUS.bit.SYNCBUSY);

        SerialUSB.println("WatchdogTimer ready");
        return 0;
    }

    void WatchDogTimer::feed() {
        WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
        while (WDT->STATUS.bit.SYNCBUSY);
    }

    int WatchDogTimer::getWatchDogPredivider(int period_ms) {
        float t_s = period_ms / 1000.0;
        float cycles = t_s * clock_frequency;
        int per = round(log2(cycles)) - 3; // PER=0 → 8 cycles
        if (per < 0) per = 0;
        if (per > 11) per = 11;
        return per;
    }

    void WatchDogTimer::logError() {
        if ((lastError.statusA == 0 && lastError.statusB == 0) || lastError.timestamp == 0) return;

        if (lastError.statusA & (1 << 2)) {
            flashStorage.writeFlashStorage(ErrorCode::DsuBusError);
        } else if (lastError.statusA & (1 << 3)) {
            flashStorage.writeFlashStorage(ErrorCode::DsuFailure);
        } else if (lastError.statusA & (1 << 4)) {
            flashStorage.writeFlashStorage(ErrorCode::DsuProtectionError);
        } else if (lastError.statusB & (1 << 2) || lastError.statusB & (1 << 3)) {
            flashStorage.writeFlashStorage(ErrorCode::Communication);
        }
    }
}

// ISR: Runs when Early Warning occurs
// Basically the last chance before the WDT resets
extern "C" void WDT_Handler(void) {
    // Clear interrupt flag
    WDT->INTFLAG.bit.EW = 1;

    // Capture DSU status into the log
    WatchDogTimerLibrary::WatchDogTimer::lastError.timestamp = millis();
    WatchDogTimerLibrary::WatchDogTimer::lastError.statusA = REG_DSU_STATUSA;
    WatchDogTimerLibrary::WatchDogTimer::lastError.statusB = REG_DSU_STATUSB;
    WatchDogTimerLibrary::WatchDogTimer watchdog;
    watchdog.logError();
}
