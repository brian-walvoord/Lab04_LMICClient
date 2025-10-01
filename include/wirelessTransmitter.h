#ifndef WIRELESS_TRANSMITTER
#define WIRELESS_TRANSMITTER 

#include <RH_RF95.h>

namespace WirelessTransmitterLibrary {
    // The packet we send and receive
    struct Payload {
        char timestamp[33];     // milliseconds since boot
        char message[32];   // up to 31 chars + null
    };
    class WirelessTransmitter {
        public:

            // Always pin 12 for CS pin and 6 for RST pin
            RH_RF95 rf95 = RH_RF95(12, 6);

            /**
             * Function initializes the RF95 receiver/transceiver
             */
            int initWirelessTransmitter(uint8_t NODE_ID);
            
            /**
             * Sends a packet with payload 'message'
             */
            int sendPacket(const char *msg, int HEADER_ID=-1);
            
            /**
             * Prints out the packet contents if received
             */
            int receivePacket();
            static int packetCounter;
        private:
            // Status LED
            static constexpr uint8_t LED = 13;
            // Broadcast frequency
            static constexpr float frequency = 920; 
    };
}

#endif