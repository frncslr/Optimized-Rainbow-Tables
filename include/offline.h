#ifndef OFFLINE_H
#define OFFLINE_H

#include "rice.h"
#include "common.h"

void initialize(RTable *, int, int);
void clean(RTable *, int *, int);
void generate(RTable, int, int *, int *, int, int, uint64_t, uint64_t *, double *, double *);
void swap(Chain *, Chain *);
int partition(RTable, int, int);
void quicksort(RTable, int, int);
void sort(RTable, int);
void precompute(RTable *, int, int *, int *, int, int, uint64_t, uint64_t *, double *, double *);
void export(RTable, int, const char *);
void cover(RTable, int, int, int, uint64_t, char *, double *);

void positions(int **, int *, const char *);
void operations(uint64_t, int, int *, int, uint64_t *);
void hashStats(uint64_t hash_expe, uint64_t hash_theo);
void epStats(int mt_expe, int mt_theo);
void coverStats(double p_expe, double p_theo);

#endif