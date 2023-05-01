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
    int size = 6;
    srand(time(NULL));
    int table_id = rand() % 4;
    Points table[size];

    printf("Initializing and exporting table %d of %d elements\n", table_id, size);
    initialize(table, table_id, size);
    export(table, size, "./tableTestImport.dat");

    printf("Table (exported):");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");

    static Pair *dict[DICTSIZE];
    printf("Importing table in dictionary\n");
    import(dict, size, "tableTestImport.dat");

    printf("Fetching keys in dictionary\n");
    Pair *pair;
    for (int key = 0; key < size * 4; key++)
    {
        if ((pair = get(key, dict)) != NULL)
        {
            printf("Pair {%u : %u} found in dictionary\n", pair->end, pair->start);
        }
    }
    printf("\n");
}

void test_chain()
{
    printf("# Test chain :\n");

    srand(time(NULL));
    int table_id = rand() % 4;
    int table_size = 1 << 4;
    int table_width = t;
    char table_name[] = "tableTestChain.dat";
    unsigned char hashed[SHA256_DIGEST_LENGTH];

    precomp(table_name, table_id, &table_size, table_width);

    static Pair *dict[DICTSIZE];
    import(dict, table_size, table_name);

    uint32_t point;
    do
    {
        point = table_id * table_size + (uint32_t)rand() % table_size;
    } while (!present(point, dict));
    printf("Random existing start point \t: %u\n", point);
    int col_id = rand() % table_width;
    printf("Random existing column \t\t: %d\n", col_id);

    int nb_hash = 0;
    compute(&point, hashed, table_id, 0, col_id, &nb_hash);
    printf("Start point computed \t\t: %u\n", point);
    printf("Number of hashes done \t\t: %d\n", nb_hash);

    hash(&point, hashed);
    print_hash(hashed);

    printf("Number of hashes to chain \t: %d\n", table_width - col_id);
    chain(&point, hashed, table_id, col_id, table_width);
    printf("End point chained \t\t: %u\n", point);
    Pair *pair;
    if ((pair = get(point, dict)) != NULL)
    {
        printf("Pair {sp : %u ; ep : %u} found in dictionary\n", pair->start, pair->end);
    }
    else
    {
        printf("End point not found in dictionary\n");
    }
}

void test_rebuild()
{
    printf("# Test rebuild :\n");
    int table_id = 0;
    int table_size = 16;
    int table_width = 30;
    unsigned char hashed[SHA256_DIGEST_LENGTH];

    srand(time(NULL));
    uint32_t point = (uint32_t)rand() % table_size;
    printf("Ramdom point %%%d \t\t: %u\n", table_size, point);
    uint32_t copy = point;
    printf("Copy of point \t\t\t: %u\n", copy);
    int col_id = rand() % table_width;
    printf("Random column \t\t\t: %d\n", col_id);
    for (int col = 0; col < col_id; col++)
        hash_reduction(&copy, hashed, table_id, col);
    printf("Copy hash reduced %d times \t: %u\n", col_id, copy);
    rebuild(&point, hashed, table_id, col_id);
    printf("Point rebuild \t\t\t: %u\n\n", point);
}

void test_attack_existing()
{
    printf("# Test attack existing:\n");
    int table_id = 0;
    int table_size = 1 << 4;
    int table_width = t;
    char file_name[] = "tableTestAttackExisting.dat";

    // precomp(file_name, &table_size, table_width);

    int dict_size = table_size;
    static Pair *dict[DICTSIZE];
    import(dict, dict_size, file_name);

    unsigned char cipher[SHA256_DIGEST_LENGTH + 1];
    unsigned char hashed[SHA256_DIGEST_LENGTH + 1];
    srand(time(NULL));
    uint32_t plain;
    do
    {
        plain = (uint32_t)rand() % table_size;
    } while (!present(plain, dict));
    printf("Random existing start point : %u\n", plain);
    int col_id = rand() % table_width;
    printf("Random column : %d\n", col_id);
    for (int col = 0; col <= col_id; col++)
        hash_reduction(&plain, hashed, table_id, col);
    printf("Random existing plain : %u\n", plain);
    hash(&plain, cipher);
    print_hash(cipher);
    uint32_t result;
    char found = 0;
    attack(cipher, hashed, dict, table_width, &result, &found);
    if (found && (result == plain))
    {
        printf("Plain recovered : %u\n", (uint32_t)result);
    }
    else
    {
        printf("Hash not recovered T_T\n");
    }
    printf("\n");
    free_dict(dict);
}

