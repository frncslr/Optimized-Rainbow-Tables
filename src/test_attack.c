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
    int htable_size = table_size;
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in dictionary\n");
    import(htable, htable_size, table_name);

    printf("Fetching endpoints in hashtable\n");
    Points *points;
    for (uint32_t end = 0; end < (uint32_t)htable_size * 4; end++)
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
    int table_size = 1 << 18;
    int table_width = 500;
    char table_name[30] = "tableTestChain";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    int nb_hash = 0;
    int coverage = 0;

    // precompute(table_name, table_id, &table_size, table_width, &nb_hash, &coverage);
    // printf("Unique endpoints in table %d : %d\n",table_id, table_size);

    Hashtable htable;
    // 2^18 500 :   0 : 47840 ; 1 47840: ; 2 : 47840; 3 : 47840
    int htable_size = 47840;
    // htable_size = table_size;
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in hashtable\n");
    import(htable, htable_size, table_name);

    unsigned char hashed[SHA256_DIGEST_LENGTH];
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
    compute(&point, hashed, table_id, 0, col_id, &nb_hash);
    printf("Intermediary point computed \t: %u\n", point);
    printf("Number of hashes done \t\t: %d\n", nb_hash);

    hash(&point, cipher);
    printf("Cipher before chain \t\t: ");
    print_hash(cipher);

    chain(&point, cipher, hashed, table_id, col_id, table_width);
    printf("End point chained \t\t: %u\n", point);
    printf("Cipher after chain \t\t: ");
    print_hash(cipher);
    Points *points;
    if ((points = search(htable, htable_size, point)) != NULL)
    {
        printf("Pair {sp : %u ; ep : %u} found in dictionary\n\n", points->start, points->end);
    }
    else
    {
        printf("End point not found in dictionary\n\n");
    }
    free(htable);
}

void test_rebuild()
{
    printf("# Test rebuild :\n");
    int table_id = 0;
    int table_width = t;
    unsigned char hashed[SHA256_DIGEST_LENGTH];

    srand(time(NULL));
    uint32_t point = (uint32_t)rand() % N;
    printf("Ramdom point\t: %u\n", point);
    uint32_t copy = point;
    printf("Copy of point\t: %u\n", copy);
    int col_id = rand() % table_width;
    printf("Random column\t: %d\n", col_id);
    for (int col = 0; col < col_id; col++)
        hash_reduction(&copy, hashed, table_id, col);
    printf("Copy hash reduced %d times \t: %u\n", col_id, copy);
    rebuild(&point, hashed, table_id, col_id);
    printf("Point rebuilt\t: %u\n\n", point);
}

void test_chain_rebuild()
{
    srand(time(NULL));
    int table_id = rand() % 4;
    int table_size = 1;
    int table_width = 1000;
    Points table[1];
    initialize(table, table_id, table_size);
    int nb_hash = 0;
    unsigned char buffer[SHA256_DIGEST_LENGTH];
    generate(table, table_id, table_size, table_width, &nb_hash, buffer);
    printf("Table : {%u : %u}\n", table->start, table->end);

    int col_id = rand() % table_width;
    printf("COL : %d\n", col_id);

    uint32_t ip = table->start;
    compute(&ip, buffer, table_id, 0, col_id, &nb_hash);
    printf("IP  : %u\n", ip);

    hash(&ip, buffer);
    uint32_t cp;
    chain(&cp, buffer, buffer, table_id, col_id, table_width);
    if (cp == table->end)
        printf("CP  : %u\n", cp);

    uint32_t rp = table->start;
    rebuild(&rp, buffer, table_id, col_id);
    if (rp == ip)
        printf("RP  : %u\n", rp);

    uint32_t cep = table->start;
    hash(&cep, buffer);
    chain(&cep, buffer, buffer, table_id, 0, table_width);
    if (cep == table->end)
        printf("CEP : %u\n", cep);

    uint32_t rep = table->start;
    rebuild(&rep, buffer, table_id, table_width);
    if (rep == table->end)
        printf("REP : %u\n", rep);
    printf("\n");
}

void test_attack_existing()
{
    printf("# Test attack existing:\n");

    int table_id = 1;
    printf("Chosen table id : %d\n", table_id);
    int table_size = 1 << 18;
    int table_width = 500;
    char table_name[30] = "tableTestAttackExisting";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    int nb_hash = 0;
    int coverage = 0;

    // precompute(table_name, table_id, &table_size, table_width, &nb_hash, &coverage);
    // printf("Unique endpoints in table %d : %d\n",table_id, table_size);

    Hashtable htable;
    // 2^18 500 :   0 : 47840 ; 1 47840: ; 2 : 47840; 3 : 47840
    int htable_size = 47840;
    // htable_size = table_size;
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in hashtable\n");
    import(htable, htable_size, table_name);

    unsigned char buffer[SHA256_DIGEST_LENGTH];
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
    compute(&point, buffer, table_id, 0, col_id, &nb_hash);
    printf("Random existing point \t\t: %u\n", point);

    hash(&point, cipher);

    uint32_t result;
    char found = 0;
    attack(cipher, htable, htable_size, table_id, table_width, &result, &found, buffer);
    if (found && (result == point))
    {
        printf("Plain recovered \t\t: %u\n", (uint32_t)result);
    }
    else
    {
        printf("Hash not recovered T_T\n");
    }
    printf("\n");
    free((void *)htable);
}

