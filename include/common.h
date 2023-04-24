#ifndef COMMON_H
#define COMMON_H

#define ERROR_ALLOC 1289
#define ERROR_FOPEN 1229
#define ERROR_FCLOSE 1223
#define ERROR_FWRITE 1103
#define ERROR_FREAD 1049

#define SPACE_SIZE (1 << 24)
#define N 1000000
#define R 20.0
#define T 1000
#define ALPHA (R / (R + 1))
#define M0 (2 * R * N / (T + 2))
#define Mt (2 * ALPHA * N / (T + 2))

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>

void hash(uint32_t *, unsigned char *);
void reduction(uint32_t *, unsigned char *, int, int);
void hash_reduction(uint32_t *, unsigned char *, int, int);

typedef struct
{
    uint32_t start;
    uint32_t end;
} Points;


#endif