#ifndef PRECOMP_H
#define PRECOMP_H

#include "./common.h"

void initialize(Points *, int, int);
void generate(Points *, int, int, int, int*);
void swap(Points *, Points *);
int partition(Points *, int, int);
void sort(Points *, int, int);
void clean(Points *, int *, Points *);
void rice(uint32_t *, uint32_t, char);
void ceri(uint32_t *, uint32_t, char, int);
void export(Points *, int, const char *);
void import(Points *, int, const char *);
void precomp();

#endif