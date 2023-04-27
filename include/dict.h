#ifndef DICT_H
#define DICT_H

#define DICTSIZE 1000

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct pair
{
    struct pair *next;
    uint32_t start;
    uint32_t end;
} Pair;

int idx(uint32_t);
Pair *get(uint32_t, Pair **);
void put(uint32_t, uint32_t, Pair **);
void free_chain(Pair *);
void free_dict(Pair **);

#endif