#include "../include/test_offline.h"

void test_initialize()
{
    printf("# Test initialize :\n");
    int m = 6;
    RTable table0, table1;
    printf("Initializing two tables of %d elements each\n", m);

    initialize(&table0, 0, m);
    printf("Table 0 :");
    for (int i = 0; i < m; i++)
        printf("\n%lu\t:\t%lu", table0[i].sp, table0[i].ep);
    printf("\n");

    initialize(&table1, 1, m);
    printf("Table 1 :");
    for (int i = 0; i < m; i++)
        printf("\n%lu\t:\t%lu", table1[i].sp, table1[i].ep);
    printf("\n\n");
    free((void *)table0);
    free((void *)table1);
}
void test_clean()
{
    printf("# Test clean :\n");
    int m = 1 << 4;
    int N = 1 << 24;

    RTable table;
    initialize(&table, 0, m);

    srand(time(NULL));
    for (int i = 0; i < 4; i++)
    {
        uint32_t end = rand() % N;
        for (int j = 4 * i; j < 4 * (i + 1); j++)
        {
            table[j].sp = j;
            table[j].ep = end;
        }
    }

    printf("Table before clean :\n");
    for (Chain *current = table, *last = table + m; current < last; current++)
        printf("%lu\t:\t%lu\n", current->sp, current->ep);

    clean(&table, &m, m);

    printf("Table after clean :\n");
    for (Chain *current = table, *last = table + m; current < last; current++)
        printf("%lu\t:\t%lu\n", current->sp, current->ep);

    printf("\n");
    free((void *)table);
}
void test_clean_n()
{
    printf("Test clean n :\n");
    int nb_tests = 100;
    int N = 1 << 24;
    double r = 20.0;
    int t = 1000;
    int m, m0 = M0(N, r, t), m_total = 0;
    int nb_filters, *filters = NULL;
    char config_mini[40] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, config_mini);
    printf("Cleaning points with %d filters %d times\n", nb_filters, nb_tests);

    RTable table;
    timeval start, end;
    double speed, local_time, global_time = 0.0;
    const char file_name[40] = "data/results/cSpeedsHTG.dat";

    srand(time(NULL));
    for (int i = 0, col; i < nb_tests; i++)
    {
        col = 0;
        speed = local_time = 0.0;
        for (int j = 0; j < nb_filters; j++)
        {
            m = (int)ceil(Mi(N, m0, col));
            col = filters[j];
            m_total += m;
            speed += (double)m;
            if ((table = (Chain *)calloc(m, sizeof(Chain))) == NULL)
            {
                fprintf(stderr, "Memory allocation problem\n");
                exit(ERROR_ALLOC);
            }
            for (Chain *current = table, *last = table + m; current < last; current++)
                current->ep = rand() % N;

            gettimeofday(&start, 0);
            clean(&table, &m, sizeHTable(N, m0, col));
            gettimeofday(&end, 0);
            local_time += elapsed(&start, &end);
            free((void *)table);
        }
        speed /= local_time;
        write_results(&speed, 1, file_name);
        global_time += local_time;
    }
    m_total /= nb_tests;
    global_time /= nb_tests;
    printf("Time to clean %d\t: %lf\n", m_total, global_time);
    speed = m_total / global_time;
    printf("Clean speed\t\t: %lf\n", speed);
    printf("\n");
}
void test_generate()
{
    printf("# Test generate :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n\tN\t: %lu\n\tr\t: %3.3f\n\talpha\t: %3.3f\n\n", N, r, alpha);

    int table_id = 0;
    int t = 1000;
    int m0 = M0(N, r, t);
    int m = m0;
    printf("Table parameters :\n\tid\t: %d\n\tt\t: %d\n\tm0\t: %d\n\n", table_id, t, m0);
    int mt = Mt(N, alpha, t);

    int nb_filters = 1;
    int *filters = &t;
    printf("Filtration parameters :\n\tnb\t: %d\n\tf\t: ", nb_filters);
    for (int i = 0; i < nb_filters; i++)
        printf("%d ", filters[i]);
    printf("\n\n");

    RTable table;
    initialize(&table, table_id, m);

    uint64_t hash_expe = 0, hash_theo = 0;
    double cleanTime = 0.0, computeTime = 0.0;
    generate(table, table_id, &m, filters, nb_filters, t, N, &hash_expe, &computeTime, &cleanTime);
    operations(N, m0, filters, nb_filters, &hash_theo);

    printf("Time to compute\t\t: %f\n", computeTime);
    printf("Time to clean\t\t: %f\n", cleanTime);
    printf("Time to generate\t: %f\n\n", computeTime + cleanTime);

    hashStats(hash_expe, hash_theo);
    epStats(m, mt);

    free((void *)table);
}
void test_generate_f()
{
    printf("# Test generate f :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n\tN\t: %lu\n\tr\t: %3.3f\n\talpha\t: %3.3f\n\n", N, r, alpha);

    int table_id = 0;
    int t = 1000;
    int m0 = M0(N, r, t);
    int m = m0;
    printf("Table parameters :\n\tid\t: %d\n\tt\t: %d\n\tm0\t: %d\n\n", table_id, t, m0);
    int mt = Mt(N, alpha, t);

    int nb_filters, *filters = NULL;
    char filters_file[40] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, filters_file);
    printf("Filtration parameters :\n\tnb\t: %d\n\tfile\t: %s\n\n", nb_filters, filters_file);

    RTable table;
    initialize(&table, table_id, m);

    uint64_t hash_expe = 0, hash_theo = 0;
    double cleanTime = 0.0;
    double computeTime = 0.0;
    generate(table, table_id, &m, filters, nb_filters, t, N, &hash_expe, &computeTime, &cleanTime);
    operations(N, m0, filters, nb_filters, &hash_theo);

    printf("Time to compute\t\t: %f\n", computeTime);
    printf("Time to clean\t\t: %f\n", cleanTime);
    printf("Time to generate\t: %f\n\n", computeTime + cleanTime);

    hashStats(hash_expe, hash_theo);
    epStats(m, mt);

    free((void *)table);
    free((void *)filters);
    printf("\n");
}
void test_sort()
{
    printf("# Test sort :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n\tN\t: %lu\n\tr\t: %3.3f\n\talpha\t: %3.3f\n\n", N, r, alpha);

    int table_id = 0;
    int t = 1000;
    int m0 = M0(N, r, t);
    int m = m0;
    printf("Table parameters :\n\tid\t: %d\n\tt\t: %d\n\tm0\t: %d\n\n", table_id, t, m0);
    int mt = Mt(N, alpha, t);

    int nb_filters, *filters = NULL;
    char filters_file[40] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, filters_file);
    printf("Filtration parameters :\n\tnb\t: %d\n\tfile\t: %s\n\n", nb_filters, filters_file);

    RTable table;
    initialize(&table, table_id, m);

    uint64_t nb_hash = 0;
    double cleanTime = 0.0;
    double computeTime = 0.0;
    generate(table, table_id, &m, filters, nb_filters, t, N, &nb_hash, &computeTime, &cleanTime);

    printf("Table before sort (first 16):");
    for (Chain *current = table, *last = table + 16; current < last; current++)
        printf("\n%lu\t:\t%lu", current->sp, current->ep);
    printf("\n");

    sort(table, m);

    printf("Table after sort (first 16) :");
    for (Chain *current = table, *last = table + 16; current < last; current++)
        printf("\n%lu\t:\t%lu", current->sp, current->ep);

    printf("\n\n");
    free((void *)table);
}
void test_precompute()
{
    printf("# Test precompute :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n\tN\t: %lu\n\tr\t: %3.3f\n\talpha\t: %3.3f\n\n", N, r, alpha);

    int table_id = 0;
    int t = 1000;
    int m0 = M0(N, r, t);
    int m = m0;
    printf("Table parameters :\n\tid\t: %d\n\tt\t: %d\n\tm0\t: %d\n\n", table_id, t, m0);
    int mt = Mt(N, alpha, t);

    int nb_filters, *filters = NULL;
    char filters_file[40] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, filters_file);
    printf("Filtration parameters :\n\tnb\t: %d\n\tfile\t: %s\n\n", nb_filters, filters_file);

    RTable table;

    uint64_t hash_expe = 0, hash_theo = 0;
    double computeTime = 0.0;
    double cleanTime = 0.0;
    precompute(&table, table_id, &m, filters, nb_filters, t, N, &hash_expe, &computeTime, &cleanTime);
    operations(N, m0, filters, nb_filters, &hash_theo);

    printf("Time to compute\t\t: %f\n", computeTime);
    printf("Time to clean\t\t: %f\n", cleanTime);
    printf("Time to generate\t: %f\n\n", computeTime + cleanTime);

    hashStats(hash_expe, hash_theo);
    epStats(m, mt);

    free((void *)table);
    free((void *)filters);
}
void test_export()
{
    printf("# Test export :\n");
    RTable table;
    int m = 4;
    int table_id = 2;
    char table_name[40] = "data/tables/std/tableTestExport";
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);
    table_name[name_length] = table_id + '0';

    initialize(&table, table_id, m);

    printf("Exporting table %d intialized in file %s\n\n", table_id, table_name);
    export(table, m, table_name);

    free((void *)table);
}
void test_cover()
{
    printf("# Test cover :\n");
    uint64_t N = 1 << 24;
    int table_id = 1;
    int m = 1 << 3;
    int t = 1000;

    RTable table;

    initialize(&table, table_id, m);

    uint64_t nb_hash = 0;
    double computeTime = 0.0;
    double cleanTime = 0.0;
    int nb_filters = 1;
    int filters = t;
    generate(table, table_id, &m, &filters, nb_filters, t, N, &nb_hash, &computeTime, &cleanTime);

    printf("Table %d of %d rows and %d columns :\n", table_id, m, t);
    for (Chain *current = table, *last = table + m; current < last; current++)
        printf("%lu\t:\t%lu\n", current->sp, current->ep);

    double coverage = 0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    cover(table, table_id, m, t, N, covered, &coverage);
    printf("Coverage of the table : %3.3f\n\n", coverage);

    free((void *)table);
    free((void *)covered);
}