void test_attack_existing_n()
{
    printf("# Test attack existing n :\n");
    int n = 100;
    int nb = 0;

    int table_id = 0;
    int table_size = 1 << 4;
    int table_width = t;
    char file_name[] = "tableTestAttackExistingN.dat";

    precomp(file_name, table_id, &table_size, table_width);

    int dict_size = table_size;
    static Pair *dict[DICTSIZE];
    import(dict, dict_size, file_name);
    printf("Dictionary imported\n");

    unsigned char cipher[SHA256_DIGEST_LENGTH + 1];
    unsigned char hashed[SHA256_DIGEST_LENGTH + 1];
    uint32_t plain, sp;
    int col_id;
    char found;
    srand(time(NULL));
    printf("Launching %d attacks\n", n);
    for (int i = 0; i < n; i++)
    {
        found = 0;
        plain = (uint32_t)rand() % table_size;
        if (!present(plain, dict))
        {
            i--;
            continue;
        }
        sp = plain;
        col_id = rand() % table_width;
        for (int col = 0; col <= col_id; col++)
            hash_reduction(&plain, hashed, table_id, col);
        hash(&plain, cipher);
        uint32_t result;
        attack(cipher, hashed, dict, table_width, &result, &found);
        if (found && (result == plain))
        {
            nb++;
        }
        else
        {
            printf("Not recovered : sp : %u \t& col : %d \tfound : %d\n", sp, col_id, present(sp, dict));
        }
    }
    printf("Number of plains recovered : %u / %u\n (%f%%)", nb, n, (100 * (float)nb / n));
    printf("\n");
}

void test_attack_random()
{
    printf("# Test attack 2 :\n");
    int table_id = 0;
    int table_size = 1 << 4;
    int table_width = t;
    char file_name[] = "tableTestAttack2.dat";

    precomp(file_name, table_id, &table_size, table_width);

    int dict_size = table_size;
    static Pair *dict[DICTSIZE];
    import(dict, dict_size, file_name);

    unsigned char cipher[SHA256_DIGEST_LENGTH + 1];
    unsigned char hashed[SHA256_DIGEST_LENGTH + 1];
    srand(time(NULL));
    // uint32_t plain = (uint32_t)rand() % table_size;
    uint32_t plain = 3;
    // uint32_t plain = 284;
    printf("Random start point : %u\n", plain);
    printf("Start point %u present : %d\n", plain, present(plain, dict));
    // int col_id = rand() % table_width;
    // int col_id = 388;
    int col_id = 25;
    printf("Random column : %d\n", col_id);
    for (int col = 0; col <= col_id; col++)
        hash_reduction(&plain, hashed, table_id, col);
    printf("Random existing plain : %u\n", plain);
    hash(&plain, cipher);
    print_hash(cipher);
    uint32_t result;
    char found = 0;
    uint32_t clair = plain;
    printf("Chaining out : clair = %u, table_id = %d, col_id = %d, table_width = %d\n", clair, table_id, col_id, table_width);
    chain(&clair, hashed, table_id, col_id, table_width);
    printf("endpoint chained %d: %u\n", col_id, clair);
    attack(cipher, hashed, dict, table_width, &result, &found);
    if (found && (result == plain))
    {
        printf("Plain recovered : %u\n", (uint32_t)result);
    }
    else
    {
        printf("Hash not recovered T_T\n");
    }
    printf("\n");
    free_dict(dict);
}