#include "../include/test_attack.h"

void test_ceri()
{
    printf("# Test ceri :\n");
    int k = 3;
    uint32_t end = 4115;
    uint32_t previous = 4099;
    printf("Current endpoint\t: %u\n", end);
    printf("Previous endpoint\t: %u\n", previous);
    printf("Uncompressed difference\t: %u\n", (end - previous - 1));
    rice(&end, (end - previous - 1), k);
    printf("Compressed difference\t: %u\n", end);
    char size = 0;
    for (uint32_t bits = end; bits; bits >>= 1, size++)
        ;
    printf("Compressed bits size\t: %d\n", size);
    ceri(&end, end, k, size);
    printf("Decompressed difference\t: %u\n\n", end);
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
    precompute(table, table_id, &table_size, table_width, &nb_hash);
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
        printf("End point not found in dictionary\n\n");
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
    unsigned char buffer[SHA256_DIGEST_LENGTH];
    generate(table, table_id, table_size, table_width, &nb_hash);
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

    int table_id = 1;
    int table_size = 1 << 12;
    int table_width = t;
    printf("Table :\n index : %d\n size  : %d\n width : %d\n", table_id, table_size, table_width);
    char table_name[30] = "tableTestAttackExisting";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    uint32_t nb_hash = 0;

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    precompute(table, table_id, &table_size, table_width, &nb_hash);
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
    attack(cipher, htable, htable_size, table_id, table_width, &result, &nb_hash);
    if (result == point)
    {
        printf("Plain recovered \t\t: %u\n", (uint32_t)result);
        printf("Number of hashes done \t\t: %u\n", nb_hash);
    }
    else
    {
        printf("Hash not recovered T_T\n");
        printf("Number of hashes done \t\t: %u\n", nb_hash);
    }
    printf("\n");
    free((void *)table);
    free((void *)htable);
}

void test_attack_existing_n()
{
    printf("# Test attack existing n :\n");

    int n = 10000;
    int nb = 0;

    int table_id = 1;
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

    uint32_t nb_hash = 0;
    precompute(table, table_id, &table_size, table_width, &nb_hash);
    export(table, table_size, table_name);

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
        attack(cipher, htable, htable_size, table_id, table_width, &result, &nb_hash);
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
    free((void *)htable);
}

void test_attack_random()
{
    printf("# Test attack random:\n");

    int table_id = 3;
    printf("Table id : %d\n", table_id);
    int table_size = (int)ceil(m0);
    int table_width = t;
    char table_name[30] = "tableTestAttackRandom";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    uint32_t nb_hash = 0;

    Points *table;
    if ((table = (Points *)calloc(table_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    precompute(table, table_id, &table_size, table_width, &nb_hash);
    // printf("Unique endpoints in table %d : %d\n", table_id, table_size);

    Hashtable htable;
    // 0 : 31921 ; 1 : 31882 ; 2 : 31965; 3 : 31927
    int htable_size = table_size;
    htable_size = 31927;
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
    attack(cipher, htable, htable_size, table_id, table_width, &result, &nb_hash);
    if (result == point)
    {
        printf("Point recovered : %u\n", (uint32_t)result);
        printf("Number of hashes done \t\t: %u\n", nb_hash);
    }
    else
    {
        printf("Hash not recovered T_T\n");
    }
    printf("\n");
    free((void *)table);
    free((void *)htable);
}

void test_attack_random_n()
{
    printf("# Test attack random n :\n");
    int n = 1000;
    int nb = 0;

    int table_id = 3;
    int table_size = (int)ceil(m0);
    int table_width = t;
    char table_name[30] = "tableTestAttackRandomN";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    uint32_t nb_hash = 0;
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

    precompute(table, table_id, &table_size, table_width, &nb_hash);
    export(table, table_size, table_name);
    cover(table, table_id, table_size, table_width, covered, &coverage);

    uint32_t expec_hash = (int)ceil(m0) * t;
    uint32_t diff_hash = expec_hash - nb_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %u\n\texperimental\t: %u\n\tdifference\t: %u (%3.2lf%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = (int)ceil(mt);
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2lf%%)\n", expec_size, table_size, diff_size, diff_size_perc);

    double expec_coverage_perc = (1 - pow((double)1 - mt / N, (double)t)) * 100;
    double coverage_perc = (double)coverage * 100 / N;
    double diff_coverage_perc = coverage_perc - expec_coverage_perc;
    printf("Coverage of the table :\n\texpected\t: %3.2lf%%\n\texperimental\t: %3.2lf%%\n\tdifference\t: %3.2lf%%\n\n", expec_coverage_perc, coverage_perc, diff_coverage_perc);

    Hashtable htable;
    int htable_size = (int)(1.5 * table_size);
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in hashtable\n");
    import(htable, htable_size, table_size, table_name);

    static unsigned char cipher[SHA256_DIGEST_LENGTH + 1];
    static unsigned char buffer[SHA256_DIGEST_LENGTH + 1];
    uint32_t plain, result = MAX;
    uint32_t total_hash = 0;
    srand(time(NULL));
    printf("Launching %d random attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        nb_hash = 0;
        plain = rand() % N;
        hash(&plain, cipher);
        attack(cipher, htable, htable_size, table_id, table_width, &result, &nb_hash);
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
    free((void *)covered);
}