#include "../include/dict.h"

int idx(uint32_t end)
{
    return end % DICTSIZE;
}

Pair *get(uint32_t end, Pair **dict)
{
    for (Pair *pair = dict[idx(end)]; pair != NULL; pair = pair->next)
        if (pair->end == end)
            return pair;
    return NULL;
}

char present(uint32_t start, Pair **dict)
{
    for (int i = 0; i < DICTSIZE; i++)
        for (Pair *current = dict[i]; current != NULL; current = current->next)
            if (current->start == start)
                return 1;
    return 0;
}

void put(uint32_t end, uint32_t start, Pair **dict)
{
    if (get(end, dict) == NULL)
    {
        Pair *pair;
        if ((pair = (Pair *)malloc(sizeof(Pair))) == NULL)
        {
            fprintf(stderr, "Memory allocation problem\n");
            exit(43567);
        }
        int index = idx(end);
        pair->end = end;
        pair->start = start;
        pair->next = dict[index];
        dict[index] = pair;
    }
}

void free_chain(Pair *pair)
{
    if (pair != NULL)
    {
        free_chain(pair->next);
        free((void *)pair);
    }
}

void free_dict(Pair **dict)
{
    for (Pair **current = dict, **last = dict + DICTSIZE; current < last; current++)
        free_chain(*(current));
}