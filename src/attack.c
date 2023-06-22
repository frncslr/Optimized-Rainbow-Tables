#include "../include/attack.h"

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

void chain(uint32_t *endpoint, unsigned char *cipher, int table_id, int table_width, int col_id, uint32_t *nb_hash)
{
    reduction(endpoint, cipher, table_id, col_id);
    compute(endpoint, table_id, col_id + 1, table_width, nb_hash);
}

void rebuild(uint32_t *candidate, int table_id, int col_id, uint32_t *nb_hash)
{
    compute(candidate, table_id, 0, col_id, nb_hash);
}

void attack(unsigned char *cipher, Hashtable *htables, int *htables_sizes, int nb_tables, int table_width, uint32_t *result, uint32_t *nb_hash)
{
    Points *points;
    uint32_t candidate, endpoint;
    for (int col_id = table_width - 1; col_id >= 0; col_id--)
    {
        for (int table_id = 0; table_id < nb_tables; table_id++)
        {
            chain(&endpoint, cipher, table_id, table_width, col_id, nb_hash);
            if ((points = search(htables[table_id], htables_sizes[table_id], endpoint)) != NULL)
            {
                candidate = points->start;
                rebuild(&candidate, table_id, col_id, nb_hash);
                hash(&candidate, buffer);
                (*nb_hash)++;
                if (!memcmp((const char *)cipher, (const char *)buffer, SHA256_DIGEST_LENGTH))
                {
                    *result = candidate;
                    return;
                }
            }
        }
    }
}

void attackCDE(unsigned char *cipher, uint32_t **spTables, BitStream *epStreams, Index **idxTables, int *tables_sizes, int nb_tables, int table_width, uint32_t *result, uint32_t *nb_hash)
{
    uint32_t *found, candidate, endpoint;
    for (int col_id = table_width - 1; col_id >= 0; col_id--)
    {
        for (int table_id = 0; table_id < nb_tables; table_id++)
        {
            chain(&endpoint, cipher, table_id, table_width, col_id, nb_hash);
            if ((found = searchCDE(endpoint, spTables[table_id], &(epStreams[table_id]), idxTables[table_id], tables_sizes[table_id], N, L)) != NULL)
            {
                candidate = *found;
                rebuild(&candidate, table_id, col_id, nb_hash);
                hash(&candidate, buffer);
                (*nb_hash)++;
                if (!memcmp((const char *)cipher, (const char *)buffer, SHA256_DIGEST_LENGTH))
                {
                    *result = candidate;
                    return;
                }
            }
        }
    }
}