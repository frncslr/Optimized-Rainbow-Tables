#include "../include/precomp.h"

void initialize(Points *table, int table_id, int table_size)
{
    Points *current;
    Points *last;
    uint32_t i, offset = table_id * table_size;
    for (i = 0, current = table, last = table + table_size; current < last; current++, i++)
    {
        current->start = i + offset;
        current->end = i + offset;
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

void clean(Points **table, int *table_size, int htable_size)
{
    Hashtable htable;
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    init(htable, htable_size);

    int nb_inserted = 0;
    for (Points *current = *table, *last = *table + *table_size; current < last; current++)
        nb_inserted += insert(htable, htable_size, current->start, current->end);

    if (*table_size != nb_inserted)
    {
        *table_size = nb_inserted;
        if ((*table = (Points *)realloc((void *)(*table), (*table_size) * sizeof(Points))) == NULL)
        {
            printf("Memory allocation problem");
            exit(ERROR_ALLOC);
        }
    }

    for (Points *inserted = *table, *current = htable, *last = htable + htable_size; current < last; current++)
        if (current->end != MAX)
        {
            inserted->start = current->start;
            inserted->end = current->end;
            inserted++;
        }

    free((void *)htable);
}

void generate(Points *table, int table_id, int *table_size, int *filters, int nb_filters, uint32_t *nb_hash, double *computeTime, double *cleanTime)
{
    struct timeval start, end;
    for (int col_start = 0, *col_end = filters, *last = filters + nb_filters; col_end < last; col_start = *(col_end++))
    {
        for (Points *current = table, *last = table + *table_size; current < last; current++)
        {
            gettimeofday(&start, 0);
            compute(&(current->end), table_id, col_start, *col_end, nb_hash);
            gettimeofday(&end, 0);
            *computeTime += elapsed(&start, &end);
        }
        gettimeofday(&start, 0);
        clean(&table, table_size, hsize(*col_end));
        gettimeofday(&end, 0);
        *cleanTime += elapsed(&start, &end);
    }
}

void operations(int *filters, int nb_filters, uint32_t *expec_hash)
{
    double total = 0.0;
    for (int previous = 0, *current = filters, *last = filters + nb_filters; current < last; previous = *(current++))
        total += ceil(mci(previous)) * (*current - previous);
    *expec_hash = (uint32_t)ceil(total);
}

void swap(Points *a, Points *b)
{
    Points tmp = *a;
    *a = *b;
    *b = tmp;
}

int partition(Points *array, int low, int high)
{

    // select the rightmost element as pivot
    Points pivot = array[high];

    // pointer for greater element
    int i = (low - 1);

    // traverse each element of the array
    // compare them with the pivot
    for (int j = low; j < high; j++)
    {
        if (array[j].end <= pivot.end)
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

void quicksort(Points *array, int low, int high)
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

void sort(Points *table, int table_size)
{
    quicksort(table, 0, table_size - 1);
}

void precompute(Points **table, int table_id, int *table_size, int *filters, int nb_filters, uint32_t *nb_hash, double *computeTime, double *cleanTime)
{
    initialize(*table, table_id, *table_size);
    generate(*table, table_id, table_size, filters, nb_filters, nb_hash, computeTime, cleanTime);
    sort(*table, *table_size);
}

void export(Points *table, int table_size, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "ab")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    if ((fwrite(table, sizeof(Points), table_size, file)) != (size_t)table_size)
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

void cover(Points *table, int table_id, int table_size, int table_width, char *covered, int *coverage)
{
    uint32_t point;
    for (Points *current = table, *last = table + table_size; current < last; current++)
    {
        point = current->start;
        for (int col_id = 0; col_id < table_width; col_id++)
        {
            if (!covered[point])
            {
                covered[point] = 1;
                ++*coverage;
            }
            hash_reduction(&point, table_id, col_id);
        }
        if (point != current->end)
        {
            fprintf(stderr, "Cover problem : endpoint not retrieved\n");
            exit(ERROR_COVER);
        }
    }
}

void hashStats(uint32_t nb_hash, int *filters, int nb_filters, int nb_tables)
{
    uint32_t expec_hash = 0;
    operations(filters, nb_filters, &expec_hash);
    expec_hash *= nb_tables;
    int diff_hash = nb_hash - expec_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n");
    printf("\texpected\t: %u\n", expec_hash);
    printf("\texperimental\t: %u\n", nb_hash);
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

void coverStats(int coverage, int space_size, int nb_tables, int table_width, int expec_size)
{
    double expec_coverage_perc = (1.0 - pow(1.0 - (1.0 - pow(1.0 - (double)expec_size / space_size, (double)table_width)), (double)nb_tables)) * 100.0;
    double coverage_perc = coverage * 100.0 / space_size;
    double diff_coverage_perc = coverage_perc - expec_coverage_perc;
    printf("Coverage of the table :\n");
    printf("\texpected\t: %3.2lf%%\n", expec_coverage_perc);
    printf("\texperimental\t: %3.2lf%%\n", coverage_perc);
    printf("\tdifference\t: %3.2lf%%\n\n", diff_coverage_perc);
}
