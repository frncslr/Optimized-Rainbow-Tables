#ifndef PRECOMP_H
#define PRECOMP_H

#include "common.h"

void initialize(Points *, int, int);
void generate(Points *, int, int, int, int *, unsigned char *);
void swap(Points *, Points *);
int partition(Points *, int, int);
void quicksort(Points *, int, int);
void sort(Points *, int);
void clean(Points *, int *, int);
void rice(uint32_t *, uint32_t, char);
void export(Points *, int, const char *);
void precompute(char *, int, int *, int, int *, int, int *);

#endif