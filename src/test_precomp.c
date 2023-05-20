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

void test_positions()
{
    printf("# Test positions :\n");
    int nb_filters, *filters = NULL;
    char file_name[30] = "configTestPositions.dat";
    positions(&filters, &nb_filters, file_name);
    for (int i = 0; i < nb_filters; i++)
        printf("position %d : %d\n", i, filters[i]);
    printf("\n");
    free(filters);
}

void test_clean()
{
    printf("# Test clean :\n");
    int table_id = 1;
    int table_size = 1 << 4;

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }

    srand(time(NULL));
    for (int i = 0; i < 4; i++)
    {
        uint32_t end = rand() % N;
        for (int j = 4 * i; j < 4 * (i + 1); j++)
        {
            table[j].start = j;
            table[j].end = end;
        }
    }

    printf("Table before clean :\n");
    for (Points *current = table, *last = table + table_size; current < last; current++)
        printf("%u\t:\t%u\n", current->start, current->end);

    int htable_size = (int)ceil(LOAD_FACTOR * table_size);
    clean(&table, &table_size, htable_size);

    printf("Table after clean :\n");
    for (Points *current = table, *last = table + table_size; current < last; current++)
        printf("%u\t:\t%u\n", current->start, current->end);

    printf("\n");
    free((void *)table);
}

void test_generate()
{
    printf("# Test generate :\n");
    int table_id = 3;
    printf("Table id : %d\n", table_id);
    int table_size_init = 1 << 10;
    int table_size = table_size_init;
    int table_width = t;
    int nb_filters = 1;
    int filters = t;

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

    uint32_t nb_hash = 0;
    generate(table, table_id, &table_size, &filters, nb_filters, &nb_hash);
    time_t g = time(NULL);
    printf("Time to gen %d : %lds\n", table_size, g - i);

    printf("Hash reductions :\n\texpected\t: %d\n\texperimental\t: %u\n", table_size_init * table_width, nb_hash);
    printf("Table (first 16):");
    for (Points *current = table, *last = table + 16; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n\n");
    free((void *)table);
}

void test_operations()
{
    printf("# Test operations :\n");
    int nb_filters, *filters = NULL;
    uint32_t expec_hash;

    nb_filters = 1;
    int filtres = t;
    filters = &filtres;
    expec_hash = 0;
    operations(filters, nb_filters, &expec_hash);
    printf("Expected number of hash operations for %d filters : %u\n", nb_filters, expec_hash);

    // char file_name[30] = "configTesOperaions.dat";
    // positions(&filters, &nb_filters, file_name);
    nb_filters = 37;
    int filtres2[37] = {9, 20, 32, 45, 58, 73, 88, 105, 122, 141, 160, 180, 202, 224, 247, 271, 296, 322, 349, 377, 406, 436, 467, 499, 532, 565, 600, 636, 672, 710, 748, 788, 828, 870, 912, 955, 1000};
    filters = &filtres2;
    expec_hash = 0;
    operations(filters, nb_filters, &expec_hash);
    printf("Expected number of hash operations for %d filters : %u\n", nb_filters, expec_hash);
    printf("\n");
}

void test_generate_f()
{
    printf("# Test generate f :\n");
    int table_id = 3;
    printf("Table id : %d\n", table_id);
    int table_size_init = (int)ceil(m0);
    int table_size = table_size_init;
    int table_width = t;
    // int nb_filters, *filters = NULL;
    // char file_name[30] = "configTestGenerateF.dat";
    // positions(&filters, &nb_filters, file_name);
    int nb_filters = 37;
    int filters[37] = {9, 20, 32, 45, 58, 73, 88, 105, 122, 141, 160, 180, 202, 224, 247, 271, 296, 322, 349, 377, 406, 436, 467, 499, 532, 565, 600, 636, 672, 710, 748, 788, 828, 870, 912, 955, 1000};
    // nb_filters = 1;
    // filters[0] = t;
    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }

    time_t s = time(NULL);
    initialize(table, table_id, table_size);
    time_t i = time(NULL);
    printf("Time to initialize %d : %lds\n", table_size, i - s);

    uint32_t nb_hash = 0;
    generate(table, table_id, &table_size, filters, nb_filters, &nb_hash);
    time_t g = time(NULL);
    printf("Time to generate %d : %lds\n", table_size, g - i);

    uint32_t expec_hash;
    operations(filters, nb_filters, &expec_hash);
    int diff_hash = nb_hash - expec_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %d (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = (int)ceil(mt);
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, table_size, diff_size, diff_size_perc);

    // free((void *)filters);
    free((void *)table);
    printf("\n");
}

