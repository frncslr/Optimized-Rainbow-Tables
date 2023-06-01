#include "../include/test_attack.h"

void test_ceri()
{
    printf("# Test ceri :\n");
    int k = 3;
    uint32_t end = 4115;
    uint32_t previous = 4099;
    printf("Current endpoint\t: %u\n", end);
    printf("Previous endpoint\t: %u\n", previous);
    uint32_t difference = end - previous - 1;
    printf("Uncompressed difference\t: %u\n", difference);
    char size = 0;
    for (uint32_t bits = difference; bits; bits >>= 1, size++)
        ;
    printf("Uncompressed bits size\t: %d\n", size);
    rice(&difference, difference, k);
    printf("Compressed difference\t: %u\n", difference);
    size = 0;
    for (uint32_t bits = difference; bits; bits >>= 1, size++)
        ;
    printf("Compressed bits size\t: %d\n", size);
    ceri(&difference, difference, k, size);
    printf("Decompressed difference\t: %u\n\n", difference);
}

void test_import()
{
    printf("# Test import :\n");

    srand(time(NULL));
    int table_id = rand() % 4;
    int table_size = 6;
    char table_name[30] = "tableTestImport";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    printf("Initializing and exporting table %d of %d elements\n", table_id, table_size);
    initialize(table, table_id, table_size);
    export(table, table_size, table_name);

    printf("Table (exported):");
    for (int i = 0; i < table_size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");

    Hashtable htable;
    int htable_size = (int)ceil(LOAD_FACTOR * table_size);
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in dictionary\n");
    import(htable, htable_size, table_size, table_name);

    printf("Fetching endpoints in hashtable\n");
    Points *points;
    for (uint32_t end = 0; end < (uint32_t)table_size * 4; end++)
    {
        if ((points = search(htable, htable_size, end)) != NULL)
        {
            printf("Points {%u : %u} found in hashtable\n", points->start, points->end);
        }
        else
        {
            printf("Endpoint %u not found in hashtable\n", end);
        }
    }
    printf("\n");
    free(table);
    free(htable);
}

void test_import_m()
{
    printf("# Test import m :\n");
    int nb_tables = 4;
    int table_size = 6;
    char table_name[30] = "tableTestImportM";
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);

    int tables_sizes[nb_tables];
    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    printf("Initializing and exporting %d tables\n", nb_tables);
    for (int table_id = 0; table_id < nb_tables; table_id++)
    {

        initialize(table, table_id, table_size);

        table_name[name_length] = table_id + '0';
        export(table, table_size, table_name);

        tables_sizes[table_id] = table_size;

        printf("Table %d (exported) :\n", table_id);
        for (int i = 0; i < table_size; i++)
            printf("%u\t:\t%u\n", table[i].start, table[i].end);

        printf("\n");
    }

    int htables_sizes[nb_tables];
    Hashtable *htables;
    if ((htables = (Hashtable *)calloc(nb_tables, sizeof(Hashtable))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    for (int table_id = 0; table_id < nb_tables; table_id++)
    {
        htables_sizes[table_id] = (int)ceil(LOAD_FACTOR * tables_sizes[table_id]);
        if ((htables[table_id] = (Points *)calloc(htables_sizes[table_id], sizeof(Points))) == NULL)
        {
            fprintf(stderr, "Memory allocation problem\n");
            exit(ERROR_ALLOC);
        }
        init(htables[table_id], htables_sizes[table_id]);
        printf("Importing table %d in hashtable\n", table_id);
        table_name[name_length] = table_id + '0';
        import(htables[table_id], htables_sizes[table_id], table_size, table_name);
    }

    printf("Fetching endpoints in hashtables\n");
    Points *points;
    for (int table_id = 0; table_id < nb_tables; table_id++)
    {
        for (uint32_t end = (uint32_t)table_size * table_id; end < (uint32_t)table_size * (table_id + 1); end++)
        {
            if ((points = search(htables[table_id], htables_sizes[table_id], end)) != NULL)
            {
                printf("Points {%u : %u} found in hashtable %d\n", points->start, points->end, table_id);
            }
            else
            {
                printf("Endpoint %u not found in hashtable\n", end);
            }
        }
    }
    printf("\n");

    free((void *)table);
    for (int table_id = 0; table_id < nb_tables; table_id++)
        free((void *)htables[table_id]);
    free((void *)htables);
}

void test_chain()
{
    printf("# Test chain :\n");

    int table_id = 3;
    int table_size = 1 << 10;
    int table_width = 500;
    printf("Table :\n id : %d\n size : %d\n width : %d\n", table_id, table_size, table_width);
    char table_name[30] = "tableTestChain";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    uint32_t nb_hash = 0;
    int nb_filters = 1;
    int filters = table_width;
    precompute(&table, table_id, &table_size, &filters, nb_filters, &nb_hash);
    export(table, table_size, table_name);

    Hashtable htable;
    int htable_size = (int)ceil(LOAD_FACTOR * table_size);
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in hashtable\n");
    import(htable, htable_size, table_size, table_name);

    unsigned char cipher[SHA256_DIGEST_LENGTH];
    uint32_t point;
    srand(time(NULL));
    do
    {
        point = htable[rand() % htable_size].start;
    } while (point == MAX);
    printf("Random existing start point \t: %u\n", point);
    int col_id = rand() % table_width;
    printf("Random existing column \t\t: %d\n", col_id);

    nb_hash = 0;
    compute(&point, table_id, 0, col_id, &nb_hash);
    printf("Intermediary point computed \t: %u\n", point);
    printf("Number of hashes done \t\t: %u\n", nb_hash);

    hash(&point, cipher);
    printf("Cipher before chain \t\t: ");
    print_hash(cipher);

    nb_hash = 0;
    chain(&point, cipher, table_id, table_width, col_id, &nb_hash);
    printf("End point chained \t\t: %u\n", point);
    printf("Number of hashes done \t\t: %u\n", nb_hash);
    printf("Cipher after chain \t\t: ");
    print_hash(cipher);
    Points *points;
    if ((points = search(htable, htable_size, point)) != NULL)
    {
        printf("Pair {sp : %u ; ep : %u} found in hashtable\n\n", points->start, points->end);
    }
    else
    {
        printf("End point not found in hashtable\n\n");
    }
    free((void *)htable);
    free((void *)table);
}

void test_rebuild()
{
    printf("# Test rebuild :\n");
    int table_id = 0;
    int table_width = t;

    srand(time(NULL));
    uint32_t point = (uint32_t)rand() % N;
    printf("Ramdom point\t: %u\n", point);
    uint32_t copy = point;
    printf("Copy of point\t: %u\n", copy);
    int col_id = rand() % table_width;
    printf("Random column\t: %d\n", col_id);
    for (int col = 0; col < col_id; col++)
        hash_reduction(&copy, table_id, col);
    printf("Copy hashred \t: %u\n", copy);
    uint32_t nb_hash = 0;
    rebuild(&point, table_id, col_id, &nb_hash);
    printf("Point rebuilt\t: %u\n", point);
    printf("Number of hashes done\t: %u\n\n", nb_hash);
}

void test_chain_rebuild()
{
    srand(time(NULL));
    int table_id = rand() % 4;
    int table_size = 1;
    int table_width = 1000;
    Points table[1];

    initialize(table, table_id, table_size);

    uint32_t nb_hash = 0;
    int nb_filters = 1;
    int filters = table_width;
    generate(table, table_id, &table_size, &filters, nb_filters, &nb_hash);
    printf("Table : {%u : %u}\n", table->start, table->end);
    printf("id : %d\n size : %d\n width : %d\n", table_id, table_size, table_width);

    int col_id = rand() % table_width;
    printf("Random column\t\t: %d\n", col_id);

    uint32_t ip = table->start;
    compute(&ip, table_id, 0, col_id, &nb_hash);
    printf("Intermediary point\t: %u\n", ip);

    hash(&ip, buffer);
    uint32_t cp;
    nb_hash = 0;
    chain(&cp, buffer, table_id, table_width, col_id, &nb_hash);
    if (cp == table->end)
        printf("Chained point\t\t: %u\n", cp);
    printf("Number of hashes done\t: %u\n", nb_hash);

    uint32_t rp = table->start;
    nb_hash = 0;
    rebuild(&rp, table_id, col_id, &nb_hash);
    if (rp == ip)
        printf("Reduced point\t\t: %u\n", rp);
    printf("Number of hashes done\t: %u\n", nb_hash);

    uint32_t cep = table->start;
    hash(&cep, buffer);
    nb_hash = 0;
    chain(&cep, buffer, table_id, table_width, 0, &nb_hash);
    if (cep == table->end)
        printf("Chained endpoint\t: %u\n", cep);
    printf("Number of hashes done\t: %u\n", nb_hash);

    uint32_t rep = table->start;
    nb_hash = 0;
    rebuild(&rep, table_id, table_width, &nb_hash);
    if (rep == table->end)
        printf("Reduced endpoint\t: %u\n", rep);
    printf("Number of hashes done\t: %u\n\n", nb_hash);
}

void test_attack_existing()
{
    printf("# Test attack existing:\n");

    int nb_tables = 1;
    int table_id = 0;
    int table_size = 1 << 12;
    int table_width = t;
    printf("Table :\n index : %d\n size  : %d\n width : %d\n", table_id, table_size, table_width);
    char table_name[30] = "tableTestAttackExisting";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    uint32_t nb_hash = 0;
    int nb_filters = 1;
    int filters = table_width;
    precompute(&table, table_id, &table_size, &filters, nb_filters, &nb_hash);
    export(table, table_size, table_name);

    Hashtable htable;
    int htable_size = (int)ceil(LOAD_FACTOR * table_size);
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in hashtable\n");
    import(htable, htable_size, table_size, table_name);

    unsigned char cipher[SHA256_DIGEST_LENGTH];
    Points *points;
    uint32_t point;
    srand(time(NULL));
    do
    {
        points = htable + rand() % htable_size;
    } while (points->end == MAX);
    point = points->start;
    printf("Random existing start point \t: %u\n", point);
    int col_id = rand() % table_width;
    printf("Random existing column \t\t: %d\n", col_id);

    nb_hash = 0;
    compute(&point, table_id, 0, col_id, &nb_hash);
    printf("Random existing point \t\t: %u\n", point);
    printf("Number of hashes done \t\t: %u\n", nb_hash);

    hash(&point, cipher);

    uint32_t result = MAX;
    nb_hash = 0;
    attack(cipher, &htable, &htable_size, nb_tables, table_width, &result, &nb_hash);
    if (result == point)
    {
        printf("Plain recovered \t\t: %u\n", (uint32_t)result);
    }
    else
    {
        printf("Hash not recovered T_T\n");
    }
    printf("Number of hashes done \t\t: %u\n", nb_hash);
    printf("\n");
    free((void *)table);
    free((void *)htable);
}

void test_attack_existing_n()
{
    printf("# Test attack existing n :\n");

    int n = 10000;
    int nb = 0;

    int nb_tables = 1;
    int table_id = 0; // DONT CHANGE IT
    printf("Table id : %d\n", table_id);
    int table_size = (int)ceil(m0);
    printf("Table size : %d\n", table_size);
    int table_width = t;
    printf("Table width : %d\n", table_width);
    char table_name[30] = "tableTestAttackExistingN";
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

    uint32_t nb_hash = 0;
    precompute(&table, table_id, &table_size, filters, nb_filters, &nb_hash);
    export(table, table_size, table_name);

    int tables_sizes[] = {31921, 31882, 31965, 31927};
    table_size = tables_sizes[table_id];
    Hashtable htable;
    int htable_size = (int)ceil(LOAD_FACTOR * table_size);
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    import(htable, htable_size, table_size, table_name);

    static unsigned char cipher[SHA256_DIGEST_LENGTH];
    Points *points;
    uint32_t plain, result, sp;
    int col_id;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        result = MAX;
        points = htable + (rand() % htable_size);
        if (points->end == MAX)
        {
            i--;
            continue;
        }
        plain = points->start;
        sp = plain;
        col_id = rand() % table_width;
        nb_hash = 0;
        compute(&plain, table_id, 0, col_id, &nb_hash);
        hash(&plain, cipher);
        nb_hash = 0;
        attack(cipher, &htable, &htable_size, nb_tables, table_width, &result, &nb_hash);
        if (result == plain)
        {
            nb++;
        }
        else
        {
            printf("Not recovered : sp : %u & col : %d T_T\n", sp, col_id);
        }
    }
    printf("Plains recovered : %u / %u (%0.2f%%)\n", nb, n, (100 * (float)nb / n));
    printf("\n");
    free((void *)table);
    free((void *)filters);
    free((void *)htable);
}

void test_attack_random()
{
    printf("# Test attack random:\n");

    int nb_tables = 1;
    int table_id = 0;
    printf("Table id : %d\n", table_id);
    int table_size = (int)ceil(m0);
    int table_width = t;
    char table_name[30] = "tableTestAttackRandom";
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

    uint32_t nb_hash = 0;
    precompute(&table, table_id, &table_size, filters, nb_filters, &nb_hash);
    export(table, table_size, table_name);

    int tables_sizes[] = {31921, 31882, 31965, 31927};
    table_size = tables_sizes[table_id];
    Hashtable htable;
    int htable_size = (int)ceil(LOAD_FACTOR * table_size);
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in hashtable\n");
    import(htable, htable_size, table_size, table_name);

    unsigned char cipher[SHA256_DIGEST_LENGTH];
    srand(time(NULL));
    uint32_t point = rand() % N;
    printf("Random point \t: %u\n", point);

    hash(&point, cipher);

    uint32_t result = MAX;
    nb_hash = 0;
    attack(cipher, &htable, &htable_size, nb_tables, table_width, &result, &nb_hash);
    if (result == point)
    {
        printf("Point recovered : %u\n", (uint32_t)result);
    }
    else
    {
        printf("Hash not recovered T_T\n");
    }
    printf("Number of hashes done \t\t: %u\n", nb_hash);
    printf("\n");
    free((void *)table);
    free((void *)htable);
    free((void *)filters);
}

void test_attack_random_n()
{
    printf("# Test attack random n :\n");
    int n = 10000;

    int nb_tables = 1;
    int table_id = 0;
    int table_size = (int)ceil(m0);
    int table_width = t;
    char table_name[30] = "tableTestAttackRandomN";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    int coverage = 0;
    char *covered;
    if ((covered = (char *)calloc(N, sizeof(char))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    printf("Precomputing table %d of initially %d rows\n", table_id, table_size);

    int nb_filters, *filters = NULL;
    char file_name[30] = "configTestPositions.dat";
    positions(&filters, &nb_filters, file_name);

    uint32_t nb_hash = 0;
    precompute(&table, table_id, &table_size, filters, nb_filters, &nb_hash);
    export(table, table_size, table_name);
    cover(table, table_id, table_size, table_width, covered, &coverage);

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

    int tables_sizes[] = {31921, 31882, 31965, 31927};
    table_size = tables_sizes[table_id];
    Hashtable htable;
    int htable_size = (int)ceil(LOAD_FACTOR * table_size);
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in hashtable\n");
    import(htable, htable_size, table_size, table_name);

    static unsigned char cipher[SHA256_DIGEST_LENGTH + 1];
    uint32_t plain, result = MAX;
    uint32_t total_hash = 0;
    srand(time(NULL));
    printf("Launching %d random attacks\n", n);
    int nb = 0;
    for (int i = 0; i < n; i++)
    {
        nb_hash = 0;
        plain = rand() % N;
        hash(&plain, cipher);
        attack(cipher, &htable, &htable_size, nb_tables, table_width, &result, &nb_hash);
        if (result == plain)
        {
            nb++;
        }
        total_hash += nb_hash;
    }
    printf("Plains recovered\n: %u / %u (%3.2lf%%)\n", nb, n, (100 * (float)nb / n));
    double avg_hash = (double)total_hash / n;
    printf("Average operations\t: %f\n\n", avg_hash);

    free((void *)table);
    free((void *)htable);
    free((void *)filters);
    free((void *)covered);
}

void test_attack_random_n_m()
{
    printf("# Test attack random n m:\n");
    int n = 10000;
    int nb = 0;

    int nb_tables = 4;
    int init_size = (int)ceil(m0);
    int table_width = t;
    char table_name[30] = "tableTestPrecompFullNM";
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
    char file_name[30] = "configTestPositions.dat";
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
    expec_hash *= nb_tables;
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

    int tables_sizes[] = {31921, 31882, 31965, 31927};
    int htables_sizes[nb_tables];
    Hashtable *htables;
    if ((htables = (Hashtable *)calloc(nb_tables, sizeof(Hashtable))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    for (int table_id = 0; table_id < nb_tables; table_id++)
    {
        htables_sizes[table_id] = (int)ceil(LOAD_FACTOR * tables_sizes[table_id]);
        if ((htables[table_id] = (Points *)calloc(htables_sizes[table_id], sizeof(Points))) == NULL)
        {
            fprintf(stderr, "Memory allocation problem\n");
            exit(ERROR_ALLOC);
        }
        init(htables[table_id], htables_sizes[table_id]);
        printf("Importing table %d in hashtable\n", table_id);
        table_name[name_length] = table_id + '0';
        import(htables[table_id], htables_sizes[table_id], tables_sizes[table_id], table_name);
    }

    static unsigned char cipher[SHA256_DIGEST_LENGTH + 1];
    uint32_t plain, result = MAX;
    uint32_t total_hash = 0;
    srand(time(NULL));
    printf("Launching %d random attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        nb_hash = 0;
        plain = rand() % N;
        hash(&plain, cipher);
        attack(cipher, htables, htables_sizes, nb_tables, table_width, &result, &nb_hash);
        if (result == plain)
        {
            nb++;
        }
        total_hash += nb_hash;
    }
    printf("Plains recovered\t: %u / %u (%3.2lf%%)\n", nb, n, (100 * (float)nb / n));
    double avg_hash = (double)total_hash / n;
    printf("Average operations\t: %f\n\n", avg_hash);

    write_results(&avg_hash, 1, "avgOpe.dat");

    for (int table_id = 0; table_id < nb_tables; table_id++)
        free((void *)htables[table_id]);
    free((void *)htables);
    free((void *)filters);
    free((void *)covered);
}