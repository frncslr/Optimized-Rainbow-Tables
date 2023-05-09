#ifndef ATTACK_H
#define ATTACK_H

#include "common.h"

void ceri(uint32_t *, uint32_t, char, int);
void import(Hashtable, int, int, const char *);
void chain(uint32_t *, unsigned char *, int, int, int, int *);
void rebuild(uint32_t *, int, int, int *);
void attack(unsigned char *, Hashtable, int, int, int, uint32_t *, char *, int *);

#endif