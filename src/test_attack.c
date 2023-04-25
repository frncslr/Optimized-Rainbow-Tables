#include "../include/test_attack.h"
#include "../include/precomp.h"

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

void test_import()
{
    int size = 6;
    Points table[size];
    // import(table, size, "./table.dat");
    printf("Table (imported):");
    for (int i = 0; i < size; i++)
        printf("\n%u\t:\t%u", table[i].start, table[i].end);
    printf("\n");
}