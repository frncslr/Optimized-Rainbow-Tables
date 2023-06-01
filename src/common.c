#include "../include/common.h"

double elapsed(struct timeval *start, struct timeval *end)
{
    return end->tv_sec - start->tv_sec + (end->tv_usec - start->tv_usec) * 1e-6;
}

void print_hash(unsigned char *hashed_value)
{
    for (unsigned int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        printf("%02x", hashed_value[i]);
    printf("\n");
}

void hash(uint32_t *point, unsigned char *digest)
{
    SHA256((const unsigned char *)point, sizeof(uint32_t), digest);
}

void reduction(uint32_t *point, unsigned char *digest, int table_id, int col_id)
{
    *point = (*(uint32_t *)digest + table_id + col_id * t) % N;
}

void hash_reduction(uint32_t *point, int table_id, int col_id)
{
    hash(point, buffer);
    reduction(point, buffer, table_id, col_id);
}

void compute(uint32_t *point, int table_id, int col_start, int col_end, uint32_t *nb_hash)
{
    for (int col_id = col_start; col_id < col_end; col_id++, (*nb_hash)++)
        hash_reduction(point, table_id, col_id);
}

void write_results(double *results, int count, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "ab")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    if ((fwrite(results, sizeof(double), count, file)) != (size_t)count)
    {
        fprintf(stderr, "Writing file problem : %s\n", file_name);
        exit(ERROR_FWRITE);
    }

    if (fclose(file))
    {
        fprintf(stderr, "Closing file problem : %s", file_name);
        exit(ERROR_FCLOSE);
    }
}

void read_results(double *results, int count, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    if ((fread(results, sizeof(double), count, file)) != (size_t)count)
    {
        fprintf(stderr, "Reading file problem : %s\n", file_name);
        exit(ERROR_FWRITE);
    }

    if (fclose(file))
    {
        fprintf(stderr, "Closing file problem : %s", file_name);
        exit(ERROR_FCLOSE);
    }

int hsize(int col_id)
{
    return (int)ceil(LOAD_FACTOR * ceil(mci(col_id)));
}

void init(Hashtable hashtable, int size)
{
    for (Points *current = hashtable, *last = hashtable + size; current < last; current++)
        current->end = MAX;
}

int insert(Hashtable hashtable, int size, uint32_t start, uint32_t end)
{
    Points *point;
    for (int i = 0; i < size; i++)
    {
        point = hashtable + (end + i) % size;
        if (point->end == MAX)
        {
            point->end = end;
            point->start = start;
            return 1;
        }
        if ((point->end) == end)
            return 0;
    }
    return 0;
}

Points *search(Hashtable hashtable, int size, uint32_t end)
{
    Points *point;
    for (int i = 0; i < size; i++)
    {
        point = hashtable + (end + i) % size;
        if (point->end == end)
            return point;
    }
    return NULL;
}