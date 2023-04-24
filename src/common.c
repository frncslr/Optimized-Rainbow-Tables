#include "../include/common.h"

void hash(uint32_t *point, unsigned char *hashed)
{
    SHA256((const unsigned char *)point, sizeof(uint32_t), hashed);
}

void reduction(uint32_t *point, unsigned char *hashed, int table_id, int col_id)
{
    *point = (*(uint32_t *)hashed + table_id + col_id * t) % N;
}

void hash_reduction(uint32_t *point, unsigned char *hashed, int table_id, int col_id)
{
    hash(point, hashed);
    reduction(point, hashed, table_id, col_id);
}