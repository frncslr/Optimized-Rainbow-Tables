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
    int l = 1 << 8;
    l = 6;
    // L = 1 << 8;
    int kopt = Kopt(n, m);
    printf("kopt : %d\n", kopt);
    double ropt = Ropt(kopt, n, m);
    printf("ropt : %f\n", ropt);
    int addrSize = addrBits(m, ropt);
    printf("addrSize : %d\n", addrSize);
    int chainSize = chainBits(m);
    printf("chainSize : %d\n", chainSize);
    int mem = memory(m, ropt, l);
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
    int space_size = 60, nb_block = 6;
    Points table[8] = {{0, 1},
                       {1, 7},
                       {2, 17},
                       {3, 31},
                       {4, 32},
                       {5, 52},
                       {6, 54},
                       {7, 59}};

    int table_size1 = 7;
    printf("First export of %d pairs :\n", table_size1);
    char spFile_name1[40] = "data/tables/cde/spTestExportCDE1.dat";
    char epFile_name1[40] = "data/tables/cde/epTestExportCDE1.dat";
    char idxFile_name1[40] = "data/tables/cde/idxTestExportCDE1.dat";
    exportCDE(table, table_size1, space_size, nb_block, spFile_name1, epFile_name1, idxFile_name1);
    printf("Hexdump on %s should display the values between 0 and %d\n", spFile_name1, table_size1 - 1);
    printf("Hexdump on %s should display the following values : 6433 0022\n", epFile_name1);
    printf("Hexdump on %s should display the following values : 3a00 8d5b\n\n", idxFile_name1);

    int table_size2 = 8;
    printf("\nSecond export of %d pairs :\n", table_size2);
    char spFile_name2[40] = "data/tables/cde/spTestExportCDE2.dat";
    char epFile_name2[40] = "data/tables/cde/epTestExportCDE2.dat";
    char idxFile_name2[40] = "data/tables/cde/idxTestExportCDE2.dat";
    exportCDE(table, table_size2, space_size, nb_block, spFile_name2, epFile_name2, idxFile_name2);
    printf("Hexdump on %s should display the values between 0 and %d\n", spFile_name2, table_size2 - 1);
    printf("Hexdump on %s should display the following values : 6433 0023\n", epFile_name2);
    printf("Hexdump on %s should display the following values : 0e00 688b 00d0\n\n", idxFile_name1);
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