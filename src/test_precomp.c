#include "../include/test_precomp.h"

void test_initialize()
{
    int size = 6;
    Points table[size];
    initialize(table, 1, size);
    printf("Table :");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");
}

void test_generate()
{
    int size = 1 << 15;
    Points *table;
    if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }
    time_t s = time(NULL);
    initialize(table, 0, size);
    time_t i = time(NULL);
    printf("Time to init %d : %lds\n", size, i - s);
    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);
    time_t g = time(NULL);
    printf("Time to gen %d : %lds\n", size, g - i);
    for (Points *current = table, *last = table + 20; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    free((void *)table);
}

void test_swap()
{
    Points a, b;
    a.start = 1;
    a.end = 1;
    b.start = 2;
    b.end = 2;
    printf("Before\t: a = {%d : %d} & b = {%d : %d}\n", a.start, a.end, b.start, b.end);
    swap(&a, &b);
    printf("After\t: a = {%d : %d} & b = {%d : %d}\n", a.start, a.end, b.start, b.end);
}

void test_sort()
{
    int size = 1 << 3;
    Points table[size];
    initialize(table, 0, size);
    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);
    printf("Before :");
    for (Points *current = table, *last = table + size; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    sort(table, 0, size - 1);
    printf("After :");
    for (Points *current = table, *last = table + size; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
}

void test_gen_sort()
{
    int size = 1 << 15;
    Points *table;
    if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }
    time_t s = time(NULL);
    initialize(table, 0, size);
    time_t i = time(NULL);
    printf("Time to init %d : %lds\n", size, i - s);
    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);
    time_t g = time(NULL);
    printf("Time to gen %d : %lds\n", size, g - i);
    sort(table, 0, size - 1);
    time_t q = time(NULL);
    printf("Time to sort %d : %lds\n", size, q - g);
    for (Points *current = table, *last = table + 20; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    free((void *)table);
}

void test_clean()
{
    int size = 1 << 12;
    Points *table, *perfect;
    if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    if ((perfect = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    time_t s = time(NULL);
    initialize(table, 0, size);
    time_t i = time(NULL);
    printf("Time to init %d : %lds\n", size, i - s);
    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);
    time_t g = time(NULL);
    printf("Time to gen %d : %lds\n", size, g - i);
    sort(table, 0, size - 1);
    time_t q = time(NULL);
    printf("Time to sort %d : %lds\n", size, q - g);
    clean(table, &size, perfect);
    time_t c = time(NULL);
    printf("Time to clean %d : %lds\n", size, c - q);
    if ((perfect = (Points *)realloc((void *)perfect, size * sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    printf("Table cleaned : (%u Points)", size);
    for (Points *current = perfect, *last = perfect + 20; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    free((void *)table);
    free((void *)perfect);
}

void test_rice()
{
    uint32_t end = 4115;
    printf("before : %u\n", end);
    uint32_t previous = 4099;
    rice(&end, (end - previous - 1), 3);
    printf("after : %u\n", end);
}

void test_export()
{
    int size = 6;
    Points table[size];
    initialize(table, 1, size);
    int nb_hash = 0;
    generate(table, 1, size, t, &nb_hash);
    sort(table, 0, size - 1);
    printf("Table (exported):");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");
    export(table, size, "./table.dat");
}

void test_precomp()
{
    precomp();
}