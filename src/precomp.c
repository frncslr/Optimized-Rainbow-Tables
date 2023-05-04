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

void generate(Points *table, int table_id, int table_size, int table_width, int *nb_hash, unsigned char *buffer)
{
    for (Points *current = table, *last = table + table_size; current < last; current++)
        compute(&(current->end), buffer, table_id, 0, table_width, nb_hash);
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

void clean(Points *table, int *table_size, int htable_size)
{
    Hashtable htable;
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    init(htable, htable_size);

    int nb_inserted = 0;
    for (Points *current = table, *last = table + *table_size; current < last; current++)
        nb_inserted += insert(htable, htable_size, current->start, current->end);
    *table_size = nb_inserted;

    if ((table = (Points *)realloc((void *)table, *table_size * sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }

    for (Points *inserted = table, *current = htable, *last = htable + htable_size; current < last; current++)
        if (current->end != MAX)
        {
            inserted->start = current->start;
            inserted->end = current->end;
            inserted++;
        }

    free((void *)htable);
}

void cover(Points *table, int table_id, int table_size, int table_width, int *coverage, unsigned char *buffer)
{
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    *coverage = 0;
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
            hash_reduction(&point, buffer, table_id, col_id);
        }
        if (point != current->end)
            printf("Error : EP not obtained\n");
    }
    free(covered);
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

void precompute(char *table_name, int table_id, int *table_size, int table_width, int *nb_hash, int *coverage)
{
    Points *table;
    if ((table = (Points *)calloc(*table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    time_t s = time(NULL);
    initialize(table, table_id, *table_size);
    time_t i = time(NULL);
    printf("Time to initialize\t: %lds\n", i - s);

    unsigned char buffer[SHA256_DIGEST_LENGTH];
    i = time(NULL);
    generate(table, table_id, *table_size, table_width, nb_hash, buffer);
    time_t g = time(NULL);
    printf("Time to generate\t: %lds\n", g - i);

    int htable_size = (int)ceil(1.5 * mt);
    g = time(NULL);
    clean(table, table_size, htable_size);
    time_t c = time(NULL);
    printf("Time to clean\t\t: %lds\n", c - g);

    c = time(NULL);
    sort(table, *table_size);
    time_t q = time(NULL);
    printf("Time to sort\t\t: %lds\n", q - c);

    q = time(NULL);
    cover(table, table_id, *table_size, table_width, coverage, buffer);
    time_t f = time(NULL);
    printf("Time to cover\t\t: %lds\n", f-q);

    q = time(NULL);
    export(table, *table_size, table_name);
    time_t e = time(NULL);
    printf("Time to export\t\t: %lds\n", e - q);

    free((void *)table);
}