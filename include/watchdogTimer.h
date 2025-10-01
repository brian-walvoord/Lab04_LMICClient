#ifndef WATCHDOGTIMER_H
#define WATCHDOGTIMER_H

#include <cstdint>

namespace WatchDogTimerLibrary {

    struct ErrorLog {
        uint32_t timestamp;   // system millis when error occurred
        uint8_t statusA;      // DSU STATUSA register
        uint8_t statusB;      // DSU STATUSB register
    };

    class WatchDogTimer {
    public:
        /**
         * Initialize the Watchdog Timer with a timeout period.
         * @param period_ms Timeout period in milliseconds (default 8000).
         * @return 0 if successful, -1 on error.
         */
        int initWatchDogTimer(float period_ms = 8000);

        /**
         * Feed (clear) the watchdog so it does not reset the system.
         */
        void feed();

        /**
         * Calculate the predivider value for CONFIG.PER.
         */
        int getWatchDogPredivider(int period_ms);

        // Last error log captured in ISR
        static volatile ErrorLog lastError;

        void logError();
    private:
        // 32 kHz clock source
        static constexpr float clock_frequency = 2048.0;
    };
}

#endif
