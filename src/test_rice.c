#include "../include/test_rice.h"

void test_variables_cde()
{
    printf("# Test variables :\n");
    uint64_t N = 1 << 24;
    int t = 1000;
    double r = 20.0;
    double alpha = ALPHA(r);
    printf("For N = %lu, r = %2.3f, t = %d :\n", N, r, t);

    int mt = Mt(N, alpha, t);
    printf("mt : %d\n", mt);
    int L = Lblocks(mt);
    printf("L : %d\n", L);
    int kopt = Kopt(N, mt);
    printf("kopt : %d\n", kopt);
    double ropt = Ropt(N, mt, kopt);
    printf("ropt : %f\n", ropt);
    int addrSize = addressBitSize(mt, ropt);
    printf("addrSize : %d\n", addrSize);
    int chainSize = chainIdBitSize(mt);
    printf("chainSize : %d\n", chainSize);
    int mem = memory(mt, ropt, L);
    printf("memory : %d\n", mem);
    printf("\n");
}

void test_initBitStream()
{
    printf("# Test initBitStream :\n");
    char table_name[40] = "data/tables/cde/testInitBitStream.dat";
    BitStream stream;
    initBitStream(&stream, table_name, 1);
    printf("Buffer\t: %d\n", stream.BitBuffer);
    printf("Count\t: %d\n", stream.BitCount);
    printf("Limit\t: %d\n", stream.BitLimit);
    printf("Name\t: %s\n", stream.file_name);
    printf("EoF\t: %d\n", stream.eof);
    closeBitStream(&stream);
    printf("\n");
}
void test_writeBit()
{
    printf("# Test writeBit :\n");
    char table_name[40] = "data/tables/cde/testWriteBit.dat";
    BitStream stream;
    initBitStream(&stream, table_name, 1);

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
    initBitStream(&stream, table_name, 1);

    Point differences[7], initial[7] = {1, 7, 17, 31, 32, 52, 54};
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
void test_exportCDE1()
{
    printf("# Test exportCDE 1 :\n");
    int ell = 1;
    int N = 60;
    int mt = 7;
    int L = 6;
    Chain table[7] = {{0, 1},
                      {1, 7},
                      {2, 17},
                      {3, 31},
                      {4, 32},
                      {5, 52},
                      {6, 54}};

    printf("Export of %d pairs :\n", mt);
    char spFile_name[40] = "data/tables/cde/spTestExportCDE10.dat";
    char epFile_name[40] = "data/tables/cde/epTestExportCDE10.dat";
    char idxFile_name[40] = "data/tables/cde/idxTestExportCDE10.dat";
    exportCDE(table, mt, N, L, spFile_name, epFile_name, idxFile_name);
    printf("Hexdump on %s should display the values between 0 and %d\n", spFile_name, mt - 1);
    printf("Hexdump on %s should display the following values : 6433 0022\n", epFile_name);
    printf("Hexdump on %s should display the following values : 3a00 8d5b\n\n", idxFile_name);
    cdeStats(ell, &mt, N, &L, spFile_name, epFile_name, idxFile_name);
}
void test_exportCDE2()
{
    printf("# Test exportCDE 2 :\n");
    int ell = 1;
    int N = 60;
    int L = 6;
    int mt = 8;
    Chain table[8] = {{0, 1},
                      {1, 7},
                      {2, 17},
                      {3, 31},
                      {4, 32},
                      {5, 52},
                      {6, 54},
                      {7, 59}};

    printf("Export of %d pairs :\n", mt);
    char spFile_name[40] = "data/tables/cde/spTestExportCDE20.dat";
    char epFile_name[40] = "data/tables/cde/epTestExportCDE20.dat";
    char idxFile_name[40] = "data/tables/cde/idxTestExportCDE20.dat";
    exportCDE(table, mt, N, L, spFile_name, epFile_name, idxFile_name);
    printf("Hexdump on %s should display the values between 0 and %d\n", spFile_name, mt - 1);
    printf("Hexdump on %s should display the following values : 6433 0023\n", epFile_name);
    printf("Hexdump on %s should display the following values : 0e00 688b 00d0\n\n", idxFile_name);
    cdeStats(ell, &mt, N, &L, spFile_name, epFile_name, idxFile_name);
}
void test_exportCDE3()
{
    printf("# Test exportCDE 3 :\n");
    int ell = 1;
    uint64_t N = 60;
    int mt = 12;
    int L = 6;
    Chain table[12] = {{0, 1},
                       {1, 7},
                       {2, 17},
                       {3, 18},
                       {4, 21},
                       {5, 23},
                       {6, 26},
                       {7, 32},
                       {8, 42},
                       {9, 45},
                       {10, 54},
                       {11, 59}};

    char spFile_name[40] = "data/tables/cde/spTestExportCDE30.dat";
    char epFile_name[40] = "data/tables/cde/epTestExportCDE30.dat";
    char idxFile_name[40] = "data/tables/cde/idxTestExportCDE30.dat";
    exportCDE(table, mt, N, L, spFile_name, epFile_name, idxFile_name);
    cdeStats(ell, &mt, N, &L, spFile_name, epFile_name, idxFile_name);
}

void test_readBit()
{
    printf("# Test readBit :\n");
    char table_name[40] = "data/tables/cde/testWriteBit.dat";
    BitStream stream;
    initBitStream(&stream, table_name, 0);
    char eof = 0;
    for (int i = 0; i < 9; i++)
    {
        printf("Bit %d\t: %d\n", i, readBit(&stream));
        if (stream.eof)
            printf("Eof\t: %d\n", stream.eof);
    }

    closeBitStream(&stream);
}
void test_importSP()
{
    printf("# Test importSP :\n");
    char spFile_name[40] = "data/tables/cde/spTestExportCDE30.dat";
    Point *spTable = NULL;
    int mt;

    importSP(spFile_name, &spTable, &mt);
    printf("Startpoints imported (%d) :\n", mt);
    for (int i = 0; i < mt; i++)
        printf("SP %d : %lu\n", i, spTable[i]);
    printf("\n");
    free((void *)spTable);
}
void test_importIdx()
{
    printf("# Test importIdx :\n");
    int N = 60;
    int L = 6;
    int mt = 12;
    char idxFile_name[40] = "data/tables/cde/idxTestExportCDE30.dat";
    Index *idxTable = NULL;

    importIdx(idxFile_name, L, mt, N, &idxTable);

    for (int i = 0; i < L; i++)
        printf("Index %d : {%u : %u}\n", i, idxTable[i].address, idxTable[i].chain_id);
    printf("\n");
    free((void *)idxTable);
}
void test_decode()
{
    printf("# Test decode :\n");
    int N = 60;
    int mt = 12;
    char epFile_name[40] = "data/tables/cde/epTestExportCDE30.dat";

    BitStream stream;
    initBitStream(&stream, epFile_name, 0);

    Point diff;
    uint32_t nbDec = 0;
    int kopt = Kopt(N, mt), nbBits = 0;
    for (int i = 0; i < mt && !stream.eof; i++)
    {
        diff = decode(&stream, kopt, &nbBits, &nbDec);
        if (!stream.eof)
            printf("Diff %d\t: %lu\n", i, diff);
    }
    printf("\n");
    closeBitStream(&stream);
}
void test_setStream()
{
    printf("# Test setStream :\n");
    int N = 60;
    int mt = 12;
    int kopt = Kopt(N, mt);
    char epFile_name[40] = "data/tables/cde/epTestExportCDE30.dat";

    BitStream stream;
    initBitStream(&stream, epFile_name, 0);
    setStream(&stream, 32);

    int nbBits = 0;
    uint32_t diff, nbDec = 0;
    for (int i = 0; i < mt && !stream.eof; i++)
    {
        diff = decode(&stream, kopt, &nbBits, &nbDec);
        if (!stream.eof)
            printf("Diff %d\t: %u\n", i, diff);
    }
    printf("\n");
    closeBitStream(&stream);
}
void test_searchCDE()
{
    printf("# Test searchCDE :\n");
    uint64_t N = 60;
    int L = 6;
    int mt;
    char spFile_name[40] = "data/tables/cde/spTestExportCDE30.dat";
    char epFile_name[40] = "data/tables/cde/epTestExportCDE30.dat";
    char idxFile_name[40] = "data/tables/cde/idxTestExportCDE30.dat";

    Point *spTable = NULL;
    importSP(spFile_name, &spTable, &mt);

    Index *idxTable = NULL;
    importIdx(idxFile_name, L, mt, N, &idxTable);

    BitStream epStream;
    initBitStream(&epStream, epFile_name, 0);

    uint32_t nbDec = 0;
    for (Point ep = 0, *sp = NULL; ep < N; ep++)
    {
        sp = searchCDE(ep, spTable, &epStream, idxTable, mt, N, L, &nbDec);
        if (sp != NULL)
            printf("Found : {%lu : %lu}\n", *sp, ep);
    }
    printf("\n");
    closeBitStream(&epStream);
    free((void *)spTable);
    free((void *)idxTable);
}
void test_searchPrecompCDE()
{
    printf("# Test searchPrecompCDE :\n");
    uint64_t N = 1 << 24;
    int t = 1000;

    uint32_t nb_test = (uint32_t)N;

    int table_id = 0;
    int m, L;
    char spFile_name[40] = "data/tables/cde/spCDE";
    char epFile_name[40] = "data/tables/cde/epCDE";
    char idxFile_name[40] = "data/tables/cde/idxCDE";
    char extension[6] = "i.dat";
    *extension = table_id + '0';
    strcat(spFile_name, extension);
    strcat(epFile_name, extension);
    strcat(idxFile_name, extension);

    Point *spTable = NULL;
    importSP(spFile_name, &spTable, &m);
    L = Lblocks(m);

    Index *idxTable = NULL;
    importIdx(idxFile_name, L, m, N, &idxTable);

    BitStream epStream;
    initBitStream(&epStream, epFile_name, 0);

    int nb_found = 0;
    uint32_t nbDec = 0;
    for (Point ep = 0, *sp = NULL; ep < nb_test; ep++)
    {
        if ((sp = searchCDE(ep, spTable, &epStream, idxTable, m, N, L, &nbDec)) != NULL)
        {
            nb_found++;
            if (!(nb_found % 1000))
                printf("nb found : %d/%lu\n", nb_found, ep);
        }
    }
    printf("Final amount found\t: %d/%d\n", nb_found, nb_test);
    printf("Average decodings\t: %f\n", (double)nbDec / nb_test);
    printf("\n");
    closeBitStream(&epStream);
    free((void *)spTable);
    free((void *)idxTable);
}