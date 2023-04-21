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

void test_initialize()
{
  int size = 6;
  Points table[size];
  initialize(table, 1, size);
  generate(table, 1, size);
  printf("Table :");
  for (int i = 0; i < size; i++)
    printf("\n%u\t:\t%u", table[i].start, table[i].end);
  printf("\n");
}

void test_generate()
{
  int size = 1 << 5;
  Points *table;
  if((table = (Points *) calloc(size, sizeof(Points))) == NULL){
    printf("Memory allocation problem");
    exit(ERROR_ALLOC);
  }
  time_t s = time(NULL);
  initialize(table, 0, size);
  time_t i = time(NULL);
  printf("Time to init %d : %lds\n", size, i - s);
  generate(table, 0, size);
  time_t g = time(NULL);
  printf("Time to gen %d : %lds\n", size, g - i);
  for(Points *current = table, *last = table+20; current < last; current++)
    printf("\n%u\t:\t%u", current->start, current->end);
  printf("\n");
}

int main(void)
{
  // hash_n(1 << 24);
  // test_reduction();
  // test_hash_reduction();
  // test_initialize();
  test_generate();
  return 0;
}
