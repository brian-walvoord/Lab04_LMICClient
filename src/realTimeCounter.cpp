#include <RTCZero.h>
#include <realTimeCounter.h>

namespace RealTimeCounterLibrary {

    RTCZero realTimeCounter;

    void RealTimeCounter::initRTC() {
        SerialUSB.println("Begin init real time counter");
        realTimeCounter.begin();

        // Time definitions in header file
        SerialUSB.print("Setting start year to ");
        SerialUSB.println(startYear);
        realTimeCounter.setYear(startYear);
        SerialUSB.print("Setting start month to");
        SerialUSB.println(startMonth);
        realTimeCounter.setMonth(startMonth);
        SerialUSB.print("Setting start day to ");
        SerialUSB.println(startDay);
        realTimeCounter.setDay(startDay);

        SerialUSB.print("Setting start hours to ");
        SerialUSB.println(startHours);
        realTimeCounter.setHours(startHours);
        SerialUSB.print("Setting start minutes to ");
        SerialUSB.println(startMinutes);
        realTimeCounter.setMinutes(startMinutes);
        SerialUSB.print("Setting start seconds ");
        SerialUSB.println(startSeconds);
        realTimeCounter.setSeconds(startSeconds);

        SerialUSB.println("Done init real time counter");
    }

    /**
     * char timeStamp[33];
     * generateTimeStampString(timeStamp, sizeof(timeStamp));
     * 
     * ^ Will populate timeStamp with correct value
     */
    void RealTimeCounter::generateTimeStampString(char* timeStamp, size_t size) {
        snprintf(timeStamp, size, "20%02d-%02d-%02dT%02d:%02d:%02d\n",
           realTimeCounter.getYear(),
           realTimeCounter.getMonth(),
           realTimeCounter.getDay(),
           realTimeCounter.getHours(),
           realTimeCounter.getMinutes(),
           realTimeCounter.getSeconds());
    }

    uint8_t RealTimeCounter::getHours() {
        return realTimeCounter.getHours();
    }

    uint8_t RealTimeCounter::getMinutes() {
        return realTimeCounter.getMinutes();
    }

    uint8_t RealTimeCounter::getSeconds() {
        return realTimeCounter.getSeconds();
    }
}