void test_positions()
{
    printf("# Test positions :\n");
    int nb_filters, *filters = NULL;
    char config_mini[40] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, config_mini);
    printf("Filters in %s :\n", config_mini);
    for (int i = 0; i < nb_filters; i++)
        printf("position %d : %d\n", i, filters[i]);
    printf("\n");
    char config_opti[30] = "data/configs/config_opti.dat";
    positions(&filters, &nb_filters, config_opti);
    printf("Filters in %s :\n", config_opti);
    for (int i = 0; i < nb_filters; i++)
        printf("position %d : %d\n", i, filters[i]);
    printf("\n");
    free((void *)filters);
}
void test_operations()
{
    printf("# Test operations :\n");
    uint64_t N = 1 << 24;
    int r = 20.0;
    int t = 1000;
    int m0 = M0(N, r, t);

    int nb_filters, *filters = NULL;
    uint64_t expec_hash;

    nb_filters = 1;
    int filtres = t;
    filters = &filtres;
    expec_hash = 0;
    operations(N, m0, filters, nb_filters, &expec_hash);
    printf("Expected number of hash operations for %d filters : %lu\n", nb_filters, expec_hash);

    char file_name[30] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, file_name);
    expec_hash = 0;
    operations(N, m0, filters, nb_filters, &expec_hash);
    printf("Expected number of hash operations for %d filters : %lu\n", nb_filters, expec_hash);
    printf("\n");
    free(filters);
}

