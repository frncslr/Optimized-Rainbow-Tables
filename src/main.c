#include "../include/precomp.h"

void print_hash(unsigned char *hashed_value)
{
  printf("Digest is : ");
  for (unsigned int i = 0; i < 32; i++)
    printf("%02x", hashed_value[i]);
  printf("\n");
}

void hash_n(uint32_t n)
{
  printf("Time to hash %d : ", n);
  fflush(stdout);
  unsigned char c[SHA256_DIGEST_LENGTH];
  time_t s = time(NULL);
  for (uint32_t i = 0; i < n; i++)
    hash(i, c);
  time_t e = time(NULL);
  printf("%lds\n", e - s);
}

int main(void)
{
  hash_n(1 << 24);
  return 0;
}
