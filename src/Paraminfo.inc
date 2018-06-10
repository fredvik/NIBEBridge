/*
 * NIBEConnect implements communication with a NIBE 360p heatpump.
 * and possibly other models as well.
 *
 * Copyright (C) 2018 Fredrik Viklund
 * fredrik@viklund.se
 */

#ifndef Paraminfo_h
#define Paraminfo_h

typedef struct {
    uint16_t bitmask;
    /*int16_t resolution; always
    int16_t minlimit;
    int16_t maxlimit;*/
    uint8_t  flags;
    int8_t   factor; 
} ParamInfo;

#define BYTE     (uint8_t)0x00  // Parameter variable types
#define INT      (uint8_t)0x01
#define UNSIGNED (uint8_t)0x00
#define SIGNED   (uint8_t)0x02  

#define RO       (uint8_t)0x00
#define RW       (uint8_t)0x04
#define PRIV     (uint8_t)0x08  // Writeable but should be limited

const ParamInfo params360p[] = {
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0x0002, UNSIGNED | BYTE | RO, 1 },


  {(uint16_t)0xE000, UNSIGNED | INT | RO, 1 },






  {(uint16_t)0x8000, UNSIGNED | INT | RW, 1 },


  {(uint16_t)0x0001, UNSIGNED | INT | RO, 1 },





  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0xFFFF, SIGNED | INT | RO, -10 },
  {(uint16_t)0x0008, SIGNED | BYTE | RW | PRIV, 1 },
  {(uint16_t)0xFFFF, UNSIGNED | INT | RO, 1 },
  {(uint16_t)0xFFFF, UNSIGNED | INT | RO, 1 },
  {(uint16_t)0xFFFF, UNSIGNED | INT | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0x0001, UNSIGNED | INT | RW | PRIV, 1 },



  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RW, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 },
  {(uint16_t)0xFFFF, SIGNED | BYTE | RO, 1 }
};

#endif