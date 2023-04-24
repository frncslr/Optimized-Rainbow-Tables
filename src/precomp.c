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

void generate(Points *table, int table_id, int table_size, int ti, int *nb_hash)
{
    unsigned char hashed[SHA256_DIGEST_LENGTH];
    for (Points *current = table, *last = table + table_size; current < last; current++)
        for (int col_id = 0; col_id < ti; col_id++, (*nb_hash)++)
            hash_reduction(&(current->end), hashed, table_id, col_id);
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

void sort(Points *array, int low, int high)
{
    if (low < high)
    {

        // find the pivot element such that
        // elements smaller than pivot are on left of pivot
        // elements greater than pivot are on right of pivot
        int pi = partition(array, low, high);

        // recursive call on the left of pivot
        sort(array, low, pi - 1);

        // recursive call on the right of pivot
        sort(array, pi + 1, high);
    }
}

void clean(Points *table, int *table_size, Points *perfect)
{
    uint32_t previous = table->end - 1;
    Points *new = perfect;
    uint32_t perfect_size = 0;
    for (Points *current = table, *last = table + *table_size; current < last; current++)
        if (current->end > previous)
        {
            *new = *current;
            previous = (new ++)->end;
            perfect_size++;
        }
    *table_size = new - perfect;
}

void rice(uint32_t *end, uint32_t value, char k)
{
    *end = ((1 << (value >> k)) - 1) << (k + 1) | (value & ((1 << k) - 1));
}

void ceri(uint32_t *end, uint32_t value, char k, int nb_bits)
{
    *end = ((nb_bits - k - 1) << k) | (value & ((1 << k) - 1));
}

void export(Points *table, int table_size, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "wb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    if ((fwrite(table, sizeof(Points), table_size, file)) != (size_t) table_size)
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

void import(Points *table, int table_size, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    if ((fread(table, sizeof(Points), table_size, file)) != (size_t) table_size)
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

void precomp(){
    int size = (int) ceil(m0);
    Points *table, *perfect;
    if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    if ((perfect = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    time_t s = time(NULL);
    initialize(table, 0, size);
    time_t i = time(NULL);
    printf("Time to init %d\t: %lds\n", size, i - s);
    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);
    time_t g = time(NULL);
    printf("Time to gen %d\t: %lds\n", size, g - i);
    sort(table, 0, size - 1);
    time_t q = time(NULL);
    printf("Time to sort %d\t: %lds\n", size, q - g);
    clean(table, &size, perfect);
    time_t c = time(NULL);
    printf("Time to clean %d\t: %lds\n", size, c - q);
    if ((perfect = (Points *)realloc((void *)perfect, size * sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    export(table, size, "./table2.dat");
    time_t e = time(NULL);
    printf("Time to export %d\t: %lds\n", size, e - c);
    printf("Hash operations :\n\texpected\t: %d\n\texperimental\t: %d\n", (int) ceil(m0) * t, nb_hash);
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n", (int) ceil(mt), size);
    printf("Table :");
    for (Points *current = perfect, *last = perfect + 20; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    free((void *)table);
    free((void *)perfect);
}