/*
 * NIBEConnect implements communication with a NIBE 360p heatpump.
 * and possibly other models as well.
 *
 * Copyright (C) 2018 Fredrik Viklund
 * fredrik@viklund.se
 */

/*
 * telegram = hela paketet från eller till RCU
 * message  = listan av C0, length, parametrar/värden - som beräknar checksum
 **/

#include <NIBEConnect.h>
#include <NIBERegisters.h>
#include <SoftwareSerial.h>

int NIBEConnect::connect() {
  //  rs485 = connection;
  return 0;
}
int NIBEConnect::action() {
  // Parse data from the NIBE
  int queue = rs485.available();

  if (queue) {
    inpar = rs485.peekParity();
    inbyte = rs485.read();

    switch (currentState) {
    case ST_idle:
      if ((inbyte == 0x00) && (inpar)) {
        Serial.printf("\n%lu ", millis());
        currentState = ST_addressbegun;
      }
      Serial.print(inpar ? '*' : ' ');
      Serial.printf("%02X ", inbyte);
      break;

    case ST_addressbegun:
      if ((inbyte == 0x14) && (inpar)) {
        if (rs485.available() == 0) { // Only respond if at end of buffer
          // rs485 queue is empty, don't respond too quickly
          delay(1);
          // TODO - wait slightly without using delay()
          rs485.write(0x06, NONE);
          Serial.print(inpar ? '*' : ' ');
          Serial.printf("%02X", inbyte);
          Serial.printf(" <ACK>");
        }
        currentState = ST_addressed;
      } else {
        // if (inbyte != 0x14) or (no parity) go back to Idle
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X", inbyte);
        Serial.printf(" (not for us)");
        currentState = ST_idle;
      }
      break;

    case ST_addressed:
      if ((inbyte == 0xC0) && (inpar == 0)) {
        // Command byte received
        chksum = inbyte;
        currentState = ST_commandreceived;
      } else {
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X ", inbyte);
        Serial.printf(". Expected C0\n");
        currentState = ST_idle; // TODO - Is this an error? Consider logging
      }
      break;

    case ST_commandreceived:
      if ((inbyte == 0x00) && (inpar == 0)) {
        chksum ^= inbyte;
        currentState = ST_getsender;
      } else {
        Serial.printf("00 expected, %02X recieved\n", inbyte);
        currentState = ST_idle; // TODO - Is this an error? Consider logging
      }
      Serial.print(inpar ? '*' : ' ');
      Serial.printf("%02X ", inbyte);
      break;

    case ST_getsender:
      if ((inbyte == 0x24)) {
        chksum ^= inbyte;
        currentState = ST_getlength;
      } else {
        currentState = ST_idle; // TODO - Is this an error? Consider logging
      }
      Serial.print(inpar ? '*' : ' ');
      Serial.printf("%02X ", inbyte);
      break;

    case ST_getlength:
      if ((inbyte != 0x00) && (inpar == 0)) {
        msglen = inbyte;
        chksum ^= inbyte;
        numbytes = 0;
        currentState = ST_getregisterhigh;
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X \n", inbyte);
      } else {
        currentState = ST_idle; // TODO - Is this an error? Consider logging
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X GetLength ERROR (zero length)", inbyte);
      }
      break;

    case ST_getregisterhigh:
      if ((inpar == 0)) {
        chksum ^= inbyte;
        numbytes++;
        paramno = inbyte;
        paramno = paramno << 8;
        if (numbytes < msglen) {
          Serial.print(inpar ? '*' : ' ');
          Serial.printf("%02X ", inbyte);
          currentState = ST_getregisterlow;
        } else {
          // a single byte of scrap at end of telegram
          Serial.print(inpar ? '*' : ' ');
          Serial.printf("%02X scrapped\n", inbyte);
          currentState = ST_checktelegram;
          break;
        }
      }
      break;

    case ST_getregisterlow:
      // if ((inpar == 0)) {
      chksum ^= inbyte;
      numbytes++;
      paramno += inbyte;
      paramval = 0;
      paramlen = getParamLen(paramno);
      if (paramlen == 1) {
        currentState = ST_getvaluelow;
      } else {
        currentState = ST_getvaluehigh;
      }
      Serial.print(inpar ? '*' : ' ');
      Serial.printf("%02X ", inbyte);
      if (numbytes == msglen) {
        Serial.print("\n Message end in ST_getregisterlow\n");
        currentState = ST_checktelegram;
      }
      // } end of if((inpar==0)
      break;

    case ST_getvaluehigh:
      chksum ^= inbyte;
      numbytes++;
      paramval = inbyte;
      paramval = paramval << 8;
      Serial.print(inpar ? '*' : ' ');
      Serial.printf("%02X ", inbyte);
      currentState = ST_getvaluelow;
      break;

    case ST_getvaluelow:
      chksum ^= inbyte;
      numbytes++;
      paramval += inbyte;
      if (numbytes >= msglen) {
        currentState = ST_checktelegram;
      } else {
        currentState = ST_getregisterhigh;
      }
      Serial.print(inpar ? '*' : ' ');
      Serial.printf("%02X ", inbyte);
      if (paramlen == 1)
        Serial.print("    "); // padding
      Serial.printf("    Param %i is %i at pos %i, length %i\n", paramno,
                    paramval, numbytes, paramlen);
      break;

    case ST_checktelegram:
      if (inbyte == chksum) {
        rs485.write(0x06, NONE);
        Serial.printf("Checksum %02X OK, ACK sent\n", chksum);
      } else {
        rs485.write(0x15, NONE);
        Serial.printf("Checksum error! (%02X), NAK sent\n", chksum);
      }
      currentState = ST_endoftelegram;
      break;

    case ST_endoftelegram:
      Serial.print("\n");
      currentState = ST_idle;
      break;

    case ST_error:
      Serial.print("Error: bla blah bla.\n");
      break;
    } // end of state machine switch
  }
  return 0; // Todo - fixa return code
} // end of action()
