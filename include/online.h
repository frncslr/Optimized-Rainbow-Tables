#ifndef ONLINE_H
#define ONLINE_H

#include "rice.h"
#include "common.h"

void import(HTable *, int *, const char *);
void chain(Point *, unsigned char *, int, int, int, uint64_t, uint64_t *);
void rebuild(Point *, int, int, int, uint64_t, uint64_t *);
void attackSTD(unsigned char *, HTable *, int *, int, int, uint64_t, Point *, uint64_t *);
void attackCDE(unsigned char *, Point **, BitStream *, Index **, int *, int, int, uint64_t, Point *, uint64_t *, double *);

#endif