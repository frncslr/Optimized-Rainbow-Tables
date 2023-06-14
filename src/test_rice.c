#include "../include/test_rice.h"

void test_variables()
{
    printf("# Test variables :\n");
    int n = 1 << 24;
    n = 60;
    // n = 1 << 20;
    int m = 31921;
    m = 7;
    // m = 1 << 16;
    int L = 1 << 8;
    L = 6;
    // L = 1 << 8;
    int kopt = Kopt(n, m);
    printf("kopt : %d\n", kopt);
    double ropt = Ropt(kopt, n, m);
    printf("ropt : %f\n", ropt);
    int addrSize = addrBits(m, ropt);
    printf("addrSize : %d\n", addrSize);
    int chainSize = chainBits(m);
    printf("chainSize : %d\n", chainSize);
    int mem = memory(m, ropt, L);
    printf("memory : %d\n", mem);
    printf("\n");
}

void test_initBitStream()
{
    printf("# Test initBitStream :\n");
    char table_name[40] = "data/tables/cde/testInitBitStream.dat";
    BitStream stream;
    initBitStream(&stream, table_name);
    printf("Buffer\t: %d\n", stream.BitBuffer);
    printf("Count\t: %d\n", stream.BitCount);
    printf("Limit\t: %d\n", stream.BitLimit);
    printf("Name\t: %s\n", stream.file_name);
    closeBitStream(&stream);
    printf("\n");
}

void test_writeBit()
{
    printf("# Test writeBit :\n");
    char table_name[40] = "data/tables/cde/testWriteBit.dat";
    BitStream stream;
    initBitStream(&stream, table_name);

    writeBit(&stream, 0);
    writeBit(&stream, 1);
    writeBit(&stream, 0);
    writeBit(&stream, 0);
    writeBit(&stream, 1);
    writeBit(&stream, 0);
    writeBit(&stream, 1);
    writeBit(&stream, 1);
    flushStream(&stream);
    closeBitStream(&stream);

    printf("Hexdump on %s should display the following values : 4b (for 75)\n", table_name);
    printf("\n");
}

void test_encode()
{
    printf("# Test encode :\n");
    char table_name[40] = "data/tables/cde/testEncode.dat";
    BitStream stream;
    initBitStream(&stream, table_name);

    uint32_t differences[7], initial[7] = {1, 7, 17, 31, 32, 52, 54};
    differences[0] = initial[0];
    for (int i = 1; i < 7; i++)
    {
        differences[i] = initial[i] - initial[i - 1] - 1;
    }

    for (int i = 0; i < 7; i++)
        encode(&stream, differences[i], 3);

    flushStream(&stream);
    closeBitStream(&stream);

    printf("Hexdump on %s should display the following values : 8d15 3143\n", table_name);
    printf("\n");
}

void test_exportCDE()
{
    printf("# Test exportCDE :\n");
    int table_size = 7;
    Points table[7] = {{0, 1},
                       {1, 7},
                       {2, 17},
                       {3, 31},
                       {4, 32},
                       {5, 52},
                       {6, 54}};
    // for (int i = 0; i < table_size; i++)
    //     printf("%d : {%u : %u}\n", i, table[i].start, table[i].end);

    char spFile_name[40] = "data/tables/cde/spTestExportCDE.dat";
    char epFile_name[40] = "data/tables/cde/epTestExportCDE.dat";
    char idxFile_name[40] = "data/tables/cde/idxTestExportCDE.dat";
    exportCDE(table, table_size, 60, 6, spFile_name, epFile_name, idxFile_name);

    // printf("\nSecond export :\n");
    // uint32_t eps2[8] = {1, 7, 17, 31, 32, 52, 54, 59};
    // exportCDE(eps2, 8);
}

void test_rice()
{
    printf("# Test rice :\n");
    uint32_t previous = 4099;
    printf("Previous endpoint : %u\n", previous);
    uint32_t end = 4115;
    printf("Current endpoint (uncompressed): %u\n", end);
    rice(&end, (end - previous - 1));
    printf("Current endpoint (compressed): %u\n\n", end);
}

void test_ceri()
{
    printf("# Test ceri :\n");
    uint32_t end = 4115;
    uint32_t previous = 4099;
    printf("Current endpoint\t: %u\n", end);
    printf("Previous endpoint\t: %u\n", previous);
    uint32_t difference = end - previous - 1;
    printf("Uncompressed difference\t: %u\n", difference);
    char size = 0;
    for (uint32_t bits = difference; bits; bits >>= 1, size++)
        ;
    printf("Uncompressed bits size\t: %d\n", size);
    rice(&difference, difference);
    printf("Compressed difference\t: %u\n", difference);
    size = 0;
    for (uint32_t bits = difference; bits; bits >>= 1, size++)
        ;
    printf("Compressed bits size\t: %d\n", size);
    ceri(&difference, difference, size);
    printf("Decompressed difference\t: %u\n\n", difference);
}