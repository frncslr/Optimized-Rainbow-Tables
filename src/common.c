#include "../include/common.h"

double ALPHA(double r)
{
    return r / (r + 1.0);
}
double R(double alpha)
{
    return alpha / (1.0 - alpha);
}
int M0(uint64_t N, double r, int t)
{
    return (int)round(2 * r * N / (t + 2));
}
int Mt(uint64_t N, double alpha, int t)
{
    return (int)round(2 * alpha * N / (t + 2));
}
int Mi(uint64_t N, int m0, int i)
{
    return (int)round(2.0 * N / (i + (2.0 * N / m0)));
}

void hash(Point *point, unsigned char *digest)
{
    SHA256((const unsigned char *)point, sizeof(Point), digest);
}
void reduce(Point *point, unsigned char *digest, int table_id, int col_id, int t, uint64_t N)
{
    *point = (*(Point *)digest + table_id + col_id * t) % N;
}
void hash_reduce(Point *point, int table_id, int col_id, int t, uint64_t N)
{
    hash(point, buffer);
    reduce(point, buffer, table_id, col_id, t, N);
}
void compute(Point *point, int table_id, int col_start, int col_end, int t, uint64_t N, uint64_t *nb_hash)
{
    for (int col_id = col_start; col_id < col_end; col_id++, (*nb_hash)++)
        hash_reduce(point, table_id, col_id, t, N);
}

int sizeHTable(uint64_t N, int m0, int col_id)
{
    return (int)ceil(LOAD_FACTOR * Mi(N, m0, col_id));
}
void initHTable(HTable *htable, int htsize)
{
    if ((*htable = (Chain *)calloc(htsize, sizeof(Chain))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    for (Chain *current = *htable, *last = *htable + htsize; current < last; current++)
    {
        current->sp = MAX;
        current->ep = MAX;
    }
}
int insert(HTable htable, int htsize, Chain *chain)
{
    Chain *place;
    for (int i = 0; i < htsize; i++)
    {
        place = htable + (chain->ep + i) % htsize;
        if (place->ep == MAX)
        {
            place->sp = chain->sp;
            place->ep = chain->ep;
            return 1;
        }
        if ((place->ep) == chain->ep)
            return 0;
    }
    return 0;
}
Chain *search(HTable htable, int htsize, Point ep)
{
    Chain *chain;
    for (int i = 0; i < 50; i++)
    {
        chain = htable + (ep + i) % htsize;
        if (chain->ep == ep)
            return chain;
        if (chain->ep == MAX)
            return NULL;
    }
    return NULL;
}

double elapsed(timeval *start, timeval *end)
{
    return end->tv_sec - start->tv_sec + (end->tv_usec - start->tv_usec) * 1e-6;
}
void print_hash(unsigned char *digest)
{
    for (unsigned int i = 0; i < HASH_LENGTH; i++)
        printf("%02x", digest[i]);
    printf("\n");
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
}
