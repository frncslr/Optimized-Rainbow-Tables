#ifndef RICE_H
#define RICE_H

// #include <stdio.h>
// #include <stdint.h>
// #include <math.h>
#include "common.h"

#define PHI ((1 + sqrt(5)) / 2.0)
#define L (1 << 5)
#define kk 3

int Kopt(int, int);
double Ropt(int, int, int);
int addrBits(int, double);
int chainBits(int);
int memory(int, double, int);

typedef struct bitstream
{
    uint8_t BitBuffer;
    uint8_t BitCount;
    uint8_t BitLimit;
    uint64_t BitTotal;
    FILE *file;
    const char *file_name;
} BitStream;

void initBitStream(BitStream *, const char *, char);
void closeBitStream(BitStream *);
void writeBit(BitStream *, uint8_t);
void encode(BitStream *, uint32_t, int);
void writeIdx(BitStream *, uint64_t, int, int, int);
void flushStream(BitStream *);
void exportCDE(Points *, int, int, int, const char *, const char *, const char *);

typedef struct index
{
    uint32_t address;
    uint32_t chain_id;
} Index;

int readBit(BitStream *);
void importSP(const char *, uint32_t **, int *);
void importIdx(const char *, int, int, int, Index *);
void getIdx(char *, uint32_t, int, int, int, uint32_t *, uint32_t *);

void cdeStats(int, int, int, const char *, const char *, const char *);

void rice(uint32_t *, uint32_t);
void ceri(uint32_t *, uint32_t, int);

#endif