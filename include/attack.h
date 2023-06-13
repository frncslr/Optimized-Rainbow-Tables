#ifndef ATTACK_H
#define ATTACK_H

#include "common.h"

void import(Hashtable, int, int, const char *);
void chain(uint32_t *, unsigned char *, int, int, int, uint32_t *);
void rebuild(uint32_t *, int, int, uint32_t *);
void attack(unsigned char *, Hashtable *, int *, int, int, uint32_t *, uint32_t *);

#endif