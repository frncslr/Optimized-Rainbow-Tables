#include "../include/test_common.h"

void print_hash(unsigned char *hashed_value)
{
    printf("Digest is : ");
    for (unsigned int i = 0; i < 32; i++)
        printf("%02x", hashed_value[i]);
    printf("\n");
}

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
    printf("\n");
}