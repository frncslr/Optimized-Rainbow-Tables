#include "../include/rice.h"

int Kopt(uint64_t N, int m)
{
    return 1 + (int)floor(log2(log(PHI - 1) / log((N - m) / (N + 1.0))));
}
double Ropt(uint64_t N, int m, int k)
{
    return k + 1.0 / (1 - pow((N - m) / (N + 1.0), 1 << k));
}
int addressBitSize(int m, double R)
{
    return (int)ceil(log2(m * R));
}
int chainIdBitSize(int m)
{
    return (int)ceil(log2(m));
}
int Lblocks(int m)
{
    return m >> 8;
}
int memory(int m, double R, int L)
{
    return (int)ceil((m * R + L * (ceil(log2(m * R)) + ceil(log2(m)))) / 8);
}

void initBitStream(BitStream *stream, const char *file_name, char mode)
{
    stream->BitBuffer = 0;
    stream->BitCount = 0;
    stream->BitLimit = 7;
    stream->BitTotal = 0;
    stream->file_name = file_name;
    if (mode)
    {
        if ((stream->file = fopen(stream->file_name, "wb")) == (FILE *)NULL)
        {
            fprintf(stderr, "Opening file problem : %s\n", stream->file_name);
            exit(ERROR_FOPEN);
        }
    }
    else
    {
        if ((stream->file = fopen(stream->file_name, "rb")) == (FILE *)NULL)
        {
            fprintf(stderr, "Opening file problem : %s\n", stream->file_name);
            exit(ERROR_FOPEN);
        }
    }
    stream->eof = 0;
}
void closeBitStream(BitStream *stream)
{
    if (fclose(stream->file))
    {
        fprintf(stderr, "Closing file problem : %s", stream->file_name);
        exit(ERROR_FCLOSE);
    }
}
void writeBit(BitStream *stream, uint8_t b)
{
    stream->BitBuffer = (stream->BitBuffer << 1) | (b & 1);
    if (stream->BitCount == stream->BitLimit)
    {
        if ((fwrite(&(stream->BitBuffer), sizeof(stream->BitBuffer), 1, stream->file)) != 1)
        {
            fprintf(stderr, "Writing file problem : %s\n", stream->file_name);
            exit(ERROR_FWRITE);
        }
        stream->BitBuffer = 0;
        stream->BitCount = 0;
    }
    else
        stream->BitCount++;
    stream->BitTotal++;
}
void encode(BitStream *stream, Point x, int k)
{
    int i, q = x >> k;
    for (i = 0; i < q; i++)
        writeBit(stream, 1);
    writeBit(stream, 0);
    for (i = k - 1; i >= 0; i--)
        writeBit(stream, (x >> i) & 1);
}
void writeIdx(BitStream *stream, uint64_t addr, int addrSize, int chainId, int chainIdSize)
{
    char size = 0;
    for (uint64_t bits = addr; bits; bits >>= 1, size++)
        ;
    for (int i = addrSize - size; i; i--)
        writeBit(stream, 0);
    for (int i = size - 1; i >= 0; i--)
        writeBit(stream, (addr >> i) & 1);
    size = 0;
    for (uint64_t bits = chainId; bits; bits >>= 1, size++)
        ;
    for (int i = chainIdSize - size; i; i--)
        writeBit(stream, 0);
    for (int i = size - 1; i >= 0; i--)
        writeBit(stream, (chainId >> i) & 1);
}
void flushStream(BitStream *stream)
{
    while (stream->BitCount)
    {
        writeBit(stream, 1);
    }
}
void exportCDE(RTable table, int m, uint64_t N, int L, const char *spFile_name, const char *epFile_name, const char *idxFile_name)
{
    BitStream epStream, idxStream;
    initBitStream(&epStream, epFile_name, 1);
    initBitStream(&idxStream, idxFile_name, 1);

    FILE *spFile;
    if ((spFile = fopen(spFile_name, "wb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", spFile_name);
        exit(ERROR_FOPEN);
    }

    int k = Kopt(N, m);
    double R = Ropt(N, m, k);
    int addrSize = addressBitSize(m, R);
    int chainSize = chainIdBitSize(m);

    Point difference;
    int j = -1, jj;
    for (int i = 0; i < m; i++)
    {
        if ((jj = (int)floor(table[i].ep * L / N)) > j)
        {
            for (; j != jj; j++)
                writeIdx(&idxStream, epStream.BitTotal, addrSize, i, chainSize);
            difference = table[i].ep - (j * N / L);
        }
        else
        {
            difference = table[i].ep - table[i - 1].ep - 1;
        }

        encode(&epStream, difference, k);

        if ((fwrite(&(table[i].sp), sizeof(Point), 1, spFile)) != 1)
        {
            fprintf(stderr, "Writing file problem : %s\n", spFile_name);
            exit(ERROR_FWRITE);
        }
    }
    flushStream(&epStream);
    flushStream(&idxStream);
    closeBitStream(&epStream);
    closeBitStream(&idxStream);
    if (fclose(spFile))
    {
        fprintf(stderr, "Closing file problem : %s", spFile_name);
        exit(ERROR_FCLOSE);
    }
}

int readBit(BitStream *stream)
{
    if (!stream->BitCount)
    {
        if ((fread(&(stream->BitBuffer), sizeof(stream->BitBuffer), 1, stream->file)) != 1)
        {
            stream->eof = 1;
            return 0;
        }
        stream->BitCount = stream->BitLimit + 1;
    }

    int bit = (stream->BitBuffer >> (stream->BitLimit)) & 1;
    stream->BitBuffer <<= 1;
    stream->BitCount--;
    return bit;
}
void importSP(const char *spFile_name, Point **spTable, int *m)
{
    FILE *file;
    struct stat stat;

    if ((file = fopen(spFile_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", spFile_name);
        exit(ERROR_FOPEN);
    }

    fstat(fileno(file), &stat);
    *m = (int)stat.st_size / sizeof(Point);

    if ((*spTable = (Point *)calloc(*m, sizeof(Point))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    if ((fread(*spTable, sizeof(Point), *m, file)) != (size_t)*m)
    {
        fprintf(stderr, "Reading file problem : %s\n", spFile_name);
        exit(ERROR_FREAD);
    }

    fclose(file);
}
void importIdx(const char *idxFile_name, int L, int m, uint64_t N, Index **idxTable)
{
    if ((*idxTable = (Index *)calloc(L, sizeof(Index))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    BitStream stream;
    initBitStream(&stream, idxFile_name, 0);

    int k = Kopt(N, m);
    double R = Ropt(N, m, k);
    int addrSize = addressBitSize(m, R);
    int chainSize = chainIdBitSize(m);

    for (int block_id = 0; block_id < L; block_id++)
    {
        (*idxTable)[block_id].address = 0;
        (*idxTable)[block_id].chain_id = 0;
        for (int i = 0; i < addrSize && !stream.eof; i++)
        {
            (*idxTable)[block_id].address <<= 1;
            (*idxTable)[block_id].address |= readBit(&stream);
        }
        for (int i = 0; i < chainSize && !stream.eof; i++)
        {
            (*idxTable)[block_id].chain_id <<= 1;
            (*idxTable)[block_id].chain_id |= readBit(&stream);
        }
        if (stream.eof)
        {
            fprintf(stderr, "Reading file problem : %s\n", stream.file_name);
            exit(ERROR_FREAD);
        }
    }

    closeBitStream(&stream);
}
Point decode(BitStream *stream, int k, int *nbBits, uint32_t *nbDec)
{
    *nbDec += 1;
    Point x = 0;
    while (readBit(stream))
        x++;
    *nbBits += k + x + 1;
    x <<= k;
    for (k--; k >= 0 && !stream->eof; k--)
        x = x | (readBit(stream) << k);
    return x;
}
void setStream(BitStream *stream, uint32_t bit_address)
{
    stream->eof = 0;
    long byte_address = bit_address / 8;
    fseek(stream->file, byte_address, SEEK_SET);
    stream->BitCount = 0;
    for (byte_address *= 8; byte_address < bit_address; byte_address++)
    {
        readBit(stream);
    }
}
Point *searchCDE(Point endpoint, Point *spTable, BitStream *epStream, Index *idxTable, int m, int N, int L, uint32_t *nbDec)
{
    int k = Kopt(N, m);
    double R = Ropt(k, N, m);
    int addrSize = addressBitSize(m, R);
    int chainSize = chainIdBitSize(m);
    int block_size = addrSize + chainSize;

    int block_id = (int)floor(endpoint * L / N);
    uint32_t bit_address = idxTable[block_id].address;
    uint32_t chain_id = idxTable[block_id].chain_id;

    int nbBits = 0, nbBitsMax;
    if (block_id + 1 == L)
    {
        struct stat stat;
        fstat(fileno(epStream->file), &stat);
        nbBitsMax = (int)stat.st_size * 8 - bit_address;
    }
    else
    {
        nbBitsMax = idxTable[block_id + 1].address - bit_address;
    }

    setStream(epStream, bit_address);
    Point value = block_id * N / L + decode(epStream, k, &nbBits, nbDec);
    while (!epStream->eof && nbBits < nbBitsMax && value < endpoint)
    {
        value += decode(epStream, k, &nbBits, nbDec) + 1;
        chain_id++;
    }
    if (value == endpoint)
        return spTable + chain_id;
    return NULL;
}

void cdeStats(int ell, int *m, int N, int *L, char *spFile_name, char *epFile_name, char *idxFile_name)
{
    int k[ell];
    double R[ell];
    int addrSize[ell];
    int chainSize[ell];

    int expec_sp_memory = 0;
    int sp_memory = 0;
    int diff_sp_memory = 0;
    double diff_sp_memory_perc = 0.0;

    int expec_std_ep_memory = 0;
    int expec_cde_ep_memory = 0;
    int cde_ep_memory = 0;
    int diff_cde_ep_memory = 0;
    int diff_ep_memory = 0;
    double diff_cde_ep_memory_perc = 0.0;
    double diff_ep_memory_perc = 0.0;

    int std_total_memory = 0;
    int cde_total_memory = 0;
    int diff_total_memory = 0;
    double diff_total_memory_perc = 0.0;

    FILE *file;
    struct stat stat;
    int spName_length = strlen((const char *)spFile_name) - 5;
    int epName_length = strlen((const char *)epFile_name) - 5;
    int idxName_length = strlen((const char *)idxFile_name) - 5;
    for (int table_id = 0; table_id < ell; table_id++)
    {
        k[table_id] = Kopt(N, m[table_id]);
        R[table_id] = Ropt(N, m[table_id], k[table_id]);
        addrSize[table_id] = addressBitSize(m[table_id], R[table_id]);
        chainSize[table_id] = chainIdBitSize(m[table_id]);

        expec_sp_memory += m[table_id] * sizeof(Point);
        expec_std_ep_memory += m[table_id] * sizeof(Point);
        expec_cde_ep_memory += memory(m[table_id], R[table_id], L[table_id]);
        std_total_memory += m[table_id] * sizeof(Point) * 2;

        spFile_name[spName_length] = table_id + '0';
        epFile_name[epName_length] = table_id + '0';
        idxFile_name[idxName_length] = table_id + '0';
        if ((file = fopen(spFile_name, "rb")) == (FILE *)NULL)
        {
            fprintf(stderr, "Opening file problem : %s\n", spFile_name);
            exit(ERROR_FOPEN);
        }
        fstat(fileno(file), &stat);
        sp_memory += (int)stat.st_size;
        fclose(file);

        if ((file = fopen(epFile_name, "rb")) == (FILE *)NULL)
        {
            fprintf(stderr, "Opening file problem : %s\n", epFile_name);
            exit(ERROR_FOPEN);
        }
        fstat(fileno(file), &stat);
        cde_ep_memory += (int)stat.st_size;
        fclose(file);

        if ((file = fopen(idxFile_name, "rb")) == (FILE *)NULL)
        {
            fprintf(stderr, "Opening file problem : %s\n", idxFile_name);
            exit(ERROR_FOPEN);
        }
        fstat(fileno(file), &stat);
        cde_ep_memory += (int)stat.st_size;
        fclose(file);
    }

    diff_sp_memory = sp_memory - expec_sp_memory;
    diff_sp_memory_perc += diff_sp_memory * 100.0 / expec_sp_memory;

    diff_ep_memory = cde_ep_memory - expec_std_ep_memory;
    diff_ep_memory_perc = diff_ep_memory * 100.0 / expec_std_ep_memory;
    diff_cde_ep_memory = cde_ep_memory - expec_cde_ep_memory;
    diff_cde_ep_memory_perc = diff_cde_ep_memory * 100.0 / expec_cde_ep_memory;
    cde_total_memory = (sp_memory + cde_ep_memory);

    diff_total_memory = cde_total_memory - std_total_memory;
    diff_total_memory_perc = diff_total_memory * 100.0 / std_total_memory;

    printf("Delta encoding variables :");
    printf("\n\tk optimal\t:");
    for (int table_id = 0; table_id < ell; table_id++)
        printf(" %d", k[table_id]);
    printf("\n\tR optimal\t:");
    for (int table_id = 0; table_id < ell; table_id++)
        printf(" %3.3f", R[table_id]);
    printf("\n\taddress size\t:");
    for (int table_id = 0; table_id < ell; table_id++)
        printf(" %d", addrSize[table_id]);
    printf("\n\tchain id size\t:");
    for (int table_id = 0; table_id < ell; table_id++)
        printf(" %d", chainSize[table_id]);
    printf("\n\tL blocks\t:");
    for (int table_id = 0; table_id < ell; table_id++)
        printf(" %d", L[table_id]);

    printf("\n\nDelta encoding memory :\n");
    printf("\tstartpoints memory :\n");
    printf("\t\texpected\t: %d\n", expec_sp_memory);
    printf("\t\texperimental\t: %d\n", sp_memory);
    printf("\t\tdifference\t: %d (%3.3lf%%)\n", diff_sp_memory, diff_sp_memory_perc);
    printf("\tendpoints memory :\n");
    printf("\t\texpected\t: %d\n", expec_cde_ep_memory);
    printf("\t\texperimental\t: %d\n", cde_ep_memory);
    printf("\t\tdifference\t: %d (%3.3lf%%)\n", diff_cde_ep_memory, diff_cde_ep_memory_perc);
    printf("\t\tstandard\t: %d\n", expec_std_ep_memory);
    printf("\t\tencoding\t: %d\n", cde_ep_memory);
    printf("\t\tdifference\t: %d (%3.3f%%)\n", diff_ep_memory, diff_ep_memory_perc);
    printf("\ttotal memory :\n");
    printf("\t\tstandard\t: %d\n", std_total_memory);
    printf("\t\tencoding\t: %d\n", cde_total_memory);
    printf("\t\tdifference\t: %d (%3.3lf%%)\n", diff_total_memory, diff_total_memory_perc);
}