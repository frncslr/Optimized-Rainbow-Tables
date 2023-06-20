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

int memory(int m, double R, int l)
{
    return (int)ceil((m * R + l * (ceil(log2(m * R)) + ceil(log2(m)))) / 8);
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
        writeBit(stream, 1);
    }
}

void exportCDE(Points *table, int table_size, int space_size, int nb_block, const char *spFile_name, const char *epFile_name, const char *idxFile_name)
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

    int kopt = Kopt(space_size, table_size);
    double ropt = Ropt(kopt, space_size, table_size);
    int addrSize = addrBits(table_size, ropt);
    int chainSize = chainBits(table_size);

    uint32_t difference;
    int j = -1, jj;
    for (int i = 0; i < table_size; i++)
    {
        if ((jj = (int)floor(table[i].end * nb_block / space_size)) > j)
        {
            for (; j != jj; j++)
                writeIdx(&idxStream, epStream.BitTotal, addrSize, i, chainSize);
            difference = table[i].end - (j * space_size / nb_block);
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

void importSP(const char *spFile_name, uint32_t **spTable, int *table_size)
{
    FILE *file;
    struct stat stat;

    if ((file = fopen(spFile_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", spFile_name);
        exit(ERROR_FOPEN);
    }

    fstat(fileno(file), &stat);
    *table_size = (int)stat.st_size / 4;

    if ((*spTable = (uint32_t *)calloc(*table_size, sizeof(uint32_t))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    if ((fread(*spTable, sizeof(uint32_t), *table_size, file)) != (size_t)*table_size)
    {
        fprintf(stderr, "Reading file problem : %s\n", spFile_name);
        exit(ERROR_FREAD);
    }

    fclose(file);
}

void importIdx(const char *idxFile_name, int nb_block, int table_size, int space_size, Index **idxTable)
{
    if ((*idxTable = (Index *)calloc(nb_block, sizeof(Index))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    BitStream stream;
    initBitStream(&stream, idxFile_name, 0);

    int kopt = Kopt(space_size, table_size);
    double ropt = Ropt(kopt, space_size, table_size);
    int addrSize = addrBits(table_size, ropt);
    int chainSize = chainBits(table_size);

    for (int block_id = 0; block_id < nb_block; block_id++)
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

uint32_t decode(BitStream *stream, int k, int *nbBits)
{
    uint32_t x = 0;
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
    long byte_address = bit_address / 8;
    fseek(stream->file, byte_address, SEEK_SET);
    stream->BitCount = 0;
    for (byte_address *= 8; byte_address < bit_address; byte_address++)
    {
        readBit(stream);
    }
}

uint32_t *searchCDE(uint32_t endpoint, uint32_t *spTable, BitStream *epStream, Index *idxTable, int table_size, int space_size, int nb_block)
{
    int kopt = Kopt(space_size, table_size);
    double ropt = Ropt(kopt, space_size, table_size);
    int addrSize = addrBits(table_size, ropt);
    int chainSize = chainBits(table_size);
    int block_size = addrSize + chainSize;

    int block_id = (int)floor(endpoint * nb_block / space_size);
    uint32_t bit_address = idxTable[block_id].address;
    uint32_t chain_id = idxTable[block_id].chain_id;

    int nbBits = 0, nbBitsMax;
    if (block_id + 1 == nb_block)
    {
        struct stat stat;
        fstat(fileno(epStream->file), &stat);
        nbBitsMax = (int)stat.st_size * 8;
    }
    else
    {
        nbBitsMax = idxTable[block_id + 1].address - bit_address;
    }

    setStream(epStream, bit_address);
    uint32_t value = block_id * space_size / nb_block + decode(epStream, kopt, &nbBits);
    while (!epStream->eof && nbBits < nbBitsMax && value < endpoint)
    {
        value += decode(epStream, kopt, &nbBits) + 1;
        chain_id++;
    }
    if (value == endpoint)
        return spTable + chain_id;
    return NULL;
}

void cdeStats(int table_size, int space_size, int nb_block, const char *spFile_name, const char *epFile_name, const char *idxFile_name)
{
    int kopt = Kopt(space_size, table_size);
    double ropt = Ropt(kopt, space_size, table_size);
    int addrSize = addrBits(table_size, ropt);
    int chainSize = chainBits(table_size);

    printf("Delta encoding variables :\n");
    printf("\tk optimal\t: %d\n", kopt);
    printf("\tR optimal\t: %f\n", ropt);
    printf("\taddr size\t: %d\n", addrSize);
    printf("\tchain size\t: %d\n", chainSize);

    int ep_memory, expec_ep_memory, diff_ep_memory;
    double diff_ep_memory_perc;
    int sp_memory, expec_sp_memory, diff_sp_memory;
    double diff_sp_memory_perc;
    int cde_total_memory, std_total_memory, diff_total_memory;
    double diff_total_memory_perc;

    expec_ep_memory = memory(table_size, ropt, nb_block);
    expec_sp_memory = table_size * 4;
    std_total_memory = table_size * 4 * 2;

    FILE *file;
    struct stat stat;

    if ((file = fopen(spFile_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", spFile_name);
        exit(ERROR_FOPEN);
    }
    fstat(fileno(file), &stat);
    sp_memory = (int)stat.st_size;
    fclose(file);
    diff_sp_memory = sp_memory - expec_sp_memory;
    diff_sp_memory_perc = diff_sp_memory * 100.0 / expec_sp_memory;

    if ((file = fopen(epFile_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", epFile_name);
        exit(ERROR_FOPEN);
    }
    fstat(fileno(file), &stat);
    ep_memory = (int)stat.st_size;
    fclose(file);
    if ((file = fopen(idxFile_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", idxFile_name);
        exit(ERROR_FOPEN);
    }
    fstat(fileno(file), &stat);
    ep_memory += (int)stat.st_size;
    fclose(file);

    cde_total_memory = sp_memory + ep_memory;
    diff_ep_memory = ep_memory - expec_ep_memory;
    diff_ep_memory_perc = diff_ep_memory * 100.0 / expec_ep_memory;

    diff_total_memory = cde_total_memory - std_total_memory;
    diff_total_memory_perc = diff_total_memory * 100.0 / std_total_memory;

    printf("Delta encoding memory :\n");
    printf("\tstartpoints memory :\n");
    printf("\t\texpected\t: %d\n", expec_sp_memory);
    printf("\t\texperimental\t: %d\n", sp_memory);
    printf("\t\tdifference\t: %d (%3.3lf%%)\n", diff_sp_memory, diff_sp_memory_perc);
    printf("\tendpoints memory :\n");
    printf("\t\texpected\t: %d\n", expec_ep_memory);
    printf("\t\texperimental\t: %d\n", ep_memory);
    printf("\t\tdifference\t: %d (%3.3lf%%)\n", diff_ep_memory, diff_ep_memory_perc);
    printf("\ttotal memory :\n");
    printf("\t\tstandard\t: %d\n", std_total_memory);
    printf("\t\tencoding\t: %d\n", cde_total_memory);
    printf("\t\tdifference\t: %d (%3.3lf%%)\n", diff_total_memory, diff_total_memory_perc);
}

void rice(uint32_t *end, uint32_t value)
{
    *end = ((1 << (value >> kk)) - 1) << (kk + 1) | (value & ((1 << kk) - 1));
}

void ceri(uint32_t *end, uint32_t value, int nb_bits)
{
    *end = ((nb_bits - kk - 1) << kk) | (value & ((1 << kk) - 1));
}