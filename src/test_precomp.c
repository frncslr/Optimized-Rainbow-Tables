#include "../include/test_precomp.h"

void test_initialize()
{
    printf("# Test initialize :\n");
    int size = 6;
    Points table0[size], table1[size];
    printf("Initializing two tables of %d elements each\n", size);

    initialize(table0, 0, size);
    printf("Table 0 :");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table0[i].start, table0[i].end);
    printf("\n");

    initialize(table1, 1, size);
    printf("Table 1 :");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table1[i].start, table1[i].end);
    printf("\n\n");
}

void test_generate()
{
    printf("# Test generate :\n");
    int table_id = 0;
    int table_size = 1 << 4;
    int table_width = t;
    int nb_hash = 0;

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }

    time_t s = time(NULL);
    initialize(table, table_id, table_size);
    time_t i = time(NULL);
    printf("Time to init %d : %lds\n", table_size, i - s);

    generate(table, table_id, table_size, table_width, &nb_hash);
    time_t g = time(NULL);
    printf("Time to gen %d : %lds\n", table_size, g - i);

    printf("Table (first 16 rows) :");
    for (Points *current = table, *last = table + 16; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n\n");
    free((void *)table);
}

void test_sort()
{
    printf("# Test sort :\n");
    int size = 1 << 3;
    Points table[size];
    printf("Initializing and generating a table of %d elements\n", size);

    initialize(table, 0, size);

    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);

    printf("Table before sort :");
    for (Points *current = table, *last = table + size; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");

    sort(table, 0, size - 1);

    printf("Table after sort :");
    for (Points *current = table, *last = table + size; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n\n");
}

void test_gen_sort()
{
    printf("# Test gen sort :\n");
    int size = 1 << 15;
    Points *table;
    if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }
    printf("Table size : %d\n", size);
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
    printf("\n\n");
    free((void *)table);
}

void test_clean()
{
    printf("# Test clean :\n");
    int size = 1 << 15;
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
    printf("Table size : %d\n", size);
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
    printf("Table cleaned : (%u rows)", size);
    for (Points *current = perfect, *last = perfect + 20; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    free((void *)table);
    free((void *)perfect);
}

void test_rice()
{
    printf("# Test rice :\n");
    uint32_t previous = 4099;
    printf("Previous endpoint : %u\n", previous);
    uint32_t end = 4115;
    printf("Current endpoint (uncompressed): %u\n", end);
    rice(&end, (end - previous - 1), 3);
    printf("Current endpoint (compressed): %u\n\n", end);
}

void test_export()
{
    printf("# Test export :\n");
    int size = 6;
    Points table[size];
    printf("Initializing and exporting a table of %d elements\n", size);
    initialize(table, 0, size);
    export(table, size, "tableTestExport.dat");
    printf("Table (exported):");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n\n");
}

void test_precomp()
{
    char table_name[] = "tableTestPrecomp.dat";
    int table_size = (int)ceil(m0);
    int table_id = 0;
    int table_width = t;
    precomp(table_name, table_id, &table_size, table_width);
}