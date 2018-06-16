/*
 * NIBERegisters holds the content of information sent to or from the NIBE
 * heatpump.
 *
 * Copyright (C) 2018 Fredrik Viklund
 * fredrik@viklund.se
 */
#include <Arduino.h>
#include <HardwareSerial.h>
#include <NIBERegisters.h>
#include <PubSubClient.h>

extern char mqttMsg[50];
extern char mqttTopic[50];
extern PubSubClient mqtt;

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
  msglen = tg[0];
  // Serial.printf("msglen = %d\n", msglen);
  // Serial.printf("String length: %i\n", tgstr.length());
  while (count < msglen - 3) {
    // May be scrap at the end, must be >2 chars left to be meaningful
    paramno = ((int16_t)tg[count]) << 8 | (int16_t)tg[count + 1];
    count += 2;
    paramlen = getParamLen(paramno);
    paramtype = getParamType(paramno);
    /* Serial.printf("\nParam %d (%02X), type %d, length %d ", paramno, paramno,
                  paramtype, paramlen); */
    switch (paramtype) {
    case SBYTE:
      // Serial.print("SBYTE ");
      paramval = (float)((int8_t)tg[count]) * getParamFactor(paramno);
      count++;
      break;
    case UBYTE:
      // Serial.print("UBYTE ");
      paramval = (float)((uint8_t)tg[count]) * getParamFactor(paramno);
      count++;
      break;
    case SINT:
      // Serial.print("SINT ");
      paramval = (float)((int16_t)tg[count] << 8 | (int16_t)tg[count + 1]) *
                 getParamFactor(paramno);
      count += 2;
      break;
    case UINT:
      // Serial.print("UINT ");
      paramval = (float)((uint16_t)tg[count] << 8 | (uint16_t)tg[count + 1]) *
                 getParamFactor(paramno);
      count += 2;
      break;
    case BITFIELD: // TODO
      // Serial.print("BITFIELD ");
      paramval = (uint16_t)tg[count] << 8 | (uint16_t)tg[count + 1];
      count += 2;
      break;
    }
    if (stored[paramno].value != paramval) {
      stored[paramno].value = paramval;
      stored[paramno].lastChanged = millis();

      snprintf(mqttTopic, 75, "NibeBridge/out/%02d", paramno);
      snprintf(mqttMsg, 75, "{ \"parameter\" : %2d, \"value\" : %.1f, \"name\" : \"s\" }", paramno, paramval);
      Serial.printf("Topic: %s, message: %s\n", mqttTopic, mqttMsg);
      mqtt.publish(mqttTopic, mqttMsg);

    } else if (millis() >
               stored[paramno].lastPublished + 60000 + random(5000)) {
      // TODO - replace with PublishInterval, salt with a random delay to avoid
      // too many bursts
      stored[paramno].lastPublished = millis();
    }
  }

  return 0; // TODO - add meaning to return value
}