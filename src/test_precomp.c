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
    int table_id = 3;
    int table_size = 1 << 10;
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


    unsigned char buffer[SHA256_DIGEST_LENGTH];
    generate(table, table_id, table_size, table_width, &nb_hash, buffer);
    time_t g = time(NULL);
    printf("Time to gen %d : %lds\n", table_size, g - i);

    printf("Hash reductions :\n\texpected\t: %d\n\texperimental\t: %d\n", table_size * table_width, nb_hash);
    printf("Table (first 16):");
    for (Points *current = table, *last = table + 16; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n\n");
    free((void *)table);
}

void test_sort()
{
    printf("# Test sort :\n");
    int table_id = 1;
    int table_size = 1 << 15;
    int table_width = t;
    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }
    printf("Initializing and generating table %d of %d elements\n", table_id, table_size);

    initialize(table, table_id, table_size);

    int nb_hash = 0;
    unsigned char buffer[SHA256_DIGEST_LENGTH];
    generate(table, table_id, table_size, table_width, &nb_hash, buffer);

    printf("Table before sort (first 16):");
    for (Points *current = table, *last = table + 16; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");

    sort(table, table_size);

    printf("Table after sort (first 16) :");
    for (Points *current = table, *last = table + 16; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    
    printf("\n\n");
    free((void *)table);

}

void test_clean()
{
    printf("# Test clean :\n");
    int table_id = 1;
    int table_size = 1 << 15;
    int table_width = t;
    Points *table, *perfect;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }
    if ((perfect = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    printf("Initializing, generating and sorting table %d of %d elements\n", table_id, table_size);

    initialize(table, table_id, table_size);

    int nb_hash = 0;
    unsigned char buffer[SHA256_DIGEST_LENGTH];
    generate(table, table_id, table_size, table_width, &nb_hash, buffer);

    sort(table, table_size);

    printf("Table before clean (first 16) : (%u rows)\n", table_size);
    for (Points *current = table, *last = table + 16; current < last; current++)
        printf("%u\t:\t%u\n", current->start, current->end);
    
    clean(table, &table_size, perfect);
    if ((perfect = (Points *)realloc((void *)perfect, table_size * sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    printf("Table cleaned (first 16): (%u rows)\n", table_size);
    for (Points *current = perfect, *last = perfect + 20; current < last; current++)
        printf("%u\t:\t%u\n", current->start, current->end);
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
    printf("# Test precomp :\n");
    char table_name[] = "tableTestPrecomp.dat";
    int table_id = 0;
    int table_size = (int)ceil(m0);
    int table_width = t;
    precomp(table_name, table_id, &table_size, table_width);
}