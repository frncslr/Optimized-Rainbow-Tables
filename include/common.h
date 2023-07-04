#ifndef COMMON_H
#define COMMON_H

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <openssl/sha.h>

#define ERROR_ALLOC 1289
#define ERROR_FOPEN 1229
#define ERROR_FCLOSE 1223
#define ERROR_FWRITE 1103
#define ERROR_FREAD 1049
#define ERROR_INSERT 1031
#define ERROR_COVER 1019

#define HASH_LENGTH SHA256_DIGEST_LENGTH
static unsigned char buffer[HASH_LENGTH];

double ALPHA(double);
double R(double);
int M0(uint64_t, double, int);
int Mt(uint64_t, double, int);
int Mi(uint64_t, int, int);

typedef uint64_t Point;
typedef struct chain
{
    Point sp;
    Point ep;
} Chain;
typedef Chain *RTable;
typedef Chain *HTable;

void hash(Point *, unsigned char *);
void reduce(Point *, unsigned char *, int, int, int, uint64_t);
void hash_reduce(Point *, int, int, int, uint64_t);
void compute(Point *, int, int, int, int, uint64_t, uint64_t *);

#define MAX (Point)(-1)
#define LOAD_FACTOR 1.5

int sizeHTable(uint64_t, int, int);
void initHTable(HTable *, int);
int insert(HTable, int, Chain *);
Chain *search(HTable, int, uint64_t);

typedef struct timeval timeval;

double elapsed(timeval *, timeval *);
void print_hash(unsigned char *);
void write_results(double *, int, const char *);
void read_results(double *, int, const char *);

#endif