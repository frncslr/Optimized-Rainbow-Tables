#include "../include/test_common.h"

void test_variables_std()
{
    printf("# Test variables std\n");
    double alpha, r;
    r = 20.0;
    alpha = ALPHA(r);
    printf("For r = %2.3f : alpha = %2.3f\n", r, alpha);
    alpha = 0.95;
    r = R(alpha);
    printf("For alpha = %2.3f : r = %2.3f\n", alpha, r);

    uint64_t N = 1 << 24;
    int t = 1000;
    r = 20.0;
    printf("For N = %lu, r = %2.3f, t = %d :\n", N, r, t);
    alpha = ALPHA(r);
    int m0 = M0(N, r, t);
    printf("\tm0 = %d\n", m0);
    printf("\tmi%d = %d\n", 0, Mi(N, m0, 0));
    int mt = Mt(N, alpha, t);
    printf("\tmt = %d\n", mt);
    printf("\tmi%d = %d\n", t, Mi(N, m0, t));
    int i = 349;
    printf("\tmi%d = %d\n", i, Mi(N, m0, i));
}

void test_hash()
{
    printf("# Test hash :\n");
    Point point = 123456;
    printf("Value  is\t: %lu\n", point);
    hash(&point, buffer);
    printf("Digest is\t: ");
    print_hash(buffer);
    printf("\n");
}
void test_hash_n()
{
    printf("# Test hash n:\n");
    int nb_tests = 100;
    uint64_t nb_hash = 1 << 24;
    printf("Hashing %lu numbers %d times\n", nb_hash, nb_tests);

    timeval start, end;
    double speed, time, total_time = 0.0;
    const char result_file[40] = "data/results/hSpeeds.dat";

    for (int i = 0; i < nb_tests; i++)
    {
        speed = time = 0.0;
        gettimeofday(&start, 0);
        for (Point point = 0; point < nb_hash; point++)
            hash(&point, buffer);
        gettimeofday(&end, 0);
        time = elapsed(&start, &end);
        speed = nb_hash / time;
        write_results(&speed, 1, result_file);
        total_time += time;
    }
    time = total_time / nb_tests;
    printf("Time to hash\t: %f\n", time);
    speed = nb_hash / time;
    printf("Hash speed\t: %f\n", speed);
    printf("\n");
}
void test_reduce()
{
    printf("# Test reduce :\n");
    uint64_t N = 1 << 24;
    int t = 1000;
    srand(time(NULL));
    Point point = rand() % N;
    printf("Value  is : %lu\n", point);
    hash(&point, buffer);
    printf("Digest is : ");
    print_hash(buffer);
    reduce(&point, buffer, 1, 0, t, N);
    printf("Reduce is : %lu\n\n", point);
}
void test_hash_reduce()
{
    printf("# Test hash reduce :\n");
    uint64_t N = 1 << 24;
    int t = 1000;
    srand(time(NULL));
    Point point = rand() % N;
    printf("Value  is : %lu\n", point);
    hash_reduce(&point, 1, 0, t, N);
    printf("Reduce is : %lu\n", point);
    printf("\n");
}
void test_hash_reduce_n()
{
    printf("# Test hash reduce n:\n");
    int nb_tests = 100;
    uint64_t nb_hash = 1 << 24;
    printf("Hash-reducing %lu numbers %d times\n", nb_hash, nb_tests);

    uint64_t N = 1 << 24;
    int t = 1000;

    timeval start, end;
    double time = 0.0;

    for (int i = 0; i < nb_tests; i++)
    {
        gettimeofday(&start, 0);
        for (Point point = 0, value; point < nb_hash; point++)
        {
            value = point;
            hash_reduce(&value, 1, 0, t, N);
        }
        gettimeofday(&end, 0);
        time += elapsed(&start, &end);
    }
    time /= nb_tests;
    printf("Time to hash\t: %f\n", time);
    printf("Hash speed\t: %f\n", nb_hash / time);
    printf("\n");
}
void test_compute()
{
    printf("# Test compute :\n");
    uint64_t N = 1 << 24;
    int t = 1000;
    printf("N : %lu\nt : %d\n", N, t);
    srand(time(NULL));

    int table_id = rand() % 4;
    printf("Random table id\t: %d\n", table_id);
    int nb_col = rand() % (t + 1);
    printf("Random nb of columns\t: %d\n", nb_col);
    int col_start = rand() % (t - nb_col + 1);
    printf("Random column start\t: %d\n", col_start);
    int col_end = col_start + nb_col;

    Point point = rand() % N;
    printf("Random point\t\t: %lu\n", point);
    uint64_t nb_hash = 0;
    printf("Computing point between columns %d & %d\n", col_start, col_end);
    compute(&point, table_id, col_start, col_end, t, N, &nb_hash);
    printf("Number of hashes\t: %lu\n", nb_hash);
    printf("Point computed\t\t: %lu\n", point);
}
void test_compute_n()
{
    printf("# Test compute n :\n");
    int nb_comp = 1 << 19;
    printf("Computing points %d times\n", nb_comp);

    uint64_t N = 1 << 24;
    int t = 1000;
    printf("N : %lu\nt : %d\n", N, t);
    srand(time(NULL));

    Point point;
    uint64_t nb_hash;
    int table_id, nb_col, col_start, col_end;
    timeval start, end;
    double time = 0.0;
    uint64_t nb_col_total = 0;

    for (int i = 0; i < nb_comp; i++)
    {
        table_id = rand() % 4;
        nb_col = 1 + rand() % t;
        nb_col_total += nb_col;
        col_start = rand() % (t - nb_col + 1);
        col_end = col_start + nb_col;
        nb_hash = 0;
        point = rand() % N;
        gettimeofday(&start, 0);
        compute(&point, table_id, col_start, col_end, t, N, &nb_hash);
        gettimeofday(&end, 0);
        time += elapsed(&start, &end);
    }
    printf("Computation speed\t: %f\n", nb_col_total / time);
    printf("\n");
}

