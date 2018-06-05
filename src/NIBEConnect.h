/*
 * NIBEConnect implements communication with a NIBE 360p heatpump.
 * and possibly other models as well.
 *
 * Copyright (C) 2018 Fredrik Viklund
 * fredrik@viklund.se
 */

#ifndef NIBEConnect_h
#define NIBEConnect_h

#include <Arduino.h> // Really required? What needs to be included for Serial and pause()?
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <stdint.h>

// typedef pStateFunc *(*StateFunc)();

class NIBEConnect {
  typedef void (*pStateFunc)();

public:
  SoftwareSerial &rs485;
  NIBEConnect(SoftwareSerial &connection) : rs485(connection) {}
  // virtual ~NIBEConnect(); // Is the destructor needed or useful?
  int connect();
  int action();

private:
  typedef enum {
    ST_idle,
    ST_addressbegun,
    ST_addressed,
    ST_commandreceived,
    ST_getsender,
    ST_getlength,
    ST_getregisterhigh,
    ST_getregisterlow,
    ST_getvaluehigh,
    ST_getvaluelow,
    ST_checktelegram,
    ST_endoftelegram,
    ST_error
  } ProtoStates;

  ProtoStates currentState = ST_idle;

  uint8_t inbyte, inpar, chksum, msglen, numbytes, paramlen;
  int16_t paramno, paramval;
};

#endif