#include "../include/common.h"

unsigned char *hash(uint32_t plain, unsigned char *cipher)
{
    return SHA256((const unsigned char *)&plain, sizeof(uint32_t), cipher);
}

uint32_t reduction(unsigned char *cipher, int id, int col)
{
    return (*(uint32_t *) cipher + id + col * T) % SPACE_SIZE;
}

uint32_t hash_reduction(uint32_t plain, int id, int col)
{
    unsigned char cipher[SHA256_DIGEST_LENGTH];
    return reduction(hash(plain, cipher), id, col);
}