void test_offline()
{
    printf("# Test offline :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n\tN\t: %lu\n\tr\t: %3.3f\n\talpha\t: %3.3f\n\n", N, r, alpha);

    int ell = 1;
    int table_id = 0;
    int t = 1000;
    int m0 = M0(N, r, t);
    int m = m0;
    printf("Table parameters :\n\tid\t: %d\n\tt\t: %d\n\tm0\t: %d\n\n", table_id, t, m0);
    int mt = Mt(N, alpha, t);

    int nb_filters, *filters = NULL;
    char filters_file[40] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, filters_file);
    printf("Filtration parameters :\n\tnb\t: %d\n\tfile\t: %s\n\n", nb_filters, filters_file);

    RTable table;

    uint64_t hash_expe = 0, hash_theo = 0;
    double computeTime = 0.0;
    double cleanTime = 0.0;
    precompute(&table, table_id, &m, filters, nb_filters, t, N, &hash_expe, &computeTime, &cleanTime);
    operations(N, m0, filters, nb_filters, &hash_theo);

    printf("Time to compute\t\t: %f\n", computeTime);
    printf("Time to clean\t\t: %f\n", cleanTime);
    printf("Time to generate\t: %f\n\n", computeTime + cleanTime);

    double p_expe = 0.0, p_theo = 1.0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    cover(table, table_id, m, t, N, covered, &p_expe);
    p_theo *= pow(1.0 - (double)Mt(N, alpha, t) / N, t);
    p_expe = 100.0 * p_expe / N;
    p_theo = (1 - p_theo) * 100.0;

    char table_name[30] = "tableTestPrecompFull";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    export(table, m, table_name);

    hashStats(hash_expe, hash_theo);
    epStats(m, mt);
    coverStats(p_expe, p_theo);

    free((void *)table);
    free((void *)filters);
    free((void *)covered);
}
void test_offline_ell()
{
    printf("# Test offline ell :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n");
    printf("\tN\t: %lu\n", N);
    printf("\tr\t: %3.3f\n", r);
    printf("\talpha\t: %3.3f\n\n", alpha);

    int ell = 4;
    int t = 1000;
    int m0 = M0(N, r, t);
    char table_name[40] = "data/tables/std/tableSTD";
    printf("Table parameters :\n");
    printf("\tell\t: %d\n", ell);
    printf("\tt\t: %d\n", t);
    printf("\tm0\t: %d\n", m0);
    printf("\tfile\t: %s\n\n", table_name);
    int m, mt = Mt(N, alpha, t);
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);

    int nb_filters, *filters = NULL;
    char filters_file[40] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, filters_file);
    printf("Filtration parameters :\n");
    printf("\tnb\t: %d\n", nb_filters);
    printf("\tfile\t: %s\n\n", filters_file);

    RTable table;

    int m_total = 0;
    uint64_t hash_expe = 0, hash_theo = 0;
    double computeTime = 0.0;
    double cleanTime = 0.0;

    double p_expe = 1.0, p_theo = 0.0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    printf("Precomputing, exporting and checking the coverage of %d tables\n", ell);
    for (int table_id = 0; table_id < ell; table_id++)
    {
        m = m0;

        precompute(&table, table_id, &m, filters, nb_filters, t, N, &hash_expe, &computeTime, &cleanTime);
        operations(N, m0, filters, nb_filters, &hash_theo);

        table_name[name_length] = table_id + '0';
        export(table, m, table_name);

        cover(table, table_id, m, t, N, covered, &p_expe);
        p_theo *= pow(1.0 - (double)Mt(N, alpha, t) / N, t);

        m_total += m;

        free((void *)table);
    }
    p_expe = 100.0 * p_expe / N;
    p_theo = (1 - p_theo) * 100.0;

    printf("Time to compute\t\t: %f seconds\n", computeTime / ell);
    printf("Time to clean\t\t: %f seconds\n", cleanTime / ell);
    printf("Time to generate\t: %f seconds\n\n", (computeTime + cleanTime) / ell);

    hashStats(hash_expe, hash_theo);
    epStats(m_total, mt * ell);
    coverStats(p_expe, p_theo);

    free((void *)filters);
    free((void *)covered);
}
void test_offline_cde()
{
    printf("# Test offline cde :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n");
    printf("\tN\t: %lu\n", N);
    printf("\tr\t: %3.3f\n", r);
    printf("\talpha\t: %3.3f\n\n", alpha);

    int ell = 1;
    int table_id = 0;
    int t = 1000;
    int m0 = M0(N, r, t);
    printf("Table parameters :\n");
    printf("\tell\t: %d\n", ell);
    printf("\tid\t: %d\n", table_id);
    printf("\tt\t: %d\n", t);
    printf("\tm0\t: %d\n", m0);
    int m = m0, mt = Mt(N, alpha, t);

    int nb_filters, *filters = NULL;
    char filters_file[40] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, filters_file);
    printf("Filtration parameters :\n");
    printf("\tnb\t: %d\n", nb_filters);
    printf("\tfile\t: %s\n\n", filters_file);

    char spFile_name[40] = "data/tables/cde/spCDE";
    char epFile_name[40] = "data/tables/cde/epCDE";
    char idxFile_name[40] = "data/tables/cde/idxCDE";
    printf("Delta Encoding parameters :\n");
    printf("\tsp file\t: %s\n", spFile_name);
    printf("\tep file\t: %s\n", epFile_name);
    printf("\tid file\t: %s\n\n", idxFile_name);
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(spFile_name, extension);
    strcat(epFile_name, extension);
    strcat(idxFile_name, extension);

    RTable table;

    uint64_t hash_expe = 0, hash_theo = 0;
    double computeTime = 0.0;
    double cleanTime = 0.0;
    printf("Precomputing, exporting and checking the coverage of %d tables\n", ell);
    precompute(&table, table_id, &m, filters, nb_filters, t, N, &hash_expe, &computeTime, &cleanTime);
    operations(N, m0, filters, nb_filters, &hash_theo);

    printf("Time to compute\t\t: %f\n", computeTime);
    printf("Time to clean\t\t: %f\n", cleanTime);
    printf("Time to generate\t: %f\n\n", computeTime + cleanTime);

    double p_expe = 1.0, p_theo = 0.0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    cover(table, table_id, m, t, N, covered, &p_expe);
    p_theo *= pow(1.0 - (double)Mt(N, alpha, t) / N, t);
    p_expe = 100.0 * p_expe / N;
    p_theo = (1 - p_theo) * 100.0;

    int L = Lblocks(m);
    exportCDE(table, m, N, L, spFile_name, epFile_name, idxFile_name);

    hashStats(hash_expe, hash_theo);
    epStats(m, mt);
    coverStats(p_expe, p_theo);
    cdeStats(ell, &table_id, &m, N, &L, spFile_name, epFile_name, idxFile_name);

    free((void *)table);
    free((void *)filters);
    free((void *)covered);
}
void test_offline_cde_ell()
{
    printf("# Test offline cde ell:\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n");
    printf("\tN\t: %lu\n", N);
    printf("\tr\t: %3.3f\n", r);
    printf("\talpha\t: %3.3f\n\n", alpha);

    int ell = 4;
    int t = 1000;
    int m0 = M0(N, r, t);
    printf("Table parameters :\n");
    printf("\tell\t: %d\n", ell);
    printf("\tt\t: %d\n", t);
    printf("\tm0\t: %d\n", m0);
    int m[ell], mt[ell];

    int nb_filters, *filters = NULL;
    char filters_file[40] = "data/configs/config_mini.dat";
    positions(&filters, &nb_filters, filters_file);
    printf("Filtration parameters :\n");
    printf("\tnb\t: %d\n", nb_filters);
    printf("\tfile\t: %s\n\n", filters_file);

    char spFile_name[40] = "data/tables/cde/spCDE";
    char epFile_name[40] = "data/tables/cde/epCDE";
    char idxFile_name[40] = "data/tables/cde/idxCDE";
    printf("Delta Encoding parameters :\n");
    printf("\tsp file\t: %s\n", spFile_name);
    printf("\tep file\t: %s\n", epFile_name);
    printf("\tid file\t: %s\n\n", idxFile_name);
    int spName_length = strlen((const char *)spFile_name);
    int epName_length = strlen((const char *)epFile_name);
    int idxName_length = strlen((const char *)idxFile_name);
    char extension[6] = "i.dat";
    strcat(spFile_name, extension);
    strcat(epFile_name, extension);
    strcat(idxFile_name, extension);
    int L[ell];
    int tables_id[ell];

    RTable table;

    uint64_t hash_expe = 0, hash_theo = 0;
    int mt_expe = 0, mt_theo = 0;
    double computeTime = 0.0;
    double cleanTime = 0.0;

    double p_expe = 1.0, p_theo = 0.0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    printf("Precomputing, exporting and checking the coverage of %d tables\n", ell);
    for (int table_id = 0; table_id < ell; table_id++)
    {
        m[table_id] = m0;
        mt[table_id] = Mt(N, alpha, t);
        tables_id[table_id] = table_id;

        precompute(&table, table_id, &m[table_id], filters, nb_filters, t, N, &hash_expe, &computeTime, &cleanTime);
        operations(N, m0, filters, nb_filters, &hash_theo);
        L[table_id] = Lblocks(m[table_id]);

        spFile_name[spName_length] = table_id + '0';
        epFile_name[epName_length] = table_id + '0';
        idxFile_name[idxName_length] = table_id + '0';
        exportCDE(table, m[table_id], N, L[table_id], spFile_name, epFile_name, idxFile_name);

        cover(table, table_id, m[table_id], t, N, covered, &p_expe);
        p_theo *= pow(1.0 - (double)Mt(N, alpha, t) / N, t);

        mt_expe += m[table_id];
        mt_theo += mt[table_id];

        free((void *)table);
    }
    p_expe = 100.0 * p_expe / N;
    p_theo = (1 - p_theo) * 100.0;
    free((void *)covered);
    free((void *)filters);

    printf("Time to compute\t\t: %f seconds\n", computeTime / ell);
    printf("Time to clean\t\t: %f seconds\n", cleanTime / ell);
    printf("Time to generate\t: %f seconds\n\n", (computeTime + cleanTime) / ell);

    hashStats(hash_expe, hash_theo);
    epStats(mt_expe, mt_theo);
    coverStats(p_expe, p_theo);
    cdeStats(ell, tables_id, m, N, L, spFile_name, epFile_name, idxFile_name);
}

