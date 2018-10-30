/*
 * NIBEConnect implements communication with a NIBE 360p heatpump.
 * and possibly other models as well.
 *
 * Copyright (C) 2018 Fredrik Viklund
 * fredrik@viklund.se
 */

#ifndef NIBEConnect_h
#define NIBEConnect_h

#include <stdint.h>
#include <Arduino.h> // Really required? What needs to be included for Serial and pause()?
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <NIBERegisters.h>

// Global types and variables

// Class declaration
class NIBEConnect {
  typedef void (*pStateFunc)();

public:
  NIBEConnect(SoftwareSerial &connection) : rs485(connection) {}
  int connect();
  int loop();

private:
  // TODO - forward declare NIBERegisters and avoid including the .h
  SoftwareSerial &rs485;
  NIBERegisters myHeatpump;
  typedef enum {
    ST_idle,
    ST_addressbegun,
    ST_addressed,
    ST_commandreceived,
    ST_getsender,
    ST_getlength,
    ST_gettelegram,
    ST_checktelegram
  } ProtoStates;

  ProtoStates currentState = ST_idle;

  uint8_t inbyte, inpar, chksum, msglen, bytecount, paramlen;
  int16_t paramno, paramval, paramtype;

  typedef char Telegram[30];  // Empirical value for Nibe 360P
  Telegram rxtg, txtg; // telegrams received and for transmission
  String rxstr;
};

#endif