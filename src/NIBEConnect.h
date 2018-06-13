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
  SoftwareSerial &rs485;
  NIBEConnect(SoftwareSerial &connection) : rs485(connection) {}
  // Is the destructor needed or useful?
  int connect();
  int action();

private:
// TODO - how to add declare functions here without defining them?
  NIBERegisters myHeatpump;
  typedef enum {
    ST_idle,
    ST_addressbegun,
    ST_addressed,
    ST_commandreceived,
    ST_getsender,
    ST_getlength,
    ST_gettelegram,
    ST_checktelegram,
    ST_endoftelegram,
    ST_error
  } ProtoStates;

  ProtoStates currentState = ST_idle;

  uint8_t inbyte, inpar, chksum, msglen, bytecount, paramlen;
  int16_t paramno, paramval, paramtype;

  typedef char Telegram[30];  // TODO fix char type to uint8_t
  Telegram rxtg, txtg; // telegrams received and for transmission
  String rxstr;
};

#endif