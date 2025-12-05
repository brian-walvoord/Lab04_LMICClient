
/*******************************************************************************
 * Copyright (c) 2015 Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI Corporation
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example transmits data on hardcoded channel and receives data
 * when not transmitting. Running this sketch on two nodes should allow
 * them to communicate.
 *******************************************************************************/

#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "temperatureUtils.h"
#include "realTimeCounter.h"

RealTimeCounterLibrary::RealTimeCounter rtc;
TemperatureLibrary::TemperatureReader tempSensor;
float averageTemp = 0;
const uint8_t NODE_ID = 4; // Unique ID for this node

// we formerly would check this configuration; but now there is a flag,
// in the LMIC, LMIC.noRXIQinversion;
// if we set that during init, we get the same effect.  If
// DISABLE_INVERT_IQ_ON_RX is defined, it means that LMIC.noRXIQinversion is
// treated as always set.
//
// #if !defined(DISABLE_INVERT_IQ_ON_RX)
// #error This example requires DISABLE_INVERT_IQ_ON_RX to be set. Update \
    //        lmic_project_config.h in arduino-lmic/project_config to set it.
// #endif

// How often to send a packet. Note that this sketch bypasses the normal
// LMIC duty cycle limiting, so when you change anything in this sketch
// (payload length, frequency, spreading factor), be sure to check if
// this interval should not also be increased.
// See this spreadsheet for an easy airtime and duty cycle calculator:
// https://docs.google.com/spreadsheets/d/1voGAtQAjC1qBmaVuP1ApNKs1ekgUjavHuVQIXyYSvNc
#define TX_INTERVAL 60000 // Delay between each message in milliseconds.

// Pin mapping for SAMD21
const lmic_pinmap lmic_pins = {
    .nss = 12, // RFM Chip Select
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 7,           // RFM Reset
    .dio = {6, 10, 11}, // RFM Interrupt, RFM LoRa pin, RFM LoRa pin
};

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmoc/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui(u1_t *buf) {}
void os_getDevEui(u1_t *buf) {}
void os_getDevKey(u1_t *buf) {}

void onEvent(ev_t ev)
{
}

osjob_t tempCheckJob;
osjob_t txjob;
osjob_t timeoutjob;
static void tx_func(osjob_t *job);

// Transmit the given string and call the given function afterwards
void tx(const char *str, osjobcb_t func)
{
  os_radio(RADIO_RST); // Stop RX first
  delay(1);            // Wait a bit, without this os_radio below asserts, apparently because the state hasn't changed yet
  LMIC.dataLen = 0;
  while (*str)
    LMIC.frame[LMIC.dataLen++] = *str++;
  LMIC.osjob.func = func;
  os_radio(RADIO_TX);
  SerialUSB.println("TX");
}

// Enable rx mode and call func when a packet is received
void rx(osjobcb_t func)
{
  LMIC.osjob.func = func;
  LMIC.rxtime = os_getTime(); // RX _now_
  // Enable "continuous" RX (e.g. without a timeout, still stops after
  // receiving a packet)
  os_radio(RADIO_RXON);
  SerialUSB.println("RX");
}

static void rxtimeout_func(osjob_t *job)
{
  digitalWrite(LED_BUILTIN, LOW); // off
}

static void rx_func(osjob_t *job)
{
  // Blink once to confirm reception and then keep the led on
  digitalWrite(LED_BUILTIN, LOW); // off
  delay(10);
  digitalWrite(LED_BUILTIN, HIGH); // on

  // Timeout RX (i.e. update led status) after 3 periods without RX
  os_setTimedCallback(&timeoutjob, os_getTime() + ms2osticks(3 * TX_INTERVAL), rxtimeout_func);

  // Reschedule TX so that it should not collide with the other side's
  // next TX
  os_setTimedCallback(&txjob, os_getTime() + ms2osticks(TX_INTERVAL / 2), tx_func);

  SerialUSB.print("Got ");
  SerialUSB.print(LMIC.dataLen);
  SerialUSB.println(" bytes");
  SerialUSB.write(LMIC.frame, LMIC.dataLen);
  SerialUSB.println();

  // Restart RX
  rx(rx_func);
}

