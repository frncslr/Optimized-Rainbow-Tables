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
    // printf("** chain point : %u\n", *point);
    reduction(point, cipher, table_id, col_id);
    // printf("** chain col_id : %d\n", col_id);
    compute(point, hashed, table_id, col_id + 1, table_width, &nb_hash);
    // printf("** chain nb hash : %d\n", nb_hash);
}

void rebuild(uint32_t *candidate, unsigned char *hashed, int table_id, int col_id)
{
    int nb_hash = 0;
    compute(candidate, hashed, table_id, 0, col_id, &nb_hash);
    // printf("reb nb hash : %d\n", nb_hash);

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
            // printf("** start point : %u\n", startpoint);
            
            rebuild(&startpoint, hashed, table_id, col);
            // printf("** rebuilt : %u\n", startpoint);
            
            hash(&startpoint, hashed);
            // printf("** cipher : ");
            // print_hash(cipher);
            // printf("** hashed : ");
            // print_hash(hashed);
            
            if (!memcmp((const char *)cipher, (const char *)hashed, SHA256_DIGEST_LENGTH))
            {
                *result = startpoint;
                *found = 1;
                break;
            }
        }
    }
}