#include "../include/attack.h"

void ceri(uint32_t *end, uint32_t value, char k, int nb_bits)
{
    *end = ((nb_bits - k - 1) << k) | (value & ((1 << k) - 1));
}

void import(Pair **dict, int dict_size, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    Points *table;
    if ((table = (Points *)calloc(dict_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    if ((fread(table, sizeof(Points), dict_size, file)) != (size_t)dict_size)
    {
        fprintf(stderr, "Reading file problem : %s\n", file_name);
        exit(ERROR_FWRITE);
    }

    if (fclose(file))
    {
        fprintf(stderr, "Closing file problem : %s", file_name);
        exit(ERROR_FCLOSE);
    }

    for (Points *current = table, *last = table + dict_size; current < last; current++)
    {
        put(current->end, current->start, dict);
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

void attack(unsigned char *cipher, unsigned char *hashed, Pair **dict, int table_id, int table_width, uint32_t *result, char *found)
{
    Pair *pair;
    uint32_t startpoint, endpoint;
    for (int col = table_width - 1; col >= 0; col--)
    {
        chain(&endpoint, cipher, hashed, table_id, col, table_width);
        if ((pair = get(endpoint, dict)) != NULL)
        {
            startpoint = pair->start;
            rebuild(&startpoint, hashed, table_id, col);
            hash(&startpoint, hashed);
            if (!strcmp((const char *)cipher, (const char *)hashed))
            {
                *result = startpoint;
                *found = 1;
                break;
            }
        }
    }
}