static void txdone_func(osjob_t *job)
{
  // rx(rx_func);
}

// log text to USART and toggle LED
static void tx_func(osjob_t *job)
{
  char buffer[18];
  uint8_t hours = rtc.getHours();
  uint8_t minutes = rtc.getMinutes();
  uint8_t seconds = rtc.getSeconds();
  int whole = (int)averageTemp;
  int decimal = (int)((averageTemp - whole) * 100);
  snprintf(buffer, sizeof(buffer), "%d,%02d:%02d:%02d,%d.%02d",
           NODE_ID,
           hours, minutes, seconds,
           whole, decimal);
  SerialUSB.println("-----------------------------------------");
  SerialUSB.print("Payload: ");
  SerialUSB.println(buffer);
  tx(buffer, txdone_func);
  os_setTimedCallback(job, os_getTime() + ms2osticks(TX_INTERVAL + random(500)), tx_func);
  // reschedule job every TX_INTERVAL (plus a bit of random to prevent
  // systematic collisions), unless packets are received, then rx_func
  // will reschedule at half this time.
}

void update_temps()
{
  averageTemp = tempSensor.calculateAverage();
  // SerialUSB.println("Updating temps...");
}

static void tempSecondCallback(osjob_t *job)
{
  update_temps();

  // Reschedule the job in 1 second (1 second = 1 * os_getTime_ticsPerSec())
  os_setTimedCallback(&tempCheckJob, os_getTime() + sec2osticks(1), tempSecondCallback);
}

// application entry point
void setup()
{
  SerialUSB.begin(115200);
  // while(!SerialUSB);
  delay(1000);
  SerialUSB.println("Starting");
  //  #ifdef VCC_ENABLE
  //  // For Pinoccio Scout boards
  //  pinMode(VCC_ENABLE, OUTPUT);
  //  digitalWrite(VCC_ENABLE, HIGH);
  //  delay(1000);
  //  #endif

  pinMode(LED_BUILTIN, OUTPUT);

  // initialize runtime env
  os_init();

  // this is automatically set to the proper bandwidth in kHz,
  // based on the selected channel.
  uint32_t uBandwidth;
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  LMIC.freq = 921900000; // change this for assigned frequencies, match with int freq in loraModem.h
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  uBandwidth = 125;
  LMIC.datarate = US915_DR_SF7; // DR4
  LMIC.txpow = 21;

  // disable RX IQ inversion
  LMIC.noRXIQinversion = true;

  // This sets CR 4/5, BW125 (except for EU/AS923 DR_SF7B, which uses BW250)
  LMIC.rps = updr2rps(LMIC.datarate);

  SerialUSB.print("Frequency: ");
  SerialUSB.print(LMIC.freq / 1000000);
  SerialUSB.print(".");
  SerialUSB.print((LMIC.freq / 100000) % 10);
  SerialUSB.print("MHz");
  SerialUSB.print("  LMIC.datarate: ");
  SerialUSB.print(LMIC.datarate);
  SerialUSB.print("  LMIC.txpow: ");
  SerialUSB.println(LMIC.txpow);

  // This sets CR 4/5, BW125 (except for DR_SF7B, which uses BW250)
  LMIC.rps = updr2rps(LMIC.datarate);

  // disable RX IQ inversion
  LMIC.noRXIQinversion = true;

  SerialUSB.println("Started");
  SerialUSB.flush();

  rtc.initRTC();
  tempSensor.initTemperatureSensor();

  // setup initial job
  os_setCallback(&txjob, tx_func);
  os_setTimedCallback(&tempCheckJob, os_getTime() + sec2osticks(1), tempSecondCallback);
}

void loop()
{
  // execute scheduled jobs and events
  os_runloop_once();
}
