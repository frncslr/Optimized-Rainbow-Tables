#include "../include/test.h"

void print_hash(unsigned char *hashed_value)
{
    printf("Digest is : ");
    for (unsigned int i = 0; i < 32; i++)
        printf("%02x", hashed_value[i]);
    printf("\n");
}

void hash_n(uint32_t n)
{
    // printf("Time to hash %d : ", n);
    // fflush(stdout);
    unsigned char c[SHA256_DIGEST_LENGTH];
    // time_t s = time(NULL);
    for (uint32_t i = 0; i < n; i++)
        hash(&i, c);
    // time_t e = time(NULL);
    // printf("%lds\n", e - s);
}

void test_reduction()
{
    unsigned char c[SHA256_DIGEST_LENGTH];
    uint32_t point = 123456;
    hash(&point, c);
    reduction(&point, c, 1, 0);
    print_hash(c);
    printf("Reduce is : %d\n", point);
}

void test_hash_reduction()
{
    unsigned char c[SHA256_DIGEST_LENGTH];
    uint32_t point = 123456;
    hash_reduction(&point, c, 1, 0);
    print_hash(c);
    printf("Reduce is : %d\n", point);
}

void test_initialize()
{
    int size = 6;
    Points table[size];
    initialize(table, 1, size);
    printf("Table :");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");
}

void test_generate()
{
    int size = 1 << 15;
    Points *table;
    if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }
    time_t s = time(NULL);
    initialize(table, 0, size);
    time_t i = time(NULL);
    printf("Time to init %d : %lds\n", size, i - s);
    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);
    time_t g = time(NULL);
    printf("Time to gen %d : %lds\n", size, g - i);
    for (Points *current = table, *last = table + 20; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    free((void *)table);
}

void test_swap()
{
    Points a, b;
    a.start = 1;
    a.end = 1;
    b.start = 2;
    b.end = 2;
    printf("Before\t: a = {%d : %d} & b = {%d : %d}\n", a.start, a.end, b.start, b.end);
    swap(&a, &b);
    printf("After\t: a = {%d : %d} & b = {%d : %d}\n", a.start, a.end, b.start, b.end);
}

void test_sort()
{
    int size = 1 << 3;
    Points table[size];
    initialize(table, 0, size);
    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);
    printf("Before :");
    for (Points *current = table, *last = table + size; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    sort(table, 0, size - 1);
    printf("After :");
    for (Points *current = table, *last = table + size; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
}

void test_gen_sort()
{
    int size = 1 << 15;
    Points *table;
    if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem");
        exit(ERROR_ALLOC);
    }
    time_t s = time(NULL);
    initialize(table, 0, size);
    time_t i = time(NULL);
    printf("Time to init %d : %lds\n", size, i - s);
    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);
    time_t g = time(NULL);
    printf("Time to gen %d : %lds\n", size, g - i);
    sort(table, 0, size - 1);
    time_t q = time(NULL);
    printf("Time to sort %d : %lds\n", size, q - g);
    for (Points *current = table, *last = table + 20; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    free((void *)table);
}

void test_clean()
{
    int size = 1 << 12;
    Points *table, *perfect;
    if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    if ((perfect = (Points *)calloc(size, sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    time_t s = time(NULL);
    initialize(table, 0, size);
    time_t i = time(NULL);
    printf("Time to init %d : %lds\n", size, i - s);
    int nb_hash = 0;
    generate(table, 0, size, t, &nb_hash);
    time_t g = time(NULL);
    printf("Time to gen %d : %lds\n", size, g - i);
    sort(table, 0, size - 1);
    time_t q = time(NULL);
    printf("Time to sort %d : %lds\n", size, q - g);
    clean(table, &size, perfect);
    time_t c = time(NULL);
    printf("Time to clean %d : %lds\n", size, c - q);
    if ((perfect = (Points *)realloc((void *)perfect, size * sizeof(Points))) == NULL)
    {
        printf("Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }
    printf("Table cleaned : (%u Points)", size);
    for (Points *current = perfect, *last = perfect + 20; current < last; current++)
        printf("\n%u\t:\t%u", current->start, current->end);
    printf("\n");
    free((void *)table);
    free((void *)perfect);
}

void test_rice()
{
    uint32_t end = 4115;
    printf("before : %u\n", end);
    uint32_t previous = 4099;
    rice(&end, (end - previous - 1), 3);
    printf("after : %u\n", end);
}

void test_ceri()
{
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
    printf("Compressed diff size\t: %d bits\n", size);
    ceri(&end, end, k, size);
    printf("Decompressed difference\t: %u\n", end);
}

void test_export()
{
    int size = 6;
    Points table[size];
    initialize(table, 1, size);
    int nb_hash = 0;
    generate(table, 1, size, t, &nb_hash);
    sort(table, 0, size - 1);
    printf("Table (exported):");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");
    export(table, size, "./table.dat");
}

void test_import()
{
    int size = 6;
    Points table[size];
    import(table, size, "./table.dat");
    printf("Table (imported):");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");
}

void test_precomp()
{
    precomp();
}

void test_dict()
{
    static Pair *dict[DICTSIZE];
    Pair *pair31 = get(31, dict);
    if (pair31 == NULL)
    {
        printf("31 not found in dict\n");
    }
    else
    {
        printf("31 found in dict : %u\n", pair31->start);
    }
    put(31, 32, dict);
    pair31 = get(31, dict);
    if (pair31 == NULL)
    {
        printf("31 not found in dict\n");
    }
    else
    {
        printf("31 found in dict : %u\n", pair31->start);
    }
    put(31, 33, dict);
    pair31 = get(31, dict);
    if (pair31 == NULL)
    {
        printf("31 not found in dict\n");
    }
    else
    {
        printf("31 found in dict : %u\n", pair31->start);
    }

    Pair *pair1031 = get(1031, dict);
    if (pair1031 == NULL)
    {
        printf("1031 not found in dict\n");
    }
    else
    {
        printf("1031 found in dict : %u\n", pair1031->start);
    }
    put(1031, 1032, dict);
    pair1031 = get(1031, dict);
    if (pair1031 == NULL)
    {
        printf("1031 not found in dict\n");
    }
    else
    {
        printf("1031 found in dict : %u\n", pair1031->start);
    }
}