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
    hash(&i, c);
  time_t e = time(NULL);
  printf("%lds\n", e - s);
}

void test_reduction()
{
  unsigned char c[SHA256_DIGEST_LENGTH];
  uint32_t point = 123456;
  hash(&point, c);
  reduction(&point, c, 1, 0);
  print_hash(c);
  printf("Reduce is : %d\n", point);
}

void test_hash_reduction()
{
  unsigned char c[SHA256_DIGEST_LENGTH];
  uint32_t point = 123456;
  hash_reduction(&point, c, 1, 0);
  print_hash(c);
  printf("Reduce is : %d\n", point);
}

int main(void)
{
  // hash_n(1 << 24);
  // test_reduction();
  // test_hash_reduction();
  return 0;
}
