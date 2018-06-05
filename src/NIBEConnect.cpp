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
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X ", inbyte);
        currentState = ST_addressbegun;
      } else {
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X_", inbyte);
      }
      break;

    case ST_addressbegun:
      if ((inbyte == 0x14) && (inpar)) {
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X ", inbyte);
        if (rs485.available()) { // Only respond if at end of buffer
          Serial.printf("\n%lu No ACK, %i chars in buffer.\n", millis(),
                        rs485.available());
          currentState = ST_idle;
        } else {
          // rs485 queue is empty - we are in time to respond
          delay(1); // Don't respond too quickly
          // TODO - wait slightly without using delay()
          rs485.write(0x06, NONE);
          Serial.printf("ACK ");
        }
        currentState = ST_addressed;
      } else {
        // inbyte != 0x14
        // Serial.print(inpar ? '*' : ' ');
        // Serial.printf("%02X ", inbyte);
        currentState = ST_idle;
      }
      break;

    case ST_addressed:
      if ((inbyte == 0xC0) && (inpar == 0)) {
        // Command byte received
        chksum = inbyte;
        currentState = ST_commandreceived;
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X ", inbyte);
      } else {
        currentState = ST_idle; // TODO - Is this an error? Consider logging
        Serial.printf("No command byte (C0) received.\n");
      }
      break;

    case ST_commandreceived:
      if ((inbyte == 0x00) && (inpar == 0)) {
        // Command byte received
        chksum ^= inbyte;
        currentState = ST_getsender;
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X ", inbyte);
      } else {
        currentState = ST_idle; // TODO - Is this an error? Consider logging
        Serial.printf("00 expected, %02X recieved\n", inbyte);
      }
      break;

    case ST_getsender:
      if ((inbyte == 0x24)) {
        chksum ^= inbyte;
        currentState = ST_getlength;
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X ", inbyte);
      } else {
        currentState = ST_idle; // TODO - Is this an error? Consider logging
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X ", inbyte);
      }
      break;

    case ST_getlength:
      if ((inbyte != 0x00) && (inpar == 0)) {
        msglen = inbyte;
        chksum ^= inbyte;
        numbytes = 0;
        currentState = ST_getregisterhigh;
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X (l=%i)\n", inbyte, inbyte);
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
          currentState = ST_getregisterlow;
          Serial.print(inpar ? '*' : ' ');
          Serial.printf("%02X ", inbyte);
        } else {
          // found a single byte of scrap at end of telegram
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
      // TODO - read 1-byte parameters from a list to make the function model
      // independent
      Serial.print(inpar ? '*' : ' ');
      Serial.printf("%02X ", inbyte);

      paramlen = getParamLen(paramno);
      if (paramlen == 1) {
        currentState = ST_getvaluelow;
      } else {
        currentState = ST_getvaluehigh;
      }
      if (numbytes == msglen) {
        Serial.print("\End of message\n");
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
      if (paramlen == 1) Serial.print("    "); // padding
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
      break;
    } // end of state machine switch
  }
  return 0; // Todo - fixa return code
} // end of action()

/*// Functions/states in the statemachine

void NIBEParser::getvaluehigh() {
  // Byte manipulation from
  //
https://stackoverflow.com/questions/13900302/set-upper-and-lower-bytes-of-an-short-int-in-c
  paramval = (paramval & 0x00FF) |
             (newchar << 8); // Set the high byte of the parameter value
  checksum ^= newchar;
  nextstate = getvaluelow;
}

void NIBEParser::getvaluelow() {
  paramval =
      (paramval & 0xFF00) | newchar; // Set the low byte of the parameter
value checksum ^= newchar; nextstate = getvaluelow;
}

void NIBEParser::checktelegram() {
  if (newchar == checksum) {
    sendack(); // Confirm to NIBE that message was not corrupt
    nextstate = error;
  } else {
    sendack(); // Always send ACK to NIBE to avoid alarms
    nextstate = error;
  }
}

void NIBEParser::endoftelegram() {
  if (newchar == 0x03) { // ETX received
    nextstate = idle;
  } else {
    nextstate = error;
  }
}

void NIBEParser::errorstate() {
  printf("");
  // Empty comment
}
*/