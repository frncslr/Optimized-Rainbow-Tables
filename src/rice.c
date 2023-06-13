#include "../include/rice.h"

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

void encode(BitStream *stream, uint32_t x)
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

void exportCDE(Points *table, int nb, const char *spFile_name, const char *epFile_name, const char *idxFile_name)
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

    uint32_t difference;
    int j = 0, jj, l = 6, n = 60, first = 1;
    // int addrSize = (int)ceil(log2(nb * R));
    int addrSize = 6;
    // int nbSize = (int)ceil(log2(nb));
    int nbSize = 4;
    for (int i = 0; i < nb; i++)
    {
        if ((jj = (int)floor(table[i].end * l / n)) > j)
        {
            j = jj;
            first = 1;
        }
        if (first)
        {
            writeIdx(&idxStream, epStream.BitTotal, addrSize, i, nbSize);
            difference = table[i].end - (j * n / l);
            first = 0;
        }
        else
        {
            difference = table[i].end - table[i - 1].end - 1;
        }
        encode(&epStream, difference);

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
    *end = ((1 << (value >> k)) - 1) << (k + 1) | (value & ((1 << k) - 1));
}

void ceri(uint32_t *end, uint32_t value, int nb_bits)
{
    *end = ((nb_bits - k - 1) << k) | (value & ((1 << k) - 1));
}