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

typedef enum VarType { SBYTE, UBYTE, SINT, UINT, BITFIELD } VarType;
// TODO - clean up VarType that is defined twice - here and in the class

#include <Paraminfo.inc>

class NIBERegisters {
private:
  typedef char Telegram[30];
  typedef enum { SBYTE, UBYTE, SINT, UINT, BITFIELD } VarType;
  struct stored {
    float   value;
    unsigned long lastWrite;    
  } stored[65]; // TODO - fix dynamic allocation for different models
  
  int16_t paramno, paramtype;
  float paramval;
  uint8_t msglen, paramlen;

public:
  // registernummer i NIBEs lista startar på 1 men adresseras med start på 0,
  // dvs för register 1 skickar man adress 0 på modbus
  // Parameter 4 = frånluftstemperatur = modbus reg nr 5
  int storeTg(Telegram tg);
  int setRegister();
  int getRegister();
  int getParamLen(int paramno);
  float getParamFactor(int paramno);
  VarType getParamType(int paramno);

};

#endif
