#include "../include/rice.h"

int Kopt(int n, int m)
{
    return 1 + (int)floor(log2(log(PHI - 1) / log((n - m) / (n + 1.0))));
}

double Ropt(int k, int n, int m)
{
    return k + 1.0 / (1 - pow((n - m) / (n + 1.0), 1 << k));
}

int addrBits(int m, double R)
{
    return (int)ceil(log2(m * R));
}

int chainBits(int m)
{
    return (int)ceil(log2(m));
}

int memory(int m, double R, int L)
{
    return (int)ceil((m * R + L * (ceil(log2(m * R)) + ceil(log2(m)))) / 8);
}

void initBitStream(BitStream *stream, const char *file_name)
{
    stream->BitBuffer = 0;
    stream->BitCount = 0;
    stream->BitLimit = 7;
    stream->BitTotal = 0;
    stream->file_name = file_name;
    if ((stream->file = fopen(stream->file_name, "wb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", stream->file_name);
        exit(ERROR_FOPEN);
    }
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

void encode(BitStream *stream, uint32_t x, int k)
{
    int i, q = x >> k;
    for (i = 0; i < q; i++)
        writeBit(stream, 1);
    writeBit(stream, 0);
    for (i = k - 1; i >= 0; i--)
        writeBit(stream, (x >> i) & 1);
}

void writeIdx(BitStream *stream, uint64_t addr, int addrSize, int nb, int nbSize)
{
    char size = 0;
    for (uint64_t bits = addr; bits; bits >>= 1, size++)
        ;
    for (int i = addrSize - size; i; i--)
        writeBit(stream, 0);
    for (int i = size - 1; i >= 0; i--)
        writeBit(stream, (addr >> i) & 1);
    size = 0;
    for (uint64_t bits = nb; bits; bits >>= 1, size++)
        ;
    for (int i = nbSize - size; i; i--)
        writeBit(stream, 0);
    for (int i = size - 1; i >= 0; i--)
        writeBit(stream, (nb >> i) & 1);
}

void flushStream(BitStream *stream)
{
    while (stream->BitCount)
    {
        writeBit(stream, 0);
    }
}

void exportCDE(Points *table, int table_size, int space_size, int nb_block, const char *spFile_name, const char *epFile_name, const char *idxFile_name)
{
    BitStream epStream, idxStream;
    initBitStream(&epStream, epFile_name);
    initBitStream(&idxStream, idxFile_name);

    FILE *spFile;
    if ((spFile = fopen(spFile_name, "wb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", spFile_name);
        exit(ERROR_FOPEN);
    }

    int kopt = Kopt(space_size, table_size);
    double ropt = Ropt(kopt, space_size, table_size);
    int addrSize = addrBits(table_size, ropt);
    int chainSize = chainBits(table_size);

    printf("kopt : %d\n", kopt);
    printf("ropt : %f\n", ropt);
    printf("addrSize : %d\n", addrSize);
    printf("chainSize : %d\n", chainSize);
    int mem = memory(table_size, ropt, nb_block);
    printf("memory : %d\n", mem);

    uint32_t difference;
    int j = 0, jj, first = 1;
    for (int i = 0; i < table_size; i++)
    {
        if ((jj = (int)floor(table[i].end * nb_block / space_size)) > j)
        {
            j = jj;
            first = 1;
        }
        if (first)
        {
            writeIdx(&idxStream, epStream.BitTotal, addrSize, i, chainSize);
            difference = table[i].end - (j * space_size / nb_block);
            first = 0;
        }
        else
        {
            difference = table[i].end - table[i - 1].end - 1;
        }
        encode(&epStream, difference, kopt);

        if ((fwrite(&(table[i].start), sizeof(uint32_t), 1, spFile)) != 1)
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

void rice(uint32_t *end, uint32_t value)
{
    *end = ((1 << (value >> kk)) - 1) << (kk + 1) | (value & ((1 << kk) - 1));
}

void ceri(uint32_t *end, uint32_t value, int nb_bits)
{
    *end = ((nb_bits - kk - 1) << kk) | (value & ((1 << kk) - 1));
}