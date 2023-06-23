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
    char config_mini[30] = "./configs/config_mini.dat";
    positions(&filters, &nb_filters, config_mini);
    printf("Filters in %s :\n", config_mini);
    for (int i = 0; i < nb_filters; i++)
        printf("position %d : %d\n", i, filters[i]);
    printf("\n");
    char config_opti[30] = "./configs/config_opti.dat";
    positions(&filters, &nb_filters, config_opti);
    printf("Filters in %s :\n", config_opti);
    for (int i = 0; i < nb_filters; i++)
        printf("position %d : %d\n", i, filters[i]);
    printf("\n");
    free((void *)filters);
}

void test_clean()
{
    printf("# Test clean :\n");
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

    clean(&table, &table_size, table_size);

    printf("Table after clean :\n");
    for (Points *current = table, *last = table + table_size; current < last; current++)
        printf("%u\t:\t%u\n", current->start, current->end);

    printf("\n");
    free((void *)table);
}

void test_clean_n()
{
    printf("Test clean n :\n");
    int nb_tests = 50;
    int nb_elem = 1 << 24, table_size;
    printf("Cleaning %d points %d times\n", nb_elem, nb_tests);

    Points *table;
    struct timeval start, end;
    double speed, local_time, total_time = 0.0;
    const char file_name[40] = "./data/results/cSpeeds.dat";

    srand(time(NULL));
    for (int i = 0; i < nb_tests; i++)
    {
        table_size = nb_elem;
        if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
        {
            fprintf(stderr, "Memory allocation problem\n");
            exit(ERROR_ALLOC);
        }

        for (Points *current = table, *last = table + table_size; current < last; current++)
            current->end = rand() % N;
        gettimeofday(&start, 0);
        clean(&table, &table_size, table_size);
        gettimeofday(&end, 0);
        local_time = elapsed(&start, &end);
        speed = nb_elem / local_time;
        write_results(&speed, 1, file_name);
        total_time += local_time;
    }
    local_time = total_time / nb_tests;
    printf("Time to hash\t: %lf\n", local_time);
    speed = nb_elem / local_time;
    printf("Clean speed\t: %lf\n", speed);
    printf("\n");
}

void test_generate()
{
    printf("# Test generate :\n");
    int table_id = 0;
    printf("Table index : %d\n", table_id);
    int table_size_init = (int)ceil(m0);
    int table_size = table_size_init;
    printf("Table size  : %d\n", table_size);
    int table_width = t;

    int nb_filters = 1;
    int filters = table_width;

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }

    struct timeval start, end;

    gettimeofday(&start, 0);
    initialize(table, table_id, table_size);
    gettimeofday(&end, 0);
    printf("Time init %d : %lf\n", table_size, elapsed(&start, &end));

    uint32_t nb_hash = 0;
    gettimeofday(&start, 0);
    generate(table, table_id, &table_size, &filters, nb_filters, &nb_hash);
    gettimeofday(&end, 0);
    printf("Time gen %d : %lf\n", table_size, elapsed(&start, &end));

    uint32_t expec_hash = 0;
    operations(&filters, nb_filters, &expec_hash);
    int diff_hash = nb_hash - expec_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %d (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = (int)ceil(mt);
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, table_size, diff_size, diff_size_perc);
    printf("\n");

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

    char file_name[30] = "opti.dat";
    positions(&filters, &nb_filters, file_name);
    expec_hash = 0;
    operations(filters, nb_filters, &expec_hash);
    printf("Expected number of hash operations for %d filters : %u\n", nb_filters, expec_hash);
    printf("\n");
    free(filters);
}

void test_generate_f()
{
    printf("# Test generate f :\n");
    int table_id = 0;
    printf("Table index : %d\n", table_id);
    int table_size = (int)ceil(m0);
    printf("Table size  : %d\n", table_size);
    int nb_filters, *filters = NULL;
    char file_name[30] = "./configs/config_mini.dat";
    positions(&filters, &nb_filters, file_name);
    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }

    struct timeval start, end;
    gettimeofday(&start, 0);
    initialize(table, table_id, table_size);
    gettimeofday(&end, 0);
    printf("Time to initialize\t: %lf\n", elapsed(&start, &end));

    uint32_t nb_hash = 0;
    gettimeofday(&start, 0);
    generate(table, table_id, &table_size, filters, nb_filters, &nb_hash);
    gettimeofday(&end, 0);
    printf("Time to generate\t: %lf\n", elapsed(&start, &end));

    uint32_t expec_hash = 0;
    operations(filters, nb_filters, &expec_hash);
    int diff_hash = nb_hash - expec_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %d (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = (int)ceil(mt);
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, table_size, diff_size, diff_size_perc);

    free((void *)filters);
    free((void *)table);
    printf("\n");
}

