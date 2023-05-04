#ifndef ATTACK_H
#define ATTACK_H

#include "common.h"

void ceri(uint32_t *, uint32_t, char, int);
void import(Hashtable, int, const char *);
void chain(uint32_t *, unsigned char *, unsigned char *, int, int, int);
void rebuild(uint32_t *, unsigned char *, int, int);
void attack(unsigned char *, unsigned char *, Pair **, int, int, uint32_t *, char *);

#endif