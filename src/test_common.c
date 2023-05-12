#include "../include/test_common.h"

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
    uint32_t n = 1 << 24;
    printf("Time to hash %d : ", n);
    fflush(stdout);
    time_t s = time(NULL);
    for (uint32_t point = 0; point < n; point++)
        hash(&point, buffer);
    time_t e = time(NULL);
    printf("%lds\n", e - s);
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
    printf("Copy hash reduced \t: %u\n", copy);
}

void test_hashtable()
{
    printf("# Test hashtable :\n");

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