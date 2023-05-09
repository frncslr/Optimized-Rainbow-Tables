#include "../include/attack.h"

void ceri(uint32_t *end, uint32_t value, char k, int nb_bits)
{
    *end = ((nb_bits - k - 1) << k) | (value & ((1 << k) - 1));
}

void import(Hashtable htable, int htable_size, int table_size, const char *table_name)
{
    FILE *file;
    if ((file = fopen(table_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", table_name);
        exit(ERROR_FOPEN);
    }

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    if ((fread(table, sizeof(Points), table_size, file)) != (size_t)table_size)
    {
        fprintf(stderr, "Reading file problem : %s\n", table_name);
        exit(ERROR_FWRITE);
    }

    if (fclose(file))
    {
        fprintf(stderr, "Closing file problem : %s", table_name);
        exit(ERROR_FCLOSE);
    }

    for (Points *current = table, *last = table + table_size; current < last; current++)
    {
        insert(htable, htable_size, current->start, current->end);
    }

    free((void *)table);
}

void chain(uint32_t *endpoint, unsigned char *cipher, int table_id, int table_width, int col_id, int *nb_hash)
{
    reduction(endpoint, cipher, table_id, col_id);
    compute(endpoint, table_id, col_id + 1, table_width, nb_hash);
}

void rebuild(uint32_t *candidate, int table_id, int col_id, int *nb_hash)
{
    compute(candidate, table_id, 0, col_id, nb_hash);
}

void attack(unsigned char *cipher, Hashtable htable, int htable_size, int table_id, int table_width, uint32_t *result, char *found, int *nb_hash)
{
    Points *points;
    uint32_t candidate, endpoint;
    for (int col_id = table_width - 1; col_id >= 0; col_id--)
    {
        chain(&endpoint, cipher, table_id, table_width, col_id, nb_hash);
        // printf("** endpoint : %u\n", endpoint);
        if ((points = search(htable, htable_size, endpoint)) != NULL)
        {
            candidate = points->start;
            rebuild(&candidate, table_id, col_id, nb_hash);
            hash(&candidate, buffer);
            if (!memcmp((const char *)cipher, (const char *)buffer, SHA256_DIGEST_LENGTH))
            {
                *result = candidate;
                *found = 1;
                break;
            }
        }
    }
}