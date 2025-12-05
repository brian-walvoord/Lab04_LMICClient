#include <RTCZero.h>
#include <realTimeCounter.h>

namespace RealTimeCounterLibrary {

    RTCZero realTimeCounter;

    void RealTimeCounter::initRTC() {
        SerialUSB.println("Begin init real time counter");
        realTimeCounter.begin();

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
