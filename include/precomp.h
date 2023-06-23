#ifndef PRECOMP_H
#define PRECOMP_H

#include "common.h"
#include "rice.h"

void initialize(Points *, int, int);
void positions(int **, int *, const char *);
void clean(Points **, int *, int);
void generate(Points *, int, int *, int *, int, uint32_t *);
void operations(int *, int, uint32_t *);
void swap(Points *, Points *);
int partition(Points *, int, int);
void quicksort(Points *, int, int);
void sort(Points *, int);
void precompute(Points **, int, int *, int *, int, uint32_t *);
void export(Points *, int, const char *);
void cover(Points *, int, int, int, char *, int *);

void hashStats(uint32_t, int *, int, int);
void epStats(int, int, int);
void coverStats(int, int, int, int, int);

#endif