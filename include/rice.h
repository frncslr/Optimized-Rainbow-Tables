#ifndef RICE_H
#define RICE_H

#include "common.h"

#define PHI ((1 + sqrt(5)) / 2.0)

int Kopt(uint64_t, int);
double Ropt(uint64_t, int, int);
int addressBitSize(int, double);
int chainIdBitSize(int);
int Lblocks(int);
int memory(int, double, int);

typedef struct bitstream
{
    uint8_t BitBuffer;
    uint8_t BitCount;
    uint8_t BitLimit;
    uint64_t BitTotal;
    FILE *file;
    const char *file_name;
    char eof;
} BitStream;

void initBitStream(BitStream *, const char *, char);
void closeBitStream(BitStream *);
void writeBit(BitStream *, uint8_t);
void encode(BitStream *, Point, int);
void writeIdx(BitStream *, uint64_t, int, int, int);
void flushStream(BitStream *);
void exportCDE(RTable, int, uint64_t, int, const char *, const char *, const char *);

typedef struct index
{
    uint32_t address;
    uint32_t chain_id;
} Index;

int readBit(BitStream *);
void importSP(const char *, Point **, int *);
void importIdx(const char *, int, int, uint64_t, Index **);
Point decode(BitStream *, int, int *, uint32_t *);
void setStream(BitStream *, uint32_t);
Point *searchCDE(Point, Point *, BitStream *, Index *, int, int, int, uint32_t *);

void cdeStats(int, int *, int, int*,char *, char *, char *);

#endif