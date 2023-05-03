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

void sort(Points* table, int table_size){
    quicksort(table, 0, table_size-1);
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

    // Hashtable htable;
    // if ((htable = (Points *)calloc(*table_size, sizeof(Points))) == NULL)
    // {
    //     fprintf(stderr, "Memory allocation problem\n");
    //     exit(ERROR_ALLOC);
    // }
    // init(htable, *table_size);
    // *table_size = 0;
    // for (Points *current = table, *last = table + *table_size; current < last; current++)
    //     *table_size += insert(htable, *table_size, current->start, current->end);
    
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

void precomp(char *table_name, int table_id, int *table_size, int table_width)
{
    printf("Precomputing table %d of initially %d elements\n", table_id, *table_size);
    // strcat(file_name, "0.dat");
    Points *table, *perfect;
    if ((table = (Points *)calloc(*table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    if ((perfect = (Points *)calloc(*table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    time_t s = time(NULL);
    initialize(table, table_id, *table_size);
    time_t i = time(NULL);
    printf("Time to initialize\t: %lds\n", i - s);

    int nb_hash = 0;
    unsigned char buffer[SHA256_DIGEST_LENGTH];
    generate(table, table_id, *table_size, table_width, &nb_hash, buffer);
    time_t g = time(NULL);
    printf("Time to generate\t: %lds\n", g - i);

    sort(table, *table_size);
    time_t q = time(NULL);
    printf("Time to sort\t\t: %lds\n", q - g);

    clean(table, table_size, perfect);
    time_t c = time(NULL);
    printf("Time to clean\t\t: %lds\n", c - q);
    if ((perfect = (Points *)realloc((void *)perfect, *table_size * sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    c = time(NULL);
    export(table, *table_size, table_name);
    time_t e = time(NULL);
    printf("Time to export\t\t: %lds\n", e - c);

    int expec_hash = (int)ceil(m0) * t;
    int diff_hash = expec_hash - nb_hash;
    double diff_hash_perc = diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2f)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_ep = (int)ceil(mt);
    int diff_ep = expec_ep - *table_size;
    double diff_ep_perc = diff_ep * 100 / expec_ep;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2f)\n", expec_ep, *table_size, diff_ep, diff_ep_perc);

    printf("Table (6 first):");
    for (Points *current = perfect, *last = perfect + 16; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    free((void *)table);
    free((void *)perfect);
}