void test_attack_existing_n()
{
    printf("# Test attack existing n :\n");

    int n = 10000;
    int nb = 0;

    int table_id = 0;
    printf("Table id : %d\n", table_id);
    int table_size = (int)ceil(m0);
    printf("Table size : %d\n", table_size);
    int table_width = t;
    printf("Table width : %d\n", table_width);
    char table_name[30] = "tableTestAttackExistingN";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    int nb_hash = 0;
    int coverage = 0;

    precompute(table_name, table_id, &table_size, table_width, &nb_hash, &coverage);
    printf("Unique endpoints in table %d : %d\n", table_id, table_size);

    Hashtable htable;
    // 2^18 500 :   0 : 47840 ; 1 47840: ; 2 : 47840; 3 : 47840
    // int htable_size = 47840;
    int htable_size = table_size;
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    import(htable, htable_size, table_name);

    static unsigned char cipher[SHA256_DIGEST_LENGTH + 1];
    static unsigned char buffer[SHA256_DIGEST_LENGTH + 1];
    uint32_t plain, sp;
    int col_id;
    char found;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        nb_hash = 0;
        found = 0;
        plain = htable[rand() % htable_size].start;
        if (plain == MAX)
        {
            i--;
            continue;
        }
        sp = plain;
        col_id = rand() % table_width;
        compute(&plain, buffer, table_id, 0, col_id, &nb_hash);
        hash(&plain, cipher);
        uint32_t result;
        attack(cipher, htable, htable_size, table_id, table_width, &result, &found, buffer);
        if (found && (result == plain))
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
    free((void *)htable);
}

void test_attack_random()
{
    printf("# Test attack random:\n");

    int table_id = 0;
    printf("Table id : %d\n", table_id);
    int table_size = (int)ceil(m0);
    int table_width = t;
    char table_name[30] = "tableTestAttackRandom";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    int nb_hash = 0;
    int coverage = 0;

    // precompute(table_name, table_id, &table_size, table_width, &nb_hash, &coverage);
    // printf("Unique endpoints in table %d : %d\n", table_id, table_size);

    Hashtable htable;
    // 0 : 31921 ; 1 : 31882 ; 2 : 31965; 3 : 31927
    int htable_size = table_size;
    htable_size = 31921;
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in hashtable\n");
    import(htable, htable_size, table_name);

    unsigned char buffer[SHA256_DIGEST_LENGTH];
    unsigned char cipher[SHA256_DIGEST_LENGTH];
    srand(time(NULL));
    uint32_t point = rand() % N;
    printf("Random point \t: %u\n", point);
    int col_id = rand() % table_width;
    printf("Random column\t: %d\n", col_id);

    hash(&point, cipher);

    uint32_t result;
    char found = 0;
    attack(cipher, htable, htable_size, table_id, table_width, &result, &found, buffer);
    if (found && (result == point))
    {
        printf("Point recovered : %u\n", (uint32_t)result);
    }
    else
    {
        printf("Hash not recovered T_T\n");
    }
    printf("\n");
    free((void *)htable);
}

void test_attack_random_n()
{
    printf("# Test attack random n :\n");
    int n = 10000;
    int nb = 0;

    int table_id = 1;
    int table_size = (int)ceil(m0);
    int table_width = t;
    char table_name[30] = "tableTestAttackRandomN";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(table_name, extension);
    int nb_hash = 0;
    int coverage = 0;

    printf("Precomputing table %d of initially %d rows\n", table_id, table_size);
    precompute(table_name, table_id, &table_size, table_width, &nb_hash, &coverage);

    int expec_hash = (int)ceil(m0) * t;
    int diff_hash = expec_hash - nb_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2f%%)\n", expec_hash, nb_hash, diff_hash, diff_hash_perc);

    int expec_size = (int)ceil(mt);
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n\texpected\t: %d\n\texperimental\t: %d\n\tdifference\t: %d (%3.2f%%)\n", expec_size, table_size, diff_size, diff_size_perc);

    double expec_coverage_perc = (1 - pow((double)1 - mt / N, (double)t)) * 100;
    double coverage_perc = (double)coverage * 100 / N;
    double diff_coverage_perc = coverage_perc - expec_coverage_perc;
    printf("Coverage of the table :\n\texpected\t: %3.2f%%\n\texperimental\t: %3.2f%%\n\tdifference\t:%3.2f%%\n\n", expec_coverage_perc, coverage_perc, diff_coverage_perc);

    Hashtable htable;
    // 0 : 31921 ; 1 : 31882 ; 2 : 31965; 3 : 31927
    int htable_size = table_size;
    htable_size = 31921;
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, htable_size);
    printf("Importing table in hashtable\n");
    import(htable, htable_size, table_name);

    static unsigned char cipher[SHA256_DIGEST_LENGTH + 1];
    static unsigned char buffer[SHA256_DIGEST_LENGTH + 1];
    uint32_t plain, result;
    char found;
    srand(time(NULL));
    printf("Launching %d random attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        found = 0;
        plain = rand() % N;
        hash(&plain, cipher);
        attack(cipher, htable, htable_size, table_id, table_width, &result, &found, buffer);
        if (found && (result == plain))
            nb++;
    }
    printf("Plains recovered : %u / %u (%3.2f%%)\n\n", nb, n, (100 * (float)nb / n));
    free(htable);
}