void test_sort()
{
    printf("# Test sort :\n");
    int table_id = 1;
    int table_size = 1 << 14;
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
    int table_id = 0;
    int table_size = (int)ceil(m0);
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

    int nb_filters, *filters = NULL;
    char file_name[30] = "configTestPositions.dat";
    positions(&filters, &nb_filters, file_name);

    printf("Precomputing table %d of initially %d rows\n", table_id, table_size);
    uint32_t nb_hash = 0;
    precompute(&table, table_id, &table_size, filters, nb_filters, &nb_hash);

    uint32_t expec_hash = 0;
    operations(filters, nb_filters, &expec_hash);
    int diff_hash = nb_hash - expec_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %d (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = (int)ceil(mt);
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, table_size, diff_size, diff_size_perc);

    free((void *)filters);
    free((void *)table);
}

void test_export()
{
    printf("# Test export :\n");
    int table_id = 3;
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
    int nb_filters = 1;
    int filters = t;
    generate(table, table_id, &table_size, &filters, nb_filters, &nb_hash);

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
    int table_id = 0;
    int table_size = (int)ceil(m0);
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

    int nb_filters, *filters = NULL;
    char file_name[30] = "configTestPositions.dat";
    positions(&filters, &nb_filters, file_name);

    printf("Precomputing table %d of initially %d rows\n", table_id, table_size);
    uint32_t nb_hash = 0;
    precompute(&table, table_id, &table_size, filters, nb_filters, &nb_hash);

    int coverage = 0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    cover(table, table_id, table_size, table_width, covered, &coverage);

    export(table, table_size, table_name);

    uint32_t expec_hash = 0;
    operations(filters, nb_filters, &expec_hash);
    int diff_hash = nb_hash - expec_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %d (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = (int)ceil(mt);
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, table_size, diff_size, diff_size_perc);

    double expec_coverage_perc = (1 - pow((double)1 - mt / N, (double)t)) * 100;
    double coverage_perc = (double)coverage * 100 / N;
    double diff_coverage_perc = coverage_perc - expec_coverage_perc;
    printf("Coverage of the table :\n\texpected\t: %3.2lf%%\n\texperimental\t: %3.2lf%%\n\tdifference\t: %3.2lf%%\n\n", expec_coverage_perc, coverage_perc, diff_coverage_perc);

    free((void *)table);
    free((void *)filters);
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

    int nb_filters, *filters = NULL;
    char file_name[30] = "./configs/config_mini.dat";
    positions(&filters, &nb_filters, file_name);

    printf("Precomputing, exporting and checking the coverage of %d tables\n", nb_tables);
    for (int table_id = 0; table_id < nb_tables; table_id++)
    {
        table_size = init_size;
        if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
        {
            fprintf(stderr, "Memory allocation problem\n");
            exit(ERROR_ALLOC);
        }

        precompute(&table, table_id, &table_size, filters, nb_filters, &nb_hash);

        table_name[name_length] = table_id + '0';
        export(table, table_size, table_name);

        cover(table, table_id, table_size, table_width, covered, &coverage);

        total_size += table_size;

        free((void *)table);
    }

    uint32_t expec_hash = 0;
    operations(filters, nb_filters, &expec_hash);
    expec_hash *= 4;
    int diff_hash = nb_hash - expec_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %d (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = nb_tables * (int)ceil(mt);
    int diff_size = total_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, total_size, diff_size, diff_size_perc);

    double expec_coverage_perc = (1 - pow(1 - (1 - pow((double)1 - mt / N, (double)t)), (double)nb_tables)) * 100;
    double coverage_perc = (double)coverage * 100 / N;
    double diff_coverage_perc = coverage_perc - expec_coverage_perc;
    printf("Coverage of the table :\n\texpected\t: %3.2lf%%\n\texperimental\t: %3.2lf%%\n\tdifference\t: %3.2lf%%\n\n", expec_coverage_perc, coverage_perc, diff_coverage_perc);

    free((void *)filters);
    free((void *)covered);
}

