#include "../include/precomp.h"

void initialize(RTable *table, int table_id, int m)
{
    if ((*table = (Chain *)calloc(m, sizeof(Chain))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }
    Point value = table_id * m;
    for (Chain *current = *table, *last = *table + m; current < last; current++, value++)
    {
        current->sp = value;
        current->ep = value;
    }
}
void clean(RTable *table, int *table_size, int htable_size)
{
    HTable htable;
    initHTable(&htable, htable_size);

    int nb_inserted = 0;
    for (Chain *current = *table, *last = *table + *table_size; current < last; current++)
        nb_inserted += insert(htable, htable_size, current);

    if (*table_size != nb_inserted)
    {
        *table_size = nb_inserted;
        if ((*table = (Chain *)realloc((void *)(*table), (*table_size) * sizeof(Chain))) == NULL)
        {
            printf("Memory allocation problem");
            exit(ERROR_ALLOC);
        }
    }

    for (Chain *inserted = *table, *current = htable, *last = htable + htable_size; current < last; current++)
        if (current->ep != MAX)
        {
            inserted->sp = current->sp;
            inserted->ep = current->ep;
            inserted++;
        }

    free((void *)htable);
}
void generate(RTable table, int table_id, int *m, int *filters, int nb_filters, int t, uint64_t N, uint64_t *nb_hash, double *computeTime, double *cleanTime)
{
    int m0 = *m;
    timeval start, end;
    for (int col_start = 0, *col_end = filters, *last = filters + nb_filters; col_end < last; col_start = *(col_end++))
    {
        for (Chain *current = table, *last = table + *m; current < last; current++)
        {
            gettimeofday(&start, 0);
            compute(&(current->ep), table_id, col_start, *col_end, t, N, nb_hash);
            gettimeofday(&end, 0);
            *computeTime += elapsed(&start, &end);
        }
        gettimeofday(&start, 0);
        clean(&table, m, sizeHTable(N, m0, *col_end));
        gettimeofday(&end, 0);
        *cleanTime += elapsed(&start, &end);
    }
}
void swap(Chain *a, Chain *b)
{
    Chain tmp = *a;
    *a = *b;
    *b = tmp;
}
int partition(RTable array, int low, int high)
{

    // select the rightmost element as pivot
    Chain pivot = array[high];

    // pointer for greater element
    int i = (low - 1);

    // traverse each element of the array
    // compare them with the pivot
    for (int j = low; j < high; j++)
    {
        if (array[j].ep <= pivot.ep)
        {

            // if element smaller than pivot is found
            // swap it with the greater element pointed by i
            i++;

            // swap element at i with element at j
            swap(&array[i], &array[j]);
        }
    }

    // swap the pivot element with the greater element at i
    swap(&array[i + 1], &array[high]);

    // return the partition point
    return (i + 1);
}
void quicksort(RTable array, int low, int high)
{
    if (low < high)
    {

        // find the pivot element such that
        // elements smaller than pivot are on left of pivot
        // elements greater than pivot are on right of pivot
        int pi = partition(array, low, high);

        // recursive call on the left of pivot
        quicksort(array, low, pi - 1);

        // recursive call on the right of pivot
        quicksort(array, pi + 1, high);
    }
}
void sort(RTable table, int m)
{
    quicksort(table, 0, m - 1);
}
void precompute(RTable *table, int table_id, int *m, int *filters, int nb_filters, int t, uint64_t N, uint64_t *nb_hash, double *computeTime, double *cleanTime)
{
    initialize(table, table_id, *m);
    generate(*table, table_id, m, filters, nb_filters, t, N, nb_hash, computeTime, cleanTime);
    sort(*table, *m);
}
void export(RTable table, int m, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "wb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    if ((fwrite(table, sizeof(Chain), m, file)) != (size_t)m)
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
void cover(RTable table, int table_id, int m, int t, uint64_t N, char *covered, int *coverage)
{
    Point point;
    for (Chain *current = table, *last = table + m; current < last; current++)
    {
        point = current->sp;
        for (int col_id = 0; col_id < t; col_id++)
        {
            if (!covered[point])
            {
                covered[point] = 1;
                ++*coverage;
            }
            hash_reduce(&point, table_id, col_id, t, N);
        }
        if (point != current->ep)
        {
            fprintf(stderr, "Cover problem : endpoint not retrieved\n");
            exit(ERROR_COVER);
        }
    }
}

void positions(int **filters, int *nb_filters, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    if ((fread(nb_filters, sizeof(int), 1, file)) != (size_t)1)
    {
        fprintf(stderr, "Reading file problem 1: %s\n", file_name);
        exit(ERROR_FWRITE);
    }

    if ((*filters = (int *)calloc(*nb_filters, sizeof(int))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    if ((fread(*filters, sizeof(int), *nb_filters, file)) != (size_t)*nb_filters)
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
void operations(uint64_t N, int m0, int *filters, int nb_filters, uint64_t *expec_hash)
{
    for (int previous = 0, *current = filters, *last = filters + nb_filters; current < last; previous = *(current++))
        *expec_hash += (uint64_t)(Mi(N, m0, previous) * (*current - previous));
}

void hashStats(uint64_t N, int m0, uint64_t nb_hash, int *filters, int nb_filters, int nb_tables)
{
    uint64_t expec_hash = 0;
    operations(N, m0, filters, nb_filters, &expec_hash);
    expec_hash *= nb_tables;
    int diff_hash = nb_hash - expec_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n");
    printf("\texpected\t: %lu\n", expec_hash);
    printf("\texperimental\t: %lu\n", nb_hash);
    printf("\tdifference\t: %d (%3.2lf%%)\n\n", diff_hash, diff_hash_perc);
}
void epStats(int table_size, int expec_size, int nb_tables)
{
    expec_size *= nb_tables;
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n");
    printf("\texpected\t: %d\n", expec_size);
    printf("\texperimental\t: %d\n", table_size);
    printf("\tdifference\t: %d (%3.2lf%%)\n\n", diff_size, diff_size_perc);
}
void coverStats(int coverage, uint64_t N, int ell, int t, int mt)
{
    double p_theo = 1.0 - pow(1.0 - (double)mt / N, t);
    p_theo = 1 - pow(1.0 - p_theo, ell);
    p_theo *= 100.0;
    double p_exp = (double)coverage / N;
    p_exp *= 100.0;
    double diff_p = p_exp - p_theo;
    printf("Coverage of the table :\n");
    printf("\texpected\t: %3.3lf%%\n", p_theo);
    printf("\texperimental\t: %3.3lf%%\n", p_exp);
    printf("\tdifference\t: %3.3lf%%\n\n", diff_p);
}
