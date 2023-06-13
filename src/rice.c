#include "../include/rice.h"

void initBitStream(BitStream *stream)
{
    stream->BitBuffer = 0;
    stream->BitCount = 0;
    stream->BitLimit = 7;
    stream->BitTotal = 0;
}

void writeBit(BitStream *stream, uint8_t b)
{
    stream->BitBuffer = (stream->BitBuffer << 1) | (b & 1);
    if (stream->BitCount == stream->BitLimit)
    {
        printf("Buffer filled : %d\n", stream->BitBuffer);
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

void exportCDE(uint32_t *eps, int nb)
{
    BitStream epStream, idxStream;
    initBitStream(&epStream);
    initBitStream(&idxStream);

    uint32_t difference;
    int j = 0, jj, l = 6, n = 60, first = 1;
    // int addrSize = (int)ceil(log2(nb * R));
    int addrSize = 6;
    // int nbSize = (int)ceil(log2(nb));
    int nbSize = 4;
    for (int i = 0; i < nb; i++)
    {
        printf("ep\t: %u\n", eps[i]);
        if ((jj = (int)floor(eps[i] * l / n)) > j)
        {
            j = jj;
            first = 1;
        }
        printf("first\t: %d\nj\t: %d\n", first, j);
        if (first)
        {
            printf("total\t: %lu & i : %d\n", epStream.BitTotal, i);
            printf("IDX written\n");
            writeIdx(&idxStream, epStream.BitTotal, addrSize, i, nbSize);
            difference = eps[i] - (j * n / l);
            first = 0;
        }
        else
        {
            difference = eps[i] - eps[i - 1] - 1;
        }
        printf("diff\t: %u\n", difference);
        encode(&epStream, difference);
        printf("\n");
    }
    flushStream(&epStream);
    flushStream(&idxStream);
}

void rice(uint32_t *end, uint32_t value)
{
    *end = ((1 << (value >> k)) - 1) << (k + 1) | (value & ((1 << k) - 1));
}

void ceri(uint32_t *end, uint32_t value, int nb_bits)
{
    *end = ((nb_bits - k - 1) << k) | (value & ((1 << k) - 1));
}