void test_precompute_cde()
{
    printf("# Test precompute_cde :\n");
    int nb_tables = 1;
    int table_id = 0;
    int table_size = (int)ceil(m0);
    int expec_size = (int)ceil(mt);
    int table_width = t;
    int space_size = N;
    int nb_block = L;
    char spFile_name[40] = "data/tables/cde/spPrecompCDE";
    char epFile_name[40] = "data/tables/cde/epPrecompCDE";
    char idxFile_name[40] = "data/tables/cde/idxPrecompCDE";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(spFile_name, extension);
    strcat(epFile_name, extension);
    strcat(idxFile_name, extension);

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    int nb_filters, *filters = NULL;
    char config_name[30] = "data/configs/config_mini.dat";
    printf("Importing filters from %s\n", config_name);
    positions(&filters, &nb_filters, config_name);

    printf("Precomputing table %d of initially %d rows, filtering with %d filters\n", table_id, table_size, nb_filters);
    uint32_t nb_hash = 0;
    precompute(&table, table_id, &table_size, filters, nb_filters, &nb_hash);

    printf("Exporting table in files :\n\t%s\n\t%s\n\t%s\n", spFile_name, epFile_name, idxFile_name);
    exportCDE(table, table_size, space_size, nb_block, spFile_name, epFile_name, idxFile_name);

    int coverage = 0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    printf("Computing the coverage of the table\n");
    cover(table, table_id, table_size, table_width, covered, &coverage);
    printf("\n");

    hashStats(nb_hash, filters, nb_filters, nb_tables);
    epStats(table_size, expec_size, nb_tables);
    coverStats(coverage, space_size, nb_tables, table_width, expec_size);
    cdeStats(nb_tables, &table_size, space_size, nb_block, spFile_name, epFile_name, idxFile_name);

    free((void *)table);
    free((void *)filters);
    free((void *)covered);
}

void test_precompute_cde_ell()
{
    printf("# Test precompute_cde :\n");
    int nb_tables = 4;
    int init_size = (int)ceil(m0);
    int expec_size = (int)ceil(mt);
    int table_width = t;
    int space_size = N;
    int nb_block = L;
    char spFile_name[40] = "data/tables/cde/spCDE";
    char epFile_name[40] = "data/tables/cde/epCDE";
    char idxFile_name[40] = "data/tables/cde/idxCDE";
    int spName_length = strlen((const char *)spFile_name);
    int epName_length = strlen((const char *)epFile_name);
    int idxName_length = strlen((const char *)idxFile_name);
    char extension[6] = "i.dat";
    strcat(spFile_name, extension);
    strcat(epFile_name, extension);
    strcat(idxFile_name, extension);

    Points *table;
    int table_size[nb_tables], total_size = 0;
    uint32_t nb_hash = 0;

    int coverage = 0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    int nb_filters, *filters = NULL;
    char config_name[30] = "data/configs/config_mini.dat";
    printf("Importing filters from %s\n", config_name);
    positions(&filters, &nb_filters, config_name);

    printf("Precomputing %d tables of initially %d rows and filtering with %d filters\n", nb_tables, init_size, nb_filters);
    printf("Checking the coverage of the tables after their export with CDE in files :\n\t%s\n\t%s\n\t%s\n", spFile_name, epFile_name, idxFile_name);
    for (int table_id = 0; table_id < nb_tables; table_id++)
    {
        table_size[table_id] = init_size;
        if ((table = (Points *)calloc(table_size[table_id], sizeof(Points))) == NULL)
        {
            fprintf(stderr, "Memory allocation problem\n");
            exit(ERROR_ALLOC);
        }

        precompute(&table, table_id, &table_size[table_id], filters, nb_filters, &nb_hash);

        spFile_name[spName_length] = table_id + '0';
        epFile_name[epName_length] = table_id + '0';
        idxFile_name[idxName_length] = table_id + '0';
        exportCDE(table, table_size[table_id], space_size, nb_block, spFile_name, epFile_name, idxFile_name);

        cover(table, table_id, table_size[table_id], table_width, covered, &coverage);

        total_size += table_size[table_id];

        free((void *)table);
    }

    hashStats(nb_hash, filters, nb_filters, nb_tables);
    epStats(total_size, expec_size, nb_tables);
    coverStats(coverage, space_size, nb_tables, table_width, expec_size);
    cdeStats(nb_tables, table_size, space_size, nb_block, spFile_name, epFile_name, idxFile_name);
    printf("\n");
    free((void *)filters);
    free((void *)covered);
}
