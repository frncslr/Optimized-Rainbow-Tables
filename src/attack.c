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

void chain(uint32_t *point, unsigned char *hashed, int table_id, int col_id)
{
    reduction(point, hashed, table_id, col_id);
    for (++col_id; col_id < t; col_id++)
        hash_reduction(point, hashed, table_id, col_id);
}

void rebuild(uint32_t *candidate, unsigned char *hashed, int table_id, int col_id)
{
    for (int col = 0; col < col_id; col++)
        hash_reduction(candidate, hashed, table_id, col);
}

void attack(unsigned char *cipher, char *file_name, int size)
{
    strcat(file_name, "1.dat");
    static Pair *dict[DICTSIZE];
    import(dict, size, file_name);
    uint32_t endpoint, candidate;
    int table_id = 0;
    Pair *pair;
    unsigned char hashed[SHA256_DIGEST_LENGTH];
    for (int col = t - 1; col >= 0; col--)
    {
        strcpy((char *restrict)hashed, (char *restrict)cipher);
        chain(&endpoint, hashed, table_id, col);
        if ((pair = get(endpoint, dict)) != NULL)
        {
            candidate = pair->start;
            rebuild(&candidate, hashed, table_id, col);
            hash(&candidate, hashed);
            if (!strcmp((const char *)hashed, (const char *)cipher))
            {
                printf("Hash recovered (sp : %u ; col : %d): %u\n", pair->start, col, candidate);
                free_dict(dict);
                exit(0);
            }
        }
    }
    printf("Hash not recovered T_T\n");
    free_dict(dict);
}