void test_offline_htg()
{
    printf("# Test offline htg :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n");
    printf("\tN\t: %lu\n", N);
    printf("\tr\t: %3.3f\n", r);
    printf("\talpha\t: %3.3f\n\n", alpha);

    int ell = 4;
    int t[] = {573, 1063, 1561, 2048};
    int m0[ell], mt[ell], m[ell];
    char table_name[40] = "data/tables/htg/tableHTG";
    printf("Table parameters :\n");
    printf("\tell\t: %d\n", ell);
    printf("\tfile\t: %s\n\n", table_name);
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);

    int nb_filters, *filters = NULL;
    char filters_file[40] = "data/configs/config_mini_HTG";
    printf("Filtration parameters :\n");
    printf("\tfile\t: %s\n\n", filters_file);
    int filters_length = strlen((const char *)filters_file);
    strcat(filters_file, extension);

    RTable table;

    int mt_expe = 0, mt_theo = 0;
    uint64_t hash_expe = 0, hash_theo = 0;
    double computeTime = 0.0, cleanTime = 0.0;

    double p_theo = 1.0, p_expe = 0.0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    printf("Precomputing, exporting and checking the coverage of %d tables\n", ell);
    for (int table_id = 0; table_id < ell; table_id++)
    {
        m[table_id] = m0[table_id] = M0(N, r, t[table_id]);
        mt[table_id] = Mt(N, alpha, t[table_id]);

        filters_file[filters_length] = table_id + '0';
        positions(&filters, &nb_filters, filters_file);

        precompute(&table, table_id, &m[table_id], filters, nb_filters, t[table_id], N, &hash_expe, &computeTime, &cleanTime);
        operations(N, m0[table_id], filters, nb_filters, &hash_theo);

        table_name[name_length] = table_id + '0';
        export(table, m[table_id], table_name);

        cover(table, table_id, m[table_id], t[table_id], N, covered, &p_expe);
        p_theo *= pow(1.0 - (double)Mt(N, alpha, t[table_id]) / N, t[table_id]);

        mt_expe += m[table_id];
        mt_theo += mt[table_id];

        free((void *)filters);
        free((void *)table);
    }
    p_expe = 100.0 * p_expe / N;
    p_theo = (1 - p_theo) * 100.0;
    free((void *)covered);

    printf("Time to compute\t\t: %f seconds\n", computeTime / ell);
    printf("Time to clean\t\t: %f seconds\n", cleanTime / ell);
    printf("Time to generate\t: %f seconds\n\n", (computeTime + cleanTime) / ell);

    hashStats(hash_expe, hash_theo);
    epStats(mt_expe, mt_theo);
    coverStats(p_expe, p_theo);
}
void test_offline_htg_cde()
{
    printf("# Test offline htg cde :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n");
    printf("\tN\t: %lu\n", N);
    printf("\tr\t: %3.3f\n", r);
    printf("\talpha\t: %3.3f\n\n", alpha);

    int ell = 4;
    int t[] = {573, 1063, 1561, 2048};
    int m0[ell], mt[ell], m[ell];
    printf("Table parameters :\n");
    printf("\tell\t: %d\n", ell);
    printf("\tt\t: ");
    for (int i = 0; i < ell; i++)
        printf("%d ", t[i]);
    printf("\n");

    int nb_filters, *filters = NULL;
    char filters_file[40] = "data/configs/config_mini_HTG";
    printf("Filtration parameters :\n");
    printf("\tfile\t: %s\n\n", filters_file);
    int filters_length = strlen((const char *)filters_file);
    char extension[6] = "i.dat";
    strcat(filters_file, extension);

    char spFile_name[40] = "data/tables/mix/spCDExHTG";
    char epFile_name[40] = "data/tables/mix/epCDExHTG";
    char idxFile_name[40] = "data/tables/mix/idxCDExHTG";
    printf("Delta Encoding parameters :\n");
    printf("\tsp file\t: %s\n", spFile_name);
    printf("\tep file\t: %s\n", epFile_name);
    printf("\tid file\t: %s\n\n", idxFile_name);
    int spName_length = strlen((const char *)spFile_name);
    int epName_length = strlen((const char *)epFile_name);
    int idxName_length = strlen((const char *)idxFile_name);
    strcat(spFile_name, extension);
    strcat(epFile_name, extension);
    strcat(idxFile_name, extension);
    int tables_id[ell], L[ell];

    RTable table;

    int mt_expe = 0, mt_theo = 0;
    uint64_t hash_expe = 0, hash_theo = 0;
    double computeTime = 0.0, cleanTime = 0.0;

    double p_theo = 1.0, p_expe = 0.0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    printf("Precomputing, exporting and checking the coverage of %d tables\n", ell);
    for (int table_id = 0; table_id < ell; table_id++)
    {
        m[table_id] = m0[table_id] = M0(N, r, t[table_id]);
        mt[table_id] = Mt(N, alpha, t[table_id]);
        tables_id[table_id] = table_id;

        filters_file[filters_length] = table_id + '0';
        positions(&filters, &nb_filters, filters_file);

        precompute(&table, table_id, &m[table_id], filters, nb_filters, t[table_id], N, &hash_expe, &computeTime, &cleanTime);
        operations(N, m0[table_id], filters, nb_filters, &hash_theo);
        L[table_id] = Lblocks(m[table_id]);

        spFile_name[spName_length] = table_id + '0';
        epFile_name[epName_length] = table_id + '0';
        idxFile_name[idxName_length] = table_id + '0';
        exportCDE(table, m[table_id], N, L[table_id], spFile_name, epFile_name, idxFile_name);

        cover(table, table_id, m[table_id], t[table_id], N, covered, &p_expe);
        p_theo *= pow(1.0 - (double)Mt(N, alpha, t[table_id]) / N, t[table_id]);

        mt_expe += m[table_id];
        mt_theo += mt[table_id];

        free((void *)filters);
        free((void *)table);
    }
    p_expe = 100.0 * p_expe / N;
    p_theo = (1 - p_theo) * 100.0;
    free((void *)covered);

    printf("Time to compute\t\t: %f seconds\n", computeTime / ell);
    printf("Time to clean\t\t: %f seconds\n", cleanTime / ell);
    printf("Time to generate\t: %f seconds\n\n", (computeTime + cleanTime) / ell);

    hashStats(hash_expe, hash_theo);
    epStats(mt_expe, mt_theo);
    coverStats(p_expe, p_theo);
    cdeStats(ell, tables_id, m, N, L, spFile_name, epFile_name, idxFile_name);
}