#ifndef PRECOMP_H
#define PRECOMP_H

#include "common.h"

void initialize(Points *, int, int);
void generate(Points *, int, int, int, int *);
void swap(Points *, Points *);
int partition(Points *, int, int);
void quicksort(Points *, int, int);
void sort(Points *, int);
void clean(Points *, int *, int);
void precompute(Points *, int, int *, int, int *);
void rice(uint32_t *, uint32_t, char);
void export(Points *, int, const char *);
void cover(Points *, int, int, int, int *);

#endif