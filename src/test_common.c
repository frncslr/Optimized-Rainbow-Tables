#include "../include/test_common.h"

void test_hash()
{
    printf("# Test hash :\n");
    uint32_t point = 123456;
    printf("Value  is : %u\n", point);
    unsigned char c[SHA256_DIGEST_LENGTH];
    hash(&point, c);
    print_hash(c);
    printf("\n");
}

void test_hash_n()
{
    printf("# Test hash n:\n");
    uint32_t n = 1 << 24;
    printf("Time to hash %d : ", n);
    fflush(stdout);
    unsigned char c[SHA256_DIGEST_LENGTH];
    time_t s = time(NULL);
    for (uint32_t point = 0; point < n; point++)
        hash(&point, c);
    time_t e = time(NULL);
    printf("%lds\n", e - s);
    printf("\n");
}

void test_reduction()
{
    printf("# Test reduction :\n");
    unsigned char c[SHA256_DIGEST_LENGTH];
    uint32_t point = 123456;
    printf("Value  is : %u\n", point);
    hash(&point, c);
    print_hash(c);
    reduction(&point, c, 1, 0);
    printf("Reduce is : %d\n", point);
    printf("\n");
}

void test_hash_reduction()
{
    printf("# Test hash reduction :\n");
    unsigned char c[SHA256_DIGEST_LENGTH];
    uint32_t point = 123456;
    printf("Value  is : %u\n", point);
    hash_reduction(&point, c, 1, 0);
    print_hash(c);
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
    printf("Random column start %%997: %u\n", col_start);
    int col_end = col_start + 3;
    unsigned char hashed[SHA256_DIGEST_LENGTH];

    uint32_t point = rand() % N;
    printf("Random point %%N\t\t: %u\n", point);
    uint32_t copy = point;
    printf("Copy of point\t\t: %u\n", copy);
    int nb_hash = 0;
    compute(&point, hashed, table_id, col_start, col_end, &nb_hash);
    printf("Number of hashes\t: %d\n", nb_hash);
    printf("Point computed\t\t: %u\n", point);

    hash_reduction(&copy, hashed, table_id, col_start);
    hash_reduction(&copy, hashed, table_id, col_start + 1);
    hash_reduction(&copy, hashed, table_id, col_start + 2);
    printf("Copy hash reduced \t: %u\n", copy);
}

void test_dict()
{
    printf("# Test dict :\n");
    static Pair *dict[DICTSIZE];
    printf("Dictionary empty\n");
    printf("Fetching key 31\n");
    Pair *pair31 = get(31, dict);
    if (pair31 == NULL)
    {
        printf("Key 31 not found in dictionary\n");
    }
    else
    {
        printf("Pair {%u : %u} found in dictionary", pair31->end, pair31->start);
    }
    printf("Adding pair {31 : 32} to dictionary\n");
    put(31, 32, dict);
    printf("Fetching key 31\n");
    pair31 = get(31, dict);
    if (pair31 == NULL)
    {
        printf("Key 31 not found in dictionary\n");
    }
    else
    {
        printf("Pair {%u : %u} found in dictionary\n", pair31->end, pair31->start);
    }
    printf("Trying add pair {31 : 33} to dictionary\n");
    put(31, 33, dict);
    printf("Fetching key 31\n");
    pair31 = get(31, dict);
    if (pair31 == NULL)
    {
        printf("Key 31 not found in dictionary\n");
    }
    else
    {
        printf("Pair {%u : %u} found in dictionary\n", pair31->end, pair31->start);
    }
    printf("Fetching key 1031\n");
    Pair *pair1031 = get(1031, dict);
    if (pair1031 == NULL)
    {
        printf("Key 1031 not found in dictionary\n");
    }
    else
    {
        printf("Pair {%u : %u} found in dictionary\n", pair1031->end, pair1031->start);
    }
    printf("Adding pair {1031 : 1032} to dictionary\n");
    put(1031, 1032, dict);
    printf("Fetching key 1031\n");
    pair1031 = get(1031, dict);
    if (pair1031 == NULL)
    {
        printf("Key 1031 not found in dictionary\n");
    }
    else
    {
        printf("Pair {%u : %u} found in dictionary\n", pair1031->end, pair1031->start);
    }
    if (present(32, dict))
    {
        printf("Start point 32 found in dictionary\n");
    }
    else
    {
        printf("Start point 32 not found in dictionary T_T\n");
    }
    if (present(14, dict))
    {
        printf("Start point 14 found in dictionary\n");
    }
    else
    {
        printf("Start point 14 not found in dictionary T_T\n");
    }
    printf("\n");
    free_dict(dict);
}