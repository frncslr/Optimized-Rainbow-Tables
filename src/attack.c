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