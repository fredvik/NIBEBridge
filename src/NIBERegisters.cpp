/*
 * NIBERegisters holds the content of information sent to or from the NIBE
 * heatpump.
 *
 * Copyright (C) 2018 Fredrik Viklund
 * fredrik@viklund.se
 */

#include <HardwareSerial.h>
#include <NIBERegisters.h>

// int NIBERegisters::getParamLen(int paramno) {
int NIBERegisters::getParamLen(int paramno) {
  if (params360p[paramno].flags & BYTE) {
    return 1;
  } else {
    return 2;
  }
}

NIBERegisters::VarType NIBERegisters::getParamType(int paramno) {
  uint8_t flag = params360p[paramno].flags;
  if (flag & SIGNED) {
    if (flag & INT) {
      return SINT;
    } else {
      return SBYTE;
    }
  } else {
    if (flag & INT) {
      return UINT;
    } else {
      return UBYTE;
    }
  }
  Serial.printf("\nFailed type determination of param no %d with flag %02X.\n",
                paramno, flag);
  return UINT; // Safe
}

float NIBERegisters::getParamFactor(int paramno) {
  real32_t fact = params360p[paramno].factor;
  if (fact < 0) {
    return (1 / -fact);
  } else {
    return fact;
  }
}

int NIBERegisters::storeTg(Telegram tg) {
  int count = 1;
  float temp;
  msglen = tg[0];
  // Serial.printf("msglen = %d\n", msglen);
  // Serial.printf("String length: %i\n", tgstr.length());
  while (count < msglen - 3) {
    // May be scrap at the end, must be >2 chars left to be meaningful
    paramno = ((int16_t)tg[count]) << 8 | (int16_t)tg[count + 1];
    count += 2;
    paramlen = getParamLen(paramno);
    paramtype = getParamType(paramno);
    Serial.printf("\nParam %d (%02X), type %d, length %d ", paramno, paramno, paramtype, paramlen);
    switch (paramtype) {
    case SBYTE:
      temp = (float)((int8_t)tg[count]) * getParamFactor(paramno);
      Serial.printf("SBYTE = %.1f", temp);
      count++;
      break;

    case UBYTE:
      temp = (float)tg[count] * getParamFactor(paramno);
      Serial.printf("UBYTE = %.1f", temp);
      count++;
      break;

    case SINT:
      temp = (float)(((int16_t)(int8_t)tg[count]) << 8 | (int8_t)tg[count + 1]) * getParamFactor(paramno);
      Serial.printf("SINT  = %.1f", temp);
      count += 2;
      break;

    case UINT:
      temp = (float)((uint16_t)tg[count] << 8 | (uint16_t)tg[count + 1]) * getParamFactor(paramno);
      Serial.printf("UINT  = %.1f", temp);
      count += 2;
      break;
    }
  }
  return 0; // TODO - add meaning to return value
}