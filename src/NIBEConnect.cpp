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
// #include <NIBERegisters.h>   - should be included via NIBEConnect.h
// #include <SoftwareSerial.h>
// #include <paraminfo.h>

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
      // TODO - remove the line below
      rxstr = "";
      if ((inbyte == 0x00) && (inpar)) {
        //        Serial.printf("\n%lu ", millis());
        currentState = ST_addressbegun;
      }
      //      Serial.print(inpar ? '*' : ' ');
      //      Serial.printf("%02X ", inbyte);
      break;

    case ST_addressbegun:
      if ((inbyte == 0x14) && (inpar)) {
        if (rs485.available() == 0) { // Only respond if at end of buffer
          // rs485 queue is empty, don't respond too quickly
          delay(1);
          // TODO - wait slightly without using delay()
          rs485.write(0x06, NONE);
          //          Serial.print(inpar ? '*' : ' ');
          //          Serial.printf("%02X", inbyte);
          //          Serial.printf(" <ACK>");
        }
        currentState = ST_addressed;
      } else {
        // if (inbyte != 0x14) or (no parity) go back to Idle
        /*        Serial.print(inpar ? '*' : ' ');
                Serial.printf("%02X", inbyte);
                Serial.printf(" (not for us)");*/
        currentState = ST_idle;
      }
      break;

    case ST_addressed:
      if ((inbyte == 0xC0) && (inpar == 0)) {
        // Command byte received
        chksum = inbyte;
        currentState = ST_commandreceived;
      } else {
        /*        Serial.print(inpar ? '*' : ' ');
                Serial.printf("%02X ", inbyte);
                Serial.printf(". Expected C0\n");
        */
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
      /*      Serial.print(inpar ? '*' : ' ');
            Serial.printf("%02X ", inbyte);
            */
      break;

    case ST_getsender:
      if ((inbyte == 0x24)) {
        chksum ^= inbyte;
        currentState = ST_getlength;
      } else {
        currentState = ST_idle; // TODO - Is this an error? Consider logging
      }
      //      Serial.print(inpar ? '*' : ' ');
      //      Serial.printf("%02X ", inbyte);
      break;

    case ST_getlength:
      if ((inbyte != 0x00) && (inpar == 0)) {
        msglen = inbyte;
        chksum ^= inbyte;
        bytecount = 0;
        rxtg[0] = inbyte;  // length of telegram stored in first byte
        //        rxstr.concat((unsigned char)inbyte);
        bytecount++;
        rxstr = "";
        //        Serial.print(inpar ? '*' : ' ');
        //        Serial.printf("%02X \n", inbyte);
        //        Serial.printf("(l = %d) ", msglen);
        currentState = ST_gettelegram;
      } else {
        currentState = ST_idle; // TODO - Is this an error? Consider logging
        Serial.print(inpar ? '*' : ' ');
        Serial.printf("%02X GetLength ERROR (zero length)", inbyte);
      }
      break;

    case ST_gettelegram:
      chksum ^= inbyte;
      // TODO - use only String, not array - and just one of them:
      rxtg[bytecount] = inbyte;
      //          rxstr.concat((unsigned char)inbyte);
      // Serial.print(inpar ? '*' : ' ');
      // Serial.printf("%02X ", inbyte);
      bytecount++;
      if (bytecount > msglen) {
        currentState = ST_checktelegram;
      }
      break;

    case ST_checktelegram:
      if (inbyte == chksum) {
        rs485.write(0x06, NONE);
        //Serial.printf("Checksum %02X OK, ACK sent\n", chksum);
        // Serial.printf("\nTelegram is: ");
        /* for (int i = 0; i < rxstr.length(); i++) {
          Serial.printf("%02X ", rxstr[i]);
        }*/
        myHeatpump.storeTg(rxtg);
      } else {
        rs485.write(0x15, NONE);
        Serial.printf("Checksum error! (%02X), NAK sent\n", chksum);
      }
      currentState = ST_idle;
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
