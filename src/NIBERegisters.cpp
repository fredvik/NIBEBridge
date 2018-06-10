/*
 * NIBERegisters holds the content of information sent to or from the NIBE
 * heatpump.
 *
 * Copyright (C) 2018 Fredrik Viklund
 * fredrik@viklund.se
 */

#include <HardwareSerial.h>
#include <NIBERegisters.h>
#include <string>

// int NIBERegisters::getParamLen(int paramno) {
int NIBERegisters::getParamLen(int paramno) {
  if (params360p[paramno].flags & INT) {
    return 2;
  } else {
    return 1;
  }
}

int NIBERegisters::getParamType(int paramno) {
  uint8_t flag = params360p[paramno].flags;
  if ((flag & SIGNED) && (flag & BYTE)) {
    return SBYTE;
  }
    if ((flag & UNSIGNED) && (flag & BYTE)) {
    return UBYTE;
  }
  if ((flag & SIGNED) && (flag & INT)) {
    return SINT;
  }
  if ((flag & SIGNED) && (flag & INT)) {
    return UINT;
  }
  return 0;  // TODO - add meaning to return value
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
  msglen = tg[0];
  // Serial.printf("msglen = %d\n", msglen);
  // Serial.printf("String length: %i\n", tgstr.length());
  while (count < msglen - 3) {
    // May be scrap at the end, must be >2 chars left to be meaningful
    paramno = ((int16_t)tg[count]) << 8 | (int16_t)tg[count+1];
    // Serial.printf("%02X %02X ", tgstr.charAt(count), tgstr.charAt(count+1));
    count += 2;
    paramlen = getParamLen(paramno);
    paramtype = getParamType(paramno);
    if (paramlen == 1) {
      paramval = ((int16_t)tg[count]);
      count++;
    } else {
      paramval = ((int16_t)tg[count]) << 8 | (int16_t)tg[count+1];
      count += 2;
    }
    // Serial.printf("Param %0d is %d, factor %f\n", paramno, (uint16_t)paramval, getParamFactor(paramno));
  }
  return 0;  // TODO - add meaning to return value
}