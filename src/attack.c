#include "../include/attack.h"

void ceri(uint32_t *end, uint32_t value, char k, int nb_bits)
{
    *end = ((nb_bits - k - 1) << k) | (value & ((1 << k) - 1));
}

void import(Hashtable htable, int htable_size, const char *table_name)
{
    FILE *file;
    if ((file = fopen(table_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", table_name);
        exit(ERROR_FOPEN);
    }

    Points *table;
    if ((table = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    if ((fread(table, sizeof(Points), htable_size, file)) != (size_t)htable_size)
    {
        fprintf(stderr, "Reading file problem : %s\n", table_name);
        exit(ERROR_FWRITE);
    }

    if (fclose(file))
    {
        fprintf(stderr, "Closing file problem : %s", table_name);
        exit(ERROR_FCLOSE);
    }

    for (Points *current = table, *last = table + htable_size; current < last; current++)
    {
        insert(htable, htable_size, current->start, current->end);
    }

    free((void *)table);
}

void chain(uint32_t *point, unsigned char *cipher, unsigned char *hashed, int table_id, int col_id, int table_width)
{
    int nb_hash = 0;
    reduction(point, cipher, table_id, col_id);
    compute(point, hashed, table_id, col_id + 1, table_width, &nb_hash);
}

void rebuild(uint32_t *candidate, unsigned char *hashed, int table_id, int col_id)
{
    int nb_hash = 0;
    compute(candidate, hashed, table_id, 0, col_id, &nb_hash);
}

void attack(unsigned char *cipher, Hashtable htable, int htable_size, int table_id, int table_width, uint32_t *result, char *found, unsigned char *buffer)
{
    Points *points;
    uint32_t candidate, endpoint;
    for (int col = table_width - 1; col >= 0; col--)
    {
        chain(&endpoint, cipher, buffer, table_id, col, table_width);
        if ((points = search(htable, htable_size, endpoint)) != NULL)
        {
            candidate = points->start;
            rebuild(&candidate, buffer, table_id, col);
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