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
    Points table[size];
    printf("Initializing and exporting a table of %d elements\n", size);
    initialize(table, 0, size);
    export(table, size, "./tableTestImport.dat");
    printf("Table (exported):");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");
    static Pair *dict[DICTSIZE];
    printf("Importing table in dictionnary\n");
    import(dict, size, "tableTestImport.dat");
    for (int key = 0; key < size + 2; key++)
    {
        printf("Fetching key %d in dictionary\n", key);
        Pair *pair = get(key, dict);
        if (pair == NULL)
        {
            printf("Key %d not found in dictionary\n", key);
        }
        else
        {
            printf("Pair {%u : %u} found in dictionary\n", pair->end, pair->start);
        }
    }
    printf("\n");
}

void test_chain()
{
    printf("# Test chain :\n");
    int size = 6;
    int table_id = 0;
    int table_width = 30;
    Points table[size];
    printf("Initializing and generating a table of %d elements\n", size);
    initialize(table, table_id, size);
    int nb_hash = 0;
    generate(table, table_id, size, table_width, &nb_hash);
    printf("Table :");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");
    srand(time(NULL));
    uint32_t init = (uint32_t)rand() % size;
    Points point = {init, init};
    printf("Point initialized randomly : {%u : %u}\n", point.start, point.end);
    unsigned char hashed[SHA256_DIGEST_LENGTH];
    int col_id = rand() % table_width;
    for (int i = 0; i < col_id; i++)
        hash_reduction(&(point.end), hashed, table_id, i);
    printf("Point hash reduced %d times : {%u : %u}\n", col_id, point.start, point.end);
    hash(&(point.end), hashed);
    chain(&(point.end), hashed, table_id, col_id, table_width);
    printf("Point chained : {%u : %u}\n\n", point.start, point.end);
}

void test_rebuild()
{
    printf("# Test rebuild :\n");
    int table_id = 0;
    int table_size = 6;
    int table_width = 30;
    srand(time(NULL));
    uint32_t init = (uint32_t)rand() % table_size;
    Points point = {init, init};
    printf("Point initialized randomly : {%u : %u}\n", point.start, point.end);
    unsigned char hashed[SHA256_DIGEST_LENGTH];
    int col_id = rand() % table_width;
    for (int i = 0; i < col_id; i++)
        hash_reduction(&(point.end), hashed, table_id, i);
    printf("EndpointPoint hash reduced %d times : {%u : %u}\n", col_id, point.start, point.end);
    rebuild(&(point.start), hashed, table_id, col_id);
    printf("StartPoint rebuild : {%u : %u}\n\n", point.start, point.end);
}

void test_attack()
{
    printf("# Test attack :\n");
    int table_id = 0;
    int table_size = 7;
    int perfect_size = table_size;
    int table_width = t;
    char file_name[] = "tableTestAttack.dat";

    // precomp(file_name, table_size, table_width);

    unsigned char cipher[SHA256_DIGEST_LENGTH + 1];
    unsigned char hashed[SHA256_DIGEST_LENGTH + 1];
    srand(time(NULL));
    uint32_t plain = (uint32_t) rand() % table_size;
    printf("Random start point : %u\n", plain);
    int col_id = rand() % table_width;
    printf("Random column : %d\n", col_id);
    for (int col = 0; col < col_id; col++)
        hash_reduction(&plain, hashed, table_id, col);
    printf("Random existing plain : %u\n", plain);
    hash(&plain, cipher);
    print_hash(cipher);
    attack(cipher, file_name, perfect_size, table_width);
    printf("\n");
}