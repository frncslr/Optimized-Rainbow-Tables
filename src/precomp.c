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

void generate(Points *table, int table_id, int *table_size, int *filters, int nb_filters, uint32_t *nb_hash)
{
    struct timeval start, end;
    double total_compute, total_clean;
    for (int col_start = 0, *col_end = filters, *last = filters + nb_filters; col_end < last; col_start = *(col_end++))
    {
         gettimeofday(&start, 0);
        for (Points *current = table, *last = table + *table_size; current < last; current++)
            compute(&(current->end), table_id, col_start, *col_end, nb_hash);
        gettimeofday(&end, 0);
        total_compute += elapsed(&start, &end);
         gettimeofday(&start, 0);
        clean(&table, table_size, hsize(*col_end));
        gettimeofday(&end, 0);
        total_clean += elapsed(&start, &end);
    }
    printf("Time to compute\t: %fs\nTime to clean\t: %fs\n", total_compute, total_clean);
}

void operations(int *filters, int nb_filters, uint32_t *expec_hash)
{
    for (int previous = 0, *current = filters, *last = filters + nb_filters; current < last; previous = *(current++))
        *expec_hash += (uint32_t)ceil(mci(previous)) * (*current - previous);
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

void precompute(Points **table, int table_id, int *table_size, int *filters, int nb_filters, uint32_t *nb_hash)
{
    initialize(*table, table_id, *table_size);

    generate(*table, table_id, table_size, filters, nb_filters, nb_hash);
    sort(*table, *table_size);
}

void rice(uint32_t *end, uint32_t value, char k)
{
    *end = ((1 << (value >> k)) - 1) << (k + 1) | (value & ((1 << k) - 1));
}

void export(Points *table, int table_size, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "wb")) == (FILE *)NULL)
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