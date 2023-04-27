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

void chain(uint32_t *point, unsigned char *hashed, int table_id, int col_id, int width)
{
    reduction(point, hashed, table_id, col_id);
    for (++col_id; col_id < width; col_id++)
        hash_reduction(point, hashed, table_id, col_id);
}

void rebuild(uint32_t *candidate, unsigned char *hashed, int table_id, int col_id)
{
    for (int col = 0; col < col_id; col++)
        hash_reduction(candidate, hashed, table_id, col);
}

void attack(unsigned char *cipher, char *file_name, int size, int width, uint32_t *plain, char *found)
{
    static Pair *dict[DICTSIZE];
    import(dict, size, file_name);
    int table_id = 0;
    Pair *pair;
    uint32_t endpoint, candidate;
    unsigned char hashed[SHA256_DIGEST_LENGTH + 1];
    for (int col = width - 1; col >= 0; col--)
    {
        strcpy((char *restrict)hashed, (char *restrict)cipher);
        chain(&endpoint, hashed, table_id, col, width);
        if ((pair = get(endpoint, dict)) != NULL)
        {
            candidate = pair->start;
            rebuild(&candidate, hashed, table_id, col);
            hash(&candidate, hashed);
            if (!strcmp((const char *)cipher, (const char *)hashed))
            {
                // printf("Match for start point : %u in column : %d\n", pair->start, col);
                *plain = (int)candidate;
                *found = 1;
                break;
            }
        }
    }
    free_dict(dict);
}