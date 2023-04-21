#ifndef PRECOMP_H
#define PRECOMP_H

#include "./common.h"

void initialize(Points *, int, int);
void generate(Points *, int, int);
void cleanSort(Points *, int, Points *);

void swap(Points *, Points *);
int partition(Points *, Points *, Points *);
void quickSort(Points *, Points *, Points *);

#endif