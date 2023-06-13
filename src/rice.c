#include "../include/rice.h"

void initBitStream(BitStream *stream)
{
    stream->BitBuffer = 0;
    stream->BitCount = 0;
    stream->BitLimit = 7;
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

void flushStream(BitStream *stream)
{
    while (stream->BitCount)
    {
        writeBit(stream, 0);
    }
}

void exportCDE(uint32_t *eps, int nb)
{
    BitStream stream;
    initBitStream(&stream);

    uint32_t difference;
    int j = 0, jj, l = 6, n = 600, first = 1;
    for (int i = 0; i < nb; i++)
    {
        printf("ep : %u\t", eps[i]);
        if ((jj = (int)floor(eps[i] * l / n)) > j)
        {
            j = jj;
            first = 1;
        }
        printf("first : %d\tj : %d\t", first, j);
        if (first)
        {
            difference = eps[i] - (j * n / l);
            first = 0;
        }
        else
        {
            difference = eps[i] - eps[i - 1] - 1;
        }
        printf("diff : %u\n", difference);
        encode(&stream, difference);
    }
    flushStream(&stream);
}

void rice(uint32_t *end, uint32_t value)
{
    *end = ((1 << (value >> k)) - 1) << (k + 1) | (value & ((1 << k) - 1));
}

void ceri(uint32_t *end, uint32_t value, int nb_bits)
{
    *end = ((nb_bits - k - 1) << k) | (value & ((1 << k) - 1));
}