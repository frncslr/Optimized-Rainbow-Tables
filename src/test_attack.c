#include "../include/test_attack.h"
#include "../include/precomp.h"

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
    initialize(table, 1, size);
    export(table, size, "./tableTestImport.dat");
    printf("Table (exported):");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");
    static Pair *dict[DICTSIZE];
    printf("Importing table in dictionnary\n");
    import(dict, size, "tableTestImport.dat");
    for (int key = 0; key < size+2; key++)
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