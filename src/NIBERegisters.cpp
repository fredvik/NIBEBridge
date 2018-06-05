/*
 * NIBERegisters holds the content of information sent to or from the NIBE
 * heatpump.
 *
 * Copyright (C) 2018 Fredrik Viklund
 * fredrik@viklund.se
 */

#include <NIBERegisters.h>
#include <HardwareSerial.h>

//int NIBERegisters::getParamLen(int paramno) {
int getParamLen(int paramno) {
  /*
  if ((paramno == 20) || (paramno == 27)) {
    return 1;  // single-byte registers
  }
  else {
    return 2;
  } */

  switch (paramno) {
  case 0x01:
  case 0x02:
  case 0x03:
  case 0x04:
  case 0x05:
  case 0x06:
  case 0x07:
  case 0x08:
  case 0x09:
  case 0x0a:
  case 0x0e:
  case 0x0f:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x25:
    return 2;
    break;
  case 0x00:
  case 0x0b:
  case 0x0c:
  case 0x0d:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x13:
  case 0x1a:
  case 0x1e:
  case 0x1f:
  case 0x20:
  case 0x21:
  case 0x22:
  case 0x23:
  case 0x24:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
    return 1;
    break;
  default:
    Serial.printf("Unknown cmd length for %i\n", paramno);
    return 2;
  }
}