void test_htable()
{
    printf("# Test htable :\n");
    uint64_t N = 1 << 24;
    int t = 1000;
    double r = 20.0;
    int m0 = M0(N, r, t);
    printf("Htable htsize for column %d : %d\n", t, sizeHTable(N, m0, t));

    HTable htable;
    int htsize = 10;
    initHTable(&htable, htsize);
    printf("Hashtable empty\n");

    printf("Fetching endpoint 31\n");
    Chain *chain31 = search(htable, htsize, 31);
    if (chain31 == NULL)
    {
        printf("Endpoint 31 not found in hashtable\n");
    }
    else
    {
        printf("Chain {%lu : %lu} found in hashtable\n", chain31->sp, chain31->ep);
    }

    Chain chain = {32, 31};
    printf("Adding chain {%lu : %lu} to hashtable\n", chain.sp, chain.ep);
    printf("Inserted : %d\n", insert(htable, htsize, &chain));
    printf("Fetching endpoint 31\n");
    chain31 = search(htable, htsize, 31);
    if (chain31 == NULL)
    {
        printf("Endpoint 31 not found in hashtable\n");
    }
    else
    {
        printf("Chain {%lu : %lu} found in hashtable\n", chain31->sp, chain31->ep);
    }

    chain.sp = 33;
    printf("Trying add chain {%lu : %lu} to hashtable\n", chain.sp, chain.ep);
    printf("Inserted : %d\n", insert(htable, htsize, &chain));
    printf("Fetching endpoint 31\n");
    chain31 = search(htable, htsize, 31);
    if (chain31 == NULL)
    {
        printf("Endpoint 31 not found in hashtable\n");
    }
    else
    {
        printf("Points {%lu : %lu} found in hashtable\n", chain31->sp, chain31->ep);
    }

    printf("Fetching endpoint 1031\n");
    Chain *chain1031 = search(htable, htsize, 1031);
    if (chain1031 == NULL)
    {
        printf("Endpoint 1031 not found in hashtable\n");
    }
    else
    {
        printf("Points {%lu : %lu} found in hashtable\n", chain1031->sp, chain1031->ep);
    }
    chain.sp = 1032;
    chain.ep = 1031;
    printf("Adding points chain {%lu : %lu} to hashtable\n", chain.sp, chain.ep);
    printf("Inserted : %d\n", insert(htable, htsize, &chain));
    printf("Fetching endpoint 1031\n");
    chain1031 = search(htable, htsize, 1031);
    if (chain1031 == NULL)
    {
        printf("Endpoint 1031 not found in hashtable\n");
    }
    else
    {
        printf("Points {%lu : %lu} found in hashtable\n", chain1031->sp, chain1031->ep);
    }

    printf("idx 1 : {%lu : %lu}\n", htable[1].sp, htable[1].ep);
    printf("idx 2 : {%lu : %lu}\n", htable[2].sp, htable[2].ep);

    printf("\n");
    free(htable);
}

void test_elapsed()
{
    printf("# Test elapsed\n");
    struct timeval t0, t1;
    gettimeofday(&t0, 0);
    for (uint64_t i = 0; i < 1e10; i++)
        ;
    gettimeofday(&t1, 0);
    printf("Time difference : %lf\n", elapsed(&t0, &t1));
}
void test_write_results()
{
    printf("# Test write results :\n");
    double res[3] = {155631.214, 14.12, 52.0};
    const char file_name[40] = "data/results/result.dat";
    printf("Writing 3 values in %s :\n", file_name);
    for (int i = 0; i < 3; i++)
        printf("%d : %f\n", i, res[i]);
    write_results(res, 3, file_name);
}
void test_read_results()
{
    printf("# Test read results :\n");
    double results[6];
    const char file_name[40] = "data/results/result.dat";
    int nb_res = 3;
    read_results(results, nb_res, file_name);
    for (int i = 0; i < nb_res; i++)
        printf("Result %d : %lf\n", i, results[i]);
}