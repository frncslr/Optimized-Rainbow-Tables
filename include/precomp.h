#ifndef PRECOMP_H
#define PRECOMP_H

#include "./common.h"

void initialize(Points *, int, int);
void generate(Points *, int, int);
void swap(Points *, Points *);
int partition(Points *, int, int);
void sort(Points *, int, int);
void clean(Points *, int, Points *);
void cde(uint32_t *, uint32_t *, char);

#endif