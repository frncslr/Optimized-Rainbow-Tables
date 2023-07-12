#ifndef ONLINE_H
#define ONLINE_H

#include "rice.h"
#include "common.h"

void import(HTable *, int *, int *, const char *);
void chain(Point *, unsigned char *, int, int, int, uint64_t, uint64_t *);
void rebuild(Point *, int, int, int, uint64_t, uint64_t *);
void attackSTD(unsigned char *, HTable *, int *, int, int, uint64_t, Point *, uint64_t *);
void attackCDE(unsigned char *, Point **, BitStream *, Index **, int *, int, int, uint64_t, Point *, uint64_t *, double *);

typedef struct metric
{
    long double nu;
    int table_id;
} Metric;
long double Nu(uint64_t N, int mt, int t, int col);
int compareMetric(const void *a, const void *b);
void sortMetric(Metric *metric, int ell);
void attackSTDxHTG(unsigned char *cipher, HTable *htables, int *hsizes, uint64_t N, int ell, int *t, int *mt, Point *result, uint64_t *nb_hash);
void attackCDExHTG(unsigned char *cipher, Point **spTables, BitStream *epStreams, Index **idxTables, uint64_t N, int ell, int *t, int *mt, Point *result, uint64_t *nb_hash, double *avgDec);

#endif