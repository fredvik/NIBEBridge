/*
 * NIBERegisters holds the content of information sent to or from the NIBE
 * heatpump.
 *
 * Copyright (C) 2018 Fredrik Viklund
 * fredrik@viklund.se
 */

#ifndef NIBERegisters_h
#define NIBERegisters_h

#include <stdint.h>
#include <string.h>

int getParamLen(int paramno);

class NIBERegisters {
private:
  typedef enum VarType {
    SIGNEDINT,
    UNSIGNEDINT,
    BIT,
    BITFIELD
  };
  struct NIBEregister {
    uint8_t modbusNr;
//    String nameSV;
    bool   writeable;
    VarType type;
    uint16_t bitmask;
    // TODO - resolution, factor, min, max - store efficiently as sparse array?
  } NIBEreg[65];      // TODO - fix dynamic allocation for different models

public:
  int setRegister();
  int getRegister();
  int getParamLen(int paramno);
  // registernummer i NIBEs lista startar på 1 men adresseras med start på 0,
  // dvs för register 1 skickar man adress 0 på modbus
  // Parameter 4 = frånluftstemperatur = modbus reg nr 5
};

#endif
