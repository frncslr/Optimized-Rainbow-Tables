#ifndef RICE_H
#define RICE_H

// #include <stdio.h>
// #include <stdint.h>
// #include <math.h>
#include "common.h"

#define k 3
#define R 6

typedef struct
{
    uint8_t BitBuffer;
    uint8_t BitCount;
    uint8_t BitLimit;
    uint64_t BitTotal;
    FILE *file;
    const char *file_name;
} BitStream;

void initBitStream(BitStream *, const char *);
void closeBitStream(BitStream *);
void writeBit(BitStream *, uint8_t);
void encode(BitStream *, uint32_t);
void writeIdx(BitStream *, uint64_t, int, int, int);
void flushStream(BitStream *);
void exportCDE(Points *, int, const char *, const char *, const char *);

void rice(uint32_t *, uint32_t);
void ceri(uint32_t *, uint32_t, int);

#endif