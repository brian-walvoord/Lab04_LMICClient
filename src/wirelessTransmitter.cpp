
/*
  Both the TX and RX ProRF boards will need a wire antenna. We recommend a 3" piece of wire.
  This example is a modified version of the example provided by the Radio Head
  Library which can be found here:
  www.github.com/PaulStoffregen/RadioHead
*/ 

#include <SPI.h>
#include <RH_RF95.h>
#include "wirelessTransmitter.h"
#include <realTimeCounter.h>

namespace WirelessTransmitterLibrary {
    //long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received

    // The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
    // anywhere in the range of 902-928MHz in the Americas.
    // Europe operates in the frequencies 863-870, center frequency at 868MHz.
    // This works but it is unknown how well the radio configures to this frequency:
    //float frequency = 864.1;

    static uint8_t NODE_ID; // Each node must have a unique ID
    int WirelessTransmitter::packetCounter = 0;

    /**
     * Packet structure:
     * 1. Node ID
     * 2. Packet ID
     * 3. Timestamp
     * 4. Payload
     */

    int WirelessTransmitter::initWirelessTransmitter(uint8_t nodeId) {

        NODE_ID = nodeId;
        SerialUSB.println("Begin initialize Wireless Comms");
        // Return -1 if initialization went wrong, handle in main
        if (rf95.init() == false) {
            SerialUSB.println("Error init RF95");
            return -1;
        }
        SerialUSB.println("Transmitter ready");
        SerialUSB.print("Setting Frequency ");
        SerialUSB.println(frequency);
        rf95.setFrequency(frequency);
        SerialUSB.println("Setting Transmit Power to 20");
        rf95.setTxPower(20);
        // Set my node address to 1 (change after talking with group)
        rf95.setThisAddress(NODE_ID);

        digitalWrite(LED, true);
        delay(500);
        digitalWrite(LED, false);
        delay(500);
        digitalWrite(LED, true);
        delay(500);
        digitalWrite(LED, false);
        SerialUSB.println("Wireless Comms ready");

        return 0;
    }

    int WirelessTransmitter::sendPacket(const char *msg, int HEADER_ID) {
        // if (rf95.isChannelActive()) {
        //     SerialUSB.println("Channel busy");
        //     return 0; // Return 1 to indicate channel is busy
        // }
        Payload payload;
        char timeStamp[33];
        RealTimeCounterLibrary::RealTimeCounter rtc;
        rtc.generateTimeStampString(timeStamp, sizeof(timeStamp));
        strncpy(payload.message, msg, sizeof(payload.message) - 1);
        payload.message[sizeof(payload.message) - 1] = '\0';
        strncpy(payload.timestamp, timeStamp, sizeof(payload.timestamp) - 1);

        rf95.setHeaderFrom(NODE_ID);                    // source node
        if(HEADER_ID != -1)
        {
            rf95.setHeaderTo(HEADER_ID);                         // specific node
        }
        rf95.setHeaderId(packetCounter++);
        //rf95.setHeaderFlags(0x80);               // request ACK

        rf95.send(reinterpret_cast<uint8_t*>(&payload), sizeof(payload));
        rf95.waitPacketSent();

        SerialUSB.print("Sent packet with message: ");
        SerialUSB.println(payload.message);
        return 1;
    }

    int WirelessTransmitter::receivePacket() {

        if (!rf95.available()) return 0;

        SerialUSB.println("Incoming data");
        Payload payload;
        uint8_t len = sizeof(payload);

        if (rf95.recv(reinterpret_cast<uint8_t*>(&payload), &len)) {

            if(rf95.headerFlags() & 0x00)
            {
                SerialUSB.println("Received error packet");
            }
            SerialUSB.print("Received at time: ");
            SerialUSB.println(payload.timestamp);
            SerialUSB.print("Message: ");
            SerialUSB.println(payload.message);
            SerialUSB.println(rf95.headerFrom());
        }
        return 1;
    }
}
