#ifndef PRECOMP_H
#define PRECOMP_H

#include "common.h"

void initialize(Points *, int, int);
void positions(int **, int *, const char *);
void clean(Points **, int *);
void generate(Points *, int, int *, int *, int, uint32_t *);
void operations(int *, int, uint32_t *);
void swap(Points *, Points *);
int partition(Points *, int, int);
void quicksort(Points *, int, int);
void sort(Points *, int);
void precompute(Points **, int, int *, int *, int, uint32_t *);
void rice(uint32_t *, uint32_t, char);
void export(Points *, int, const char *);
void cover(Points *, int, int, int, char *, int *);

#endif