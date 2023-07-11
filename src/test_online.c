#include "../include/test_online.h"

void test_import()
{
    printf("# Test import :\n");
    HTable htable;
    int hsize, mt;
    int table_id = 2;
    char table_name[40] = "data/tables/std/tableTestExport";
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);
    table_name[name_length] = table_id + '0';
    printf("Importing table in dictionary\n");
    import(&htable, &hsize, &mt, table_name);

    printf("Fetching endpoints in hashtable\n");
    Chain *chain;
    for (uint64_t end = 0, m = 4; end < m * 4; end++)
    {
        if ((chain = search(htable, hsize, end)) != NULL)
        {
            printf("Chain {%lu : %lu} found in hashtable\n", chain->sp, chain->ep);
        }
        else
        {
            printf("Endpoint %lu not found in hashtable\n", end);
        }
    }
    printf("\n");
    free(htable);
}
void test_import_ell()
{
    printf("# Test import ell :\n");
    int ell = 4;
    char table_name[40] = "data/tables/std/tableTestExport";
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);

    HTable *htables;
    int hsizes[ell], mt[ell];
    if ((htables = (HTable *)calloc(ell, sizeof(HTable))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    for (int table_id = 0; table_id < ell; table_id++)
    {
        table_name[name_length] = table_id + '0';
        import(&htables[table_id], &hsizes[table_id], &mt[table_id], table_name);
    }

    printf("Fetching endpoints in hashtables\n");
    Chain *chain;
    for (uint64_t end = 0, m = 4; end < m * ell; end++)
    {
        for (int table_id = 0; table_id < ell; table_id++)
        {
            if ((chain = search(htables[table_id], hsizes[table_id], end)) != NULL)
            {
                printf("Points {%lu : %lu} found in hashtable %d\n", chain->sp, chain->ep, table_id);
                continue;
            }
        }
    }
    printf("\n");

    for (int table_id = 0; table_id < ell; table_id++)
        free((void *)htables[table_id]);
    free((void *)htables);
}
void test_chain()
{
    printf("# Test chain :\n");
    uint64_t N = 1 << 24;
    int t = 1000;
    int table_id = 0;
    unsigned char cipher[SHA256_DIGEST_LENGTH];
    srand(time(NULL));
    Point point = rand() % N;
    printf("Random existing point \t: %lu\n", point);
    Point copy = point;
    int col = rand() % t;
    printf("Random existing column \t\t: %d\n", col);

    uint64_t nb_hash = 0;
    compute(&point, table_id, 0, col, t, N, &nb_hash);
    printf("Intermediary point computed \t: %lu\n", point);
    printf("Number of hashes done \t\t: %lu\n", nb_hash);

    hash(&point, cipher);
    printf("Cipher of point \t\t: ");
    print_hash(cipher);

    chain(&point, cipher, table_id, col, t, N, &nb_hash);
    printf("Point chained \t\t: %lu\n", point);
    printf("Number of hashes done \t\t: %lu\n", nb_hash);
    printf("Cipher after chain \t\t: ");
    print_hash(cipher);

    nb_hash = 0;
    compute(&copy, 0, 0, t, t, N, &nb_hash);
    printf("Copy computed \t\t: %lu\n", point);
    printf("Number of hashes done \t\t: %lu\n", nb_hash);
}
void test_rebuild()
{
    printf("# Test rebuild :\n");
    uint64_t N = 1 << 24;
    int t = 1000;
    int table_id = 0;

    srand(time(NULL));
    Point point = rand() % N;
    printf("Ramdom point\t: %lu\n", point);
    Point copy = point;
    printf("Copy of point\t: %lu\n", copy);
    int col_id = rand() % t;
    printf("Random column\t: %d\n", col_id);
    for (int col = 0; col < col_id; col++)
        hash_reduce(&copy, table_id, col, t, N);
    printf("Copy hashed \t: %lu\n", copy);
    uint64_t nb_hash = 0;
    rebuild(&point, table_id, col_id, t, N, &nb_hash);
    printf("Point rebuilt\t: %lu\n", point);
    printf("Hashes done\t: %lu\n\n", nb_hash);
}
void test_attackSTD_existing()
{
    printf("# Test attackSTD existing :\n");
    uint64_t N = 1 << 24;
    int t = 1000;
    int ell = 1;

    int n = 1000;
    int nb = 0;

    HTable htable;
    int hsize, mt;
    int table_id = 0; // MUST BE 0 IF ONLY ONE TABLE
    char table_name[40] = "data/tables/std/tableSTD";
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);
    table_name[name_length] = table_id + '0';
    printf("Importing table in dictionary\n");
    import(&htable, &hsize, &mt, table_name);

    static unsigned char cipher[SHA256_DIGEST_LENGTH];
    Chain *chain;
    Point plain, result, sp;
    int col_id;
    uint64_t nb_hash;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        result = MAX;
        chain = htable + (rand() % hsize);
        if (chain->ep == MAX)
        {
            i--;
            continue;
        }
        plain = chain->sp;
        sp = plain;
        col_id = rand() % t;
        nb_hash = 0;
        compute(&plain, table_id, 0, col_id, t, N, &nb_hash);
        hash(&plain, cipher);
        nb_hash = 0;
        attackSTD(cipher, &htable, &hsize, ell, t, N, &result, &nb_hash);
        if (result == plain)
        {
            nb++;
        }
        else
        {
            printf("Not recovered : sp : %lu & col : %d T_T\n", sp, col_id);
        }
    }
    printf("Plains recovered : %u / %u (%0.2f%%)\n", nb, n, (100 * (float)nb / n));
    printf("\n");
    free((void *)htable);
}
void test_attackSTD_random()
{
    printf("# Test attackSTD random :\n");
    uint64_t N = 1 << 24;
    int t = 1000;
    int ell = 1;

    int n = 1000;
    int nb = 0;

    HTable htable;
    int hsize, mt;
    int table_id = 0; // MUST BE 0 IF ONLY ONE TABLE
    char table_name[40] = "data/tables/std/tableSTD";
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);
    table_name[name_length] = table_id + '0';
    printf("Importing table in dictionary\n");
    import(&htable, &hsize, &mt, table_name);

    static unsigned char cipher[SHA256_DIGEST_LENGTH];
    Point plain, result;
    uint64_t nb_hash, total_hash = 0;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        nb_hash = 0;
        plain = rand() % N;
        hash(&plain, cipher);
        attackSTD(cipher, &htable, &hsize, ell, t, N, &result, &nb_hash);
        if (result == plain)
        {
            nb++;
        }
        total_hash += nb_hash;
    }
    printf("Plains recovered\t: %u / %u (%0.2f%%)\n", nb, n, (100 * (float)nb / n));
    double avg_hash = (double)total_hash / n;
    printf("Average operations\t: %f\n\n", avg_hash);
    free((void *)htable);
}
void test_attackSTD_random_ell()
{
    printf("# Test attackSTD random ell :\n");
    int n = 10000;
    int nb = 0;

    uint64_t N = 1 << 24;
    int t = 1000;
    int ell = 4;
    char table_name[40] = "data/tables/std/tableSTD";
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);

    HTable *htables;
    int hsizes[ell], mt[ell];
    if ((htables = (HTable *)calloc(ell, sizeof(HTable))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    for (int table_id = 0; table_id < ell; table_id++)
    {
        table_name[name_length] = table_id + '0';
        import(&htables[table_id], &hsizes[table_id], &mt[table_id], table_name);
    }

    static unsigned char cipher[SHA256_DIGEST_LENGTH];
    Point plain, result;
    uint64_t nb_hash, total_hash = 0;
    timeval start, end;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    gettimeofday(&start, 0);
    for (int i = 0; i < n; i++)
    {
        nb_hash = 0;
        plain = rand() % N;
        hash(&plain, cipher);
        attackSTD(cipher, htables, hsizes, ell, t, N, &result, &nb_hash);
        if (result == plain)
        {
            nb++;
        }
        total_hash += nb_hash;
    }
    gettimeofday(&end, 0);
    printf("Plains recovered\t: %u / %u (%0.2f%%)\n", nb, n, (100 * (float)nb / n));
    double avg_hash = (double)total_hash / n;
    printf("Average operations\t: %f\n\n", avg_hash);
    printf("Average time to attack\t: %f\n", elapsed(&start, &end) / n);

    // write_results(&avg_hash, 1, "avgOpe.dat");

    for (int table_id = 0; table_id < ell; table_id++)
        free((void *)htables[table_id]);
    free((void *)htables);
}
void test_attackCDE_existing()
{
    printf("# Test attackCDE existing :\n");

    int n = 1000;
    int nb = 0;

    int ell = 1;
    int table_id = 0; // MUST BE 0 IF ONLY ONE TABLE
    int m;
    int t = 1000;
    int N = 1 << 24;
    char spFile_name[40] = "data/tables/cde/spCDE";
    char epFile_name[40] = "data/tables/cde/epCDE";
    char idxFile_name[40] = "data/tables/cde/idxCDE";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(spFile_name, extension);
    strcat(epFile_name, extension);
    strcat(idxFile_name, extension);

    Point *spTable = NULL;
    importSP(spFile_name, &spTable, &m);
    int L = Lblocks(m);
    Index *idxTable = NULL;
    importIdx(idxFile_name, L, m, N, &idxTable);
    BitStream epStream;
    initBitStream(&epStream, epFile_name, 0);

    static unsigned char cipher[SHA256_DIGEST_LENGTH];
    Point plain, result, sp;
    uint64_t nb_hash;
    double avgDec, total_avgDec = 0.0;
    int col_id;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        sp = spTable[rand() % m];
        plain = sp;
        col_id = rand() % t;
        nb_hash = 0;
        compute(&plain, table_id, 0, col_id, t, N, &nb_hash);
        hash(&plain, cipher);
        nb_hash = 0;
        avgDec = 0.0;
        attackCDE(cipher, &spTable, &epStream, &idxTable, &m, ell, t, N, &result, &nb_hash, &avgDec);
        total_avgDec += avgDec;
        if (result == plain)
        {
            nb++;
        }
        else
        {
            printf("Not recovered %d : sp : %lu & col : %d T_T\n", i, sp, col_id);
        }
    }
    printf("Plains recovered : %u / %u (%0.2f%%)\n", nb, n, (100 * (float)nb / n));
    printf("Avg decodings : %f\n", total_avgDec / n);
    printf("\n");
    free((void *)spTable);
    free((void *)idxTable);
    closeBitStream(&epStream);
}
void test_attackCDE_random()
{
    printf("# Test attackCDE random :\n");

    int n = 1000;
    int nb = 0;

    int ell = 1;
    int table_id = 0; // MUST BE 0 IF ONLY ONE TABLE
    int m;
    int t = 1000;
    int N = 1 << 24;
    char spFile_name[40] = "data/tables/cde/spCDE";
    char epFile_name[40] = "data/tables/cde/epCDE";
    char idxFile_name[40] = "data/tables/cde/idxCDE";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(spFile_name, extension);
    strcat(epFile_name, extension);
    strcat(idxFile_name, extension);

    Point *spTable = NULL;
    importSP(spFile_name, &spTable, &m);
    int L = Lblocks(m);
    Index *idxTable = NULL;
    importIdx(idxFile_name, L, m, N, &idxTable);
    BitStream epStream;
    initBitStream(&epStream, epFile_name, 0);

    static unsigned char cipher[SHA256_DIGEST_LENGTH];
    Point plain, result, sp;
    uint64_t nb_hash, total_hash;
    double avgDec, total_avgDec = 0.0;
    int col_id;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        nb_hash = 0;
        avgDec = 0.0;
        plain = rand() % N;
        hash(&plain, cipher);
        attackCDE(cipher, &spTable, &epStream, &idxTable, &m, ell, t, N, &result, &nb_hash, &avgDec);
        if (result == plain)
        {
            nb++;
        }
        total_avgDec += avgDec;
        total_hash += nb_hash;
    }
    printf("Plains recovered\t: %u / %u (%3.2lf%%)\n", nb, n, (100 * (float)nb / n));
    double avg_hash = (double)total_hash / n;
    printf("Average operations\t: %f\n", avg_hash);
    printf("Average decodings\t: %f\n", total_avgDec / n);
    printf("\n");
    free((void *)spTable);
    free((void *)idxTable);
    closeBitStream(&epStream);
}
void test_attackCDE_random_ell()
{
    printf("# Test attackCDE random ell:\n");

    int n = 10000;
    int nb = 0;

    int ell = 4;
    int m[ell];
    int t = 1000;
    int N = 1 << 24;
    int L;
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

    Point **spTable = NULL;
    if ((spTable = (Point **)calloc(ell, sizeof(Point *))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    BitStream *epStream = NULL;
    if ((epStream = (BitStream *)calloc(ell, sizeof(BitStream))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    Index **idxTable = NULL;
    if ((idxTable = (Index **)calloc(ell, sizeof(Index *))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    for (int table_id = 0; table_id < ell; table_id++)
    {
        spFile_name[spName_length] = table_id + '0';
        epFile_name[epName_length] = table_id + '0';
        idxFile_name[idxName_length] = table_id + '0';
        importSP(spFile_name, &spTable[table_id], &m[table_id]);
        L = Lblocks(m[table_id]);
        importIdx(idxFile_name, L, m[table_id], N, &idxTable[table_id]);
        initBitStream(&epStream[table_id], epFile_name, 0);
    }

    static unsigned char cipher[SHA256_DIGEST_LENGTH];
    Point plain, result;
    uint64_t nb_hash, total_hash = 0;
    double avgDec, total_avgDec = 0.0;
    timeval start, end;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    gettimeofday(&start, 0);

    for (int i = 0; i < n; i++)
    {
        nb_hash = 0;
        avgDec = 0;
        plain = rand() % N;
        hash(&plain, cipher);
        attackCDE(cipher, spTable, epStream, idxTable, m, ell, t, N, &result, &nb_hash, &avgDec);
        if (result == plain)
        {
            nb++;
        }
        total_hash += nb_hash;
        total_avgDec += avgDec;
    }
    gettimeofday(&end, 0);
    printf("Plains recovered\t: %u / %u (%3.2lf%%)\n", nb, n, (100 * (float)nb / n));
    double avg_hash = (double)total_hash / n;
    printf("Average operations\t: %f\n", avg_hash);
    printf("Average decodings \t: %f\n", total_avgDec / n);
    printf("Average time to attack\t: %f\n", elapsed(&start, &end) / n);
    printf("\n");
    for (int table_id = 0; table_id < ell; table_id++)
    {
        free((void *)spTable[table_id]);
        closeBitStream(&epStream[table_id]);
        free((void *)idxTable[table_id]);
    }
    free((void *)spTable);
    free((void *)epStream);
    free((void *)idxTable);
}

void test_Nu()
{
    printf("# Test nu :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n");
    printf("\tN\t: %lu\n", N);
    printf("\tr\t: %3.3f\n", r);
    printf("\talpha\t: %3.3f\n\n", alpha);

    int ell = 4;
    int t[] = {573, 1063, 1561, 2048};
    int mt[ell];
    for (int i = 0; i < ell; i++)
        mt[i] = Mt(N, alpha, t[i]);

    srand(time(NULL));
    int col = rand() % t[ell - 1];
    col = 2047;
    printf("Random column : %d\n", col);
    printf("Metrics :\n");
    for (int i = 0; i < ell; i++)
        printf("\tnu %d : %Lf\n", i, Nu(N, mt[i], t[i], col));
    printf("\n");
}
void test_sortMetric()
{
    printf("# Test sort metric :\n");
    uint64_t N = 1 << 24;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("General parameters :\n");
    printf("\tN\t: %lu\n", N);
    printf("\tr\t: %3.3f\n", r);
    printf("\talpha\t: %3.3f\n\n", alpha);

    int ell = 4;
    int t[] = {573, 1063, 1561, 2048};
    int mt[ell];
    for (int i = 0; i < ell; i++)
        mt[i] = Mt(N, alpha, t[i]);
    Metric metric[ell];
    int table_id, col[ell];
    printf("Before sort :\n");
    for (int i = ell - 1; i >= 0; i--)
    {
        col[i] = t[i] - 1;
        metric[i].nu = Nu(N, mt[i], t[i], col[i]);
        metric[i].table_id = i;
        printf("%d : %Lf\n", metric[i].table_id, metric[i].nu);
    }
    sortMetric(metric, ell);
    printf("After sort :\n");
    for (int i = 0; i < ell; i++)
    {
        printf("%d : %Lf\n", metric[i].table_id, metric[i].nu);
    }
}
void test_attackSTDxHTG()
{
    printf("# Test attack std htg :\n");

    int n = 1;
    int nb = 0;

    uint64_t N = 1 << 24;
    double r = 20.0;
    int ell = 4;
    int t[] = {573, 1063, 1561, 2048};
    int mt[ell];

    char table_name[40] = "data/tables/std/tableSTD";
    int name_length = strlen((const char *)table_name);
    char extension[6] = "i.dat";
    strcat(table_name, extension);

    HTable *htables;
    int hsizes[ell];
    if ((htables = (HTable *)calloc(ell, sizeof(HTable))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    for (int table_id = 0; table_id < ell; table_id++)
    {
        table_name[name_length] = table_id + '0';
        import(&htables[table_id], &hsizes[table_id], &mt[table_id], table_name);
        printf("** mt : %d\n",mt[table_id]);
    }

    static unsigned char cipher[SHA256_DIGEST_LENGTH];
    Point plain, result;
    uint64_t nb_hash, total_hash = 0;
    timeval start, end;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    gettimeofday(&start, 0);
    for (int i = 0; i < n; i++)
    {
        nb_hash = 0;
        plain = rand() % N;
        hash(&plain, cipher);
        attackSTDxHTG(cipher, htables, hsizes, N, ell, t, mt, &result, &nb_hash);
        if (result == plain)
        {
            nb++;
        }
        total_hash += nb_hash;
    }
    gettimeofday(&end, 0);
    printf("Plains recovered\t: %u / %u (%0.2f%%)\n", nb, n, (100 * (float)nb / n));
    double avg_hash = (double)total_hash / n;
    printf("Average operations\t: %f\n\n", avg_hash);
    printf("Average time to attack\t: %f\n", elapsed(&start, &end) / n);

    // write_results(&avg_hash, 1, "avgOpe.dat");

    for (int table_id = 0; table_id < ell; table_id++)
        free((void *)htables[table_id]);
    free((void *)htables);
}