void test_sort()
{
    printf("# Test sort :\n");
    int table_id = 1;
    int table_size = 1 << 14;
    int table_width = t;
    int nb_filters = 1;
    int filters = t;
    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }
    printf("Initializing and generating table %d of %d elements\n", table_id, table_size);

    initialize(table, table_id, table_size);

    uint32_t nb_hash = 0;
    generate(table, table_id, &table_size, &filters, nb_filters, &nb_hash);
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

void test_precompute()
{
    printf("# Test precompute :\n");
    int table_id = 3;
    int table_size = (int)ceil(m0);
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
    uint32_t nb_hash = 0;
    precompute(&table, table_id, &table_size, table_width, &nb_hash);

    uint32_t expec_hash = (uint32_t)ceil(m0) * t;
    uint32_t diff_hash = expec_hash - nb_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %u (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = (int)ceil(mt);
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, table_size, diff_size, diff_size_perc);

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

    uint32_t nb_hash = 0;
    // generate(table, table_id, table_size, table_width, &nb_hash);

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
    char table_name[30] = "tableTestPrecompFull";
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
    uint32_t nb_hash = 0;
    precompute(&table, table_id, &table_size, table_width, &nb_hash);

    int coverage = 0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    cover(table, table_id, table_size, table_width, covered, &coverage);

    export(table, table_size, table_name);

    uint32_t expec_hash = (int)ceil(m0) * t;
    uint32_t diff_hash = expec_hash - nb_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %u (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, table_size, diff_size, diff_size_perc);

    double expec_coverage_perc = (1 - pow((double)1 - mt / N, (double)t)) * 100;
    double coverage_perc = (double)coverage * 100 / N;
    double diff_coverage_perc = coverage_perc - expec_coverage_perc;
    printf("Coverage of the table :\n\texpected\t: %3.2lf%%\n\texperimental\t: %3.2lf%%\n\tdifference\t: %3.2lf%%\n\n", expec_coverage_perc, coverage_perc, diff_coverage_perc);

    free((void *)table);
    free((void *)covered);
}

void test_precompute_full_n()
{
    printf("# Test precompute full n:\n");
    int nb_tables = 4;
    int init_size = (int)ceil(m0);
    int table_width = t;
    char table_name[30] = "tableTestPrecompFullN";
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);

    Points *table;
    int table_size;
    int total_size = 0;
    uint32_t nb_hash = 0;

    int coverage = 0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    printf("Precomputing, exporting and checking the coverage of %d tables\n", nb_tables);
    for (int table_id = 0; table_id < nb_tables; table_id++)
    {
        table_size = init_size;
        if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
        {
            fprintf(stderr, "Memory allocation problem\n");
            exit(ERROR_ALLOC);
        }

        precompute(&table, table_id, &table_size, table_width, &nb_hash);

        table_name[name_length] = table_id + '0';
        export(table, table_size, table_name);

        cover(table, table_id, table_size, table_width, covered, &coverage);

        total_size += table_size;

        free((void *)table);
    }

    uint32_t expec_hash = nb_tables * (int)ceil(m0) * t;
    uint32_t diff_hash = expec_hash - nb_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %u (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = nb_tables * (int)ceil(mt);
    int diff_size = total_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, total_size, diff_size, diff_size_perc);

    double expec_coverage_perc = (1 - pow(1 - (1 - pow((double)1 - mt / N, (double)t)), (double)nb_tables)) * 100;
    double coverage_perc = (double)coverage * 100 / N;
    double diff_coverage_perc = coverage_perc - expec_coverage_perc;
    printf("Coverage of the table :\n\texpected\t: %3.2lf%%\n\texperimental\t: %3.2lf%%\n\tdifference\t: %3.2lf%%\n\n", expec_coverage_perc, coverage_perc, diff_coverage_perc);

    free((void *)covered);
}