#ifndef COMMON_H
#define COMMON_H

#define ERROR_ALLOC 1289
#define ERROR_FOPEN 1229
#define ERROR_FCLOSE 1223
#define ERROR_FWRITE 1103
#define ERROR_FREAD 1049

#define N (1 << 24)
#define r 20.0
#define t 1000
#define l 1
#define alpha (r / (r + 1))
#define m0 (2 * r * N / (t + 2))
#define mt (2 * alpha * N / (t + 2))

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>
#include "dict.h"

void print_hash(unsigned char *);
void hash(uint32_t *, unsigned char *);
void reduction(uint32_t *, unsigned char *, int, int);
void hash_reduction(uint32_t *, unsigned char *, int, int);

typedef struct
{
    uint32_t start;
    uint32_t end;
} Points;


#endif