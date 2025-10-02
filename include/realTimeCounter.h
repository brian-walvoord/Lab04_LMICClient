#ifndef REAL_TIME_COUNTER
#define REAL_TIME_COUNTER

namespace RealTimeCounterLibrary {
    
    class RealTimeCounter {

        public:
            /**
             * Initializes the Real Time Counter
             */
            void initRTC();

            /**
             * Generates a timestamp string with the following format
             * YYYY-MM-DDThh:mm:ss
             */
            void generateTimeStampString(char* timeStamp, size_t size);

            uint8_t getHours();

            uint8_t getMinutes();

            uint8_t getSeconds();

        private:
            // Starts RTC at 2025-09-25T00:00:00
            const uint8_t startYear = 25;
            const uint8_t startMonth = 9;
            const uint8_t startDay = 25;
            const uint8_t startHours = 0;
            const uint8_t startMinutes = 0;
            const uint8_t startSeconds = 0;
    };
}

#endif