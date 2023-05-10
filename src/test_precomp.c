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
    printf("Table id : %d\n", table_id);
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

    generate(table, table_id, table_size, table_width, &nb_hash);
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
    generate(table, table_id, table_size, table_width, &nb_hash);

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
    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }

    printf("Initializing and generating table %d of %d elements\n", table_id, table_size);

    initialize(table, table_id, table_size);

    int nb_hash = 0;
    generate(table, table_id, table_size, table_width, &nb_hash);

    int htable_size = table_size;
    clean(table, &table_size, htable_size);

    sort(table, table_size);

    printf("Table cleaned and sorted (first 16/%u rows) :\n", table_size);
    for (Points *current = table, *last = table + 16; current < last; current++)
        printf("%u\t:\t%u\n", current->start, current->end);

    printf("\n");
    free((void *)table);
}

void test_precompute()
{
    printf("# Test precompute :\n");
    int table_id = 3;
    int table_size = (int)ceil(m0);
    int expec_size = (int)ceil(mt);
    int table_width = t;
    char table_name[30] = "tableTestPrecomp";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    printf("Precomputing table %d of initially %d rows\n", table_id, table_size);
    int nb_hash = 0;
    precompute(table, table_id, &table_size, table_width, &nb_hash);

    int expec_hash = (int)ceil(m0) * t;
    int diff_hash = expec_hash - nb_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2f%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2f%%)\n", expec_size, table_size, diff_size, diff_size_perc);

    free((void *)table);
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
    int table_id = 1;
    int table_size = 1 << 4;
    char table_name[30] = "tableTestExport.dat";
    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }

    initialize(table, table_id, table_size);

    printf("Exporting table %d intialized, generated, cleaned and sorted\n", table_id);
    export(table, table_size, table_name);

    printf("Table cleaned and sorted (first 16/%u rows) :\n", table_size);
    for (Points *current = table, *last = table + 16; current < last; current++)
        printf("%u\t:\t%u\n", current->start, current->end);

    printf("\n");
    free((void *)table);
}

void test_cover()
{
    printf("# Test cover :\n");
    int table_id = 1;
    int table_size = 1 << 3;
    int table_width = t;
    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }

    initialize(table, table_id, table_size);

    int nb_hash = 0;
    generate(table, table_id, table_size, table_width, &nb_hash);

    printf("Table %d of %d rows initialized :\n", table_id, table_size);
    for (Points *current = table, *last = table + table_size; current < last; current++)
        printf("%u\t:\t%u\n", current->start, current->end);

    int coverage = 0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    cover(table, table_id, table_size, table_width, covered, &coverage);
    printf("Coverage of the table : %d\n\n", coverage);

    free((void *)table);
    free((void *)covered);
}

void test_precompute_full()
{
    printf("# Test precompute full:\n");
    int table_id = 3;
    int table_size = (int)ceil(m0);
    int expec_size = (int)ceil(mt);
    int table_width = t;
    char table_name[30] = "tableTestPrecomp";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    printf("Precomputing table %d of initially %d rows\n", table_id, table_size);
    int nb_hash = 0;
    precompute(table, table_id, &table_size, table_width, &nb_hash);

    int coverage = 0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    cover(table, table_id, table_size, table_width, covered, &coverage);

    export(table, table_size, table_name);

    int expec_hash = (int)ceil(m0) * t;
    int diff_hash = expec_hash - nb_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2f%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2f%%)\n", expec_size, table_size, diff_size, diff_size_perc);

    double expec_coverage_perc = (1 - pow((double)1 - mt / N, (double)t)) * 100;
    double coverage_perc = (double)coverage * 100 / N;
    double diff_coverage_perc = coverage_perc - expec_coverage_perc;
    printf("Coverage of the table :\n\texpected\t: %3.2f%%\n\texperimental\t: %3.2f%%\n\tdifference\t: %3.2f%%\n\n", expec_coverage_perc, coverage_perc, diff_coverage_perc);

    free((void *)table);
    free((void *)covered);
}