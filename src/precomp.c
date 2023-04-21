#include "../include/precomp.h"

void initialize(Points *table, int table_id, int table_size)
{
    Points *current;
    Points *last;
    uint32_t i, offset = table_id * table_size;
    for (i = 0, current = table, last = table + table_size; current < last; current++, i++){
        current->start = i + offset;
        current->end = i + offset;
    }
}

void generate(Points *table, int table_id, int table_size)
{
    unsigned char hashed[SHA256_DIGEST_LENGTH];
    for (Points *current = table, *last = table + table_size; current < last; current++)
        for (uint32_t col_id = 0; col_id < T; col_id++)
            hash_reduction(&(current->end), hashed, table_id, col_id);
}

void cleanSort(Points *table, int table_size, Points *perfect)
{
    
}

void swap(Points *a, Points *b){
    Points * tmp = a;
    a = b;
    b = tmp;
}