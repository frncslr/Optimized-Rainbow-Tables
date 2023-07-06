#include "../include/online.h"

void import(HTable *htable, int *hsize, const char *table_name)
{
    FILE *file;
    if ((file = fopen(table_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", table_name);
        exit(ERROR_FOPEN);
    }
    struct stat stat;
    fstat(fileno(file), &stat);
    int m = (int)stat.st_size / sizeof(Chain);

    RTable table;
    if ((table = (Chain *)calloc(m, sizeof(Chain))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    if ((fread(table, sizeof(Chain), m, file)) != (size_t)m)
    {
        fprintf(stderr, "Reading file problem : %s\n", table_name);
        exit(ERROR_FWRITE);
    }

    if (fclose(file))
    {
        fprintf(stderr, "Closing file problem : %s", table_name);
        exit(ERROR_FCLOSE);
    }

    *hsize = (int)ceil(LOAD_FACTOR * m);
    initHTable(htable, *hsize);

    for (Chain *current = table, *last = table + m; current < last; current++)
    {
        insert(*htable, *hsize, current);
    }

    free((void *)table);
}
void chain(Point *endpoint, unsigned char *cipher, int table_id, int col_id, int t, uint64_t N, uint64_t *nb_hash)
{
    reduce(endpoint, cipher, table_id, col_id, t, N);
    compute(endpoint, table_id, col_id + 1, t, t, N, nb_hash);
}
void rebuild(Point *candidate, int table_id, int col_id, int t, uint64_t N, uint64_t *nb_hash)
{
    compute(candidate, table_id, 0, col_id, t, N, nb_hash);
}
void attackSTD(unsigned char *cipher, HTable *htables, int *hsizes, int ell, int t, uint64_t N, Point *result, uint64_t *nb_hash)
{
    Chain *chain_found;
    Point candidate, endpoint;
    for (int col_id = t - 1; col_id >= 0; col_id--)
    {
        for (int table_id = 0; table_id < ell; table_id++)
        {
            chain(&endpoint, cipher, table_id, col_id, t, N, nb_hash);
            if ((chain_found = search(htables[table_id], hsizes[table_id], endpoint)) != NULL)
            {
                candidate = chain_found->sp;
                rebuild(&candidate, table_id, col_id, t, N, nb_hash);
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
void attackCDE(unsigned char *cipher, Point **spTables, BitStream *epStreams, Index **idxTables, int *m, int ell, int t, uint64_t N, Point *result, uint64_t *nb_hash, double *avgDec)
{
    Point *found, candidate, endpoint;
    uint32_t nbDec = 0;
    double nbSearch = 0.0;
    for (int col_id = t - 1; col_id >= 0; col_id--)
    {
        for (int table_id = 0; table_id < ell; table_id++)
        {
            chain(&endpoint, cipher, table_id, col_id, t, N, nb_hash);
            nbSearch++;
            if ((found = searchCDE(endpoint, spTables[table_id], &(epStreams[table_id]), idxTables[table_id], m[table_id], N, Lblocks(m[table_id]), &nbDec)) != NULL)
            {
                candidate = *found;
                rebuild(&candidate, table_id, col_id, t, N, nb_hash);
                hash(&candidate, buffer);
                (*nb_hash)++;
                if (!memcmp((const char *)cipher, (const char *)buffer, SHA256_DIGEST_LENGTH))
                {
                    *result = candidate;
                    *avgDec = nbDec / nbSearch;
                    return;
                }
            }
        }
    }
    *avgDec = nbDec / nbSearch;
}