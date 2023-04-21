#ifndef COMMON_H
#define COMMON_H

#define SPACE_SIZE (1 << 24)
#define N 1000000
#define R 20.0
#define T 500
#define ALPHA (R / (R + 1))
#define M0 (2 * R * N / (T + 2))
#define Mt (2 * ALPHA * N / (T + 2))

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <openssl/sha.h>

unsigned char *hash(uint32_t, unsigned char *);
uint32_t reduction(unsigned char *, int, int);
uint32_t hash_reduction(uint32_t, int, int);

#endif