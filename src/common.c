#include "../include/common.h"

void print_hash(unsigned char *hashed_value)
{
    for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        printf("%02x", hashed_value[i]);
    printf("\n");
}

void hash(uint32_t *point, unsigned char *digest)
{
    SHA256((const unsigned char *)point, sizeof(uint32_t), digest);
}

void reduction(uint32_t *point, unsigned char *digest, int table_id, int col_id)
{
    *point = (*(uint32_t *)digest + table_id + col_id * t) % N;
}

void hash_reduction(uint32_t *point, int table_id, int col_id)
{
    hash(point, buffer);
    reduction(point, buffer, table_id, col_id);
}

void compute(uint32_t *point, int table_id, int col_start, int col_end, uint32_t *nb_hash)
{
    for (int col_id = col_start; col_id < col_end; col_id++, (*nb_hash)++)
        hash_reduction(point, table_id, col_id);
}

void init(Hashtable hashtable, int size)
{
    for (Points *current = hashtable, *last = hashtable + size; current < last; current++)
        current->end = MAX;
}

int insert(Hashtable hashtable, int size, uint32_t start, uint32_t end)
{
    Points *point;
    int i;
    for (i = 0; i < size; i++)
    {
        point = hashtable + (end + i) % size;
        if (point->end == MAX)
        {
            point->end = end;
            point->start = start;
            return 1;
        }
        if ((point->end) == end)
            return 0;
    }
    return 0;
}

Points *search(Hashtable hashtable, int size, uint32_t end)
{
    Points *point;
    int i, idx = end % size;
    for (i = 0; i < size; i++)
    {
        point = hashtable + (idx + i) % size;
        if (point->end == end)
            return point;
    }
    return NULL;
}