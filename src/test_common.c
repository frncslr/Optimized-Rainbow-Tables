#include "../include/test_common.h"

void test_elapsed()
{
    struct timeval t0, t1;
    gettimeofday(&t0, 0);
    for (uint64_t i = 0; i < 1e10; i++)
        ;
    gettimeofday(&t1, 0);
    printf("Time difference : %lf\n", elapsed(&t0, &t1));
}

void test_hash()
{
    printf("# Test hash :\n");
    uint32_t point = 123456;
    printf("Value  is : %u\n", point);
    hash(&point, buffer);
    print_hash(buffer);
    printf("\n");
}

void test_hash_n()
{
    printf("# Test hash n:\n");
    int nb_tests = 500;
    uint32_t nb_hash = 1 << 24;
    printf("Hashing %u numbers %d times\n", nb_hash, nb_tests);

    struct timeval start, end;
    double speed, time, total_time = 0.0;
    const char file_name[40] = "data/results/hSpeeds.dat";

    for (int i = 0; i < nb_tests; i++)
    {
        speed = time = 0.0;
        gettimeofday(&start, 0);
        for (uint32_t point = 0; point < nb_hash; point++)
            hash(&point, buffer);
        gettimeofday(&end, 0);
        time = elapsed(&start, &end);
        speed = nb_hash / time;
        write_results(&speed, 1, file_name);
        total_time += time;
    }
    time = total_time / nb_tests;
    printf("Time to hash\t: %lf\n", time);
    speed = nb_hash / time;
    printf("Hash speed\t: %lf\n", speed);
    printf("\n");
}

void test_reduction()
{
    printf("# Test reduction :\n");
    srand(time(NULL));
    uint32_t point = rand() % N;
    printf("Value  is : %u\n", point);
    hash(&point, buffer);
    printf("Digest is : ");
    print_hash(buffer);
    reduction(&point, buffer, 1, 0);
    printf("Reduce is : %d\n\n", point);
}

void test_hash_reduction()
{
    printf("# Test hash reduction :\n");
    srand(time(NULL));
    uint32_t point = rand() % N;
    printf("Value  is : %u\n", point);
    hash_reduction(&point, 1, 0);
    printf("Reduce is : %d\n", point);
    printf("\n");
}

void test_hash_reduction_n()
{
    printf("# Test hash reduction n:\n");
    int nb_tests = 20;
    uint32_t nb_hash = 1 << 24;
    printf("Hashing %u numbers %d times\n", nb_hash, nb_tests);

    struct timeval start, end;
    double difference = 0.0;

    for (int i = 0; i < nb_tests; i++)
    {
        gettimeofday(&start, 0);
        for (uint32_t point = 0, value; point < nb_hash; point++)
        {
            value = point;
            hash_reduction(&value, 1, 0);
        }
        gettimeofday(&end, 0);
        difference += elapsed(&start, &end);
    }
    printf("Time to hash\t: %lf\n", difference / nb_tests);
    printf("Hash speed\t: %lf\n", nb_hash * nb_tests / difference);
    printf("\n");
}

void test_compute()
{
    printf("# Test compute :\n");
    srand(time(NULL));
    int table_id = rand() % 4;
    printf("Random table id %%4\t: %u\n", table_id);
    int col_start = rand() % (t - 3);
    printf("Random column start %%%d: %u\n", t - 3, col_start);
    int col_end = col_start + 3;

    uint32_t point = rand() % N;
    printf("Random point %%N\t\t: %u\n", point);
    uint32_t copy = point;
    printf("Copy of point\t\t: %u\n", copy);
    uint32_t nb_hash = 0;
    printf("Computing point between columns %d & %d\n", col_start, col_end);
    compute(&point, table_id, col_start, col_end, &nb_hash);
    printf("Number of hashes\t: %u\n", nb_hash);
    printf("Point computed\t\t: %u\n", point);

    hash_reduction(&copy, table_id, col_start);
    hash_reduction(&copy, table_id, col_start + 1);
    hash_reduction(&copy, table_id, col_start + 2);
    printf("Copy hash reduced \t: %u\n\n", copy);
}

void test_write_results()
{
    double res[3] = {155631.214, 14.12, 52.0};
    const char file_name[20] = "result.dat";
    write_results(res, 3, file_name);
}

void test_read_results()
{
    double results[6];
    // const char file_name[20] = "result.dat";
    const char file_name[20] = "avgOpe.dat";
    int nb_res = 8;
    read_results(results, nb_res, file_name);
    for (int i = 0; i < nb_res; i++)
        printf("Result %d : %lf\n", i, results[i]);
}

void test_hashtable()
{
    printf("# Test hashtable :\n");

    printf("** mci(t) : %f \t mt : %f\n", mci(t), mt);

    int col_id = t;
    printf("Htable size for column %d : %d\n", col_id, hsize(col_id));

    Hashtable htable;
    int size = 10;
    if ((htable = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    init(htable, size);
    printf("Hashtable empty\n");

    printf("Fetching endpoint 31\n");
    Points *points31 = search(htable, size, 31);
    if (points31 == NULL)
    {
        printf("Endpoint 31 not found in hashtable\n");
    }
    else
    {
        printf("Points {%u : %u} found in hashtable\n", points31->start, points31->end);
    }

    printf("Adding points {32 : 31} to hashtable\n");
    printf("Inserted : %d\n", insert(htable, size, 32, 31));
    printf("Fetching endpoint 31\n");
    points31 = search(htable, size, 31);
    if (points31 == NULL)
    {
        printf("Endpoint 31 not found in hashtable\n");
    }
    else
    {
        printf("Points {%u : %u} found in hashtable\n", points31->start, points31->end);
    }

    printf("Trying add points {33 : 31} to hashtable\n");
    printf("Inserted : %d\n", insert(htable, size, 33, 31));
    printf("Fetching endpoint 31\n");
    points31 = search(htable, size, 31);
    if (points31 == NULL)
    {
        printf("Endpoint 31 not found in hashtable\n");
    }
    else
    {
        printf("Points {%u : %u} found in hashtable\n", points31->start, points31->end);
    }

    printf("Fetching endpoint 1031\n");
    Points *points1031 = search(htable, size, 1031);
    if (points1031 == NULL)
    {
        printf("Endpoint 1031 not found in hashtable\n");
    }
    else
    {
        printf("Points {%u : %u} found in hashtable\n", points1031->start, points1031->end);
    }
    printf("Adding points {1032 : 1031} to hashtable\n");
    printf("Inserted : %d\n", insert(htable, size, 1032, 1031));
    printf("Fetching endpoint 1031\n");
    points1031 = search(htable, size, 1031);
    if (points1031 == NULL)
    {
        printf("Endpoint 1031 not found in hashtable\n");
    }
    else
    {
        printf("Points {%u : %u} found in hashtable\n", points1031->start, points1031->end);
    }

    printf("idx 1 : {%u : %u}\n", htable[1].start, htable[1].end);
    printf("idx 2 : {%u : %u}\n", htable[2].start, htable[2].end);

    printf("\n");
    free(htable);
}