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
  int size = 1 << 13;
  Points *table;
  if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
  {
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
  for (Points *current = table, *last = table + 20; current < last; current++)
    printf("\n%u\t:\t%u", current->start, current->end);
  printf("\n");
  free((void *)table);
}

void test_swap()
{
  Points a, b;
  a.start = 1;
  a.end = 1;
  b.start = 2;
  b.end = 2;
  printf("Before\t: a = {%d : %d} & b = {%d : %d}\n", a.start, a.end, b.start, b.end);
  swap(&a, &b);
  printf("After\t: a = {%d : %d} & b = {%d : %d}\n", a.start, a.end, b.start, b.end);
}

void test_sort()
{
  int size = 1 << 3;
  Points table[size];
  initialize(table, 0, size);
  generate(table, 0, size);
  printf("Before :");
  for (Points *current = table, *last = table + size; current < last; current++)
    printf("\n%u\t:\t%u", current->start, current->end);
  printf("\n");
  sort(table, 0, size - 1);
  printf("After :");
  for (Points *current = table, *last = table + size; current < last; current++)
    printf("\n%u\t:\t%u", current->start, current->end);
  printf("\n");
}

void test_gen_sort()
{
  int size = 1 << 15;
  Points *table;
  if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
  {
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
  sort(table, 0, size - 1);
  time_t q = time(NULL);
  printf("Time to sort %d : %lds\n", size, q - g);
  for (Points *current = table, *last = table + 20; current < last; current++)
    printf("\n%u\t:\t%u", current->start, current->end);
  printf("\n");
  free((void *)table);
}

void test_clean()
{
  int size = 1 << 20;
  Points *table, *perfect;
  if ((table = (Points *)calloc(size, sizeof(Points))) == NULL)
  {
    printf("Memory allocation problem");
    exit(ERROR_ALLOC);
  }
  if ((perfect = (Points *)calloc(size, sizeof(Points))) == NULL)
  {
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
  sort(table, 0, size - 1);
  time_t q = time(NULL);
  printf("Time to sort %d : %lds\n", size, q - g);
  clean(table, size, perfect);
  time_t c = time(NULL);
  printf("Time to clean %d : %lds\n", size, c - q);
  printf("Table cleaned :");
  for (Points *current = perfect, *last = perfect + 20; current < last; current++)
    printf("\n%u\t:\t%u", current->start, current->end);
  printf("\n");
  free((void *)table);
  free((void *)perfect);
}

int main(void)
{
  // hash_n(1 << 24);
  // test_reduction();
  // test_hash_reduction();
  // test_initialize();
  // test_generate();
  // test_swap();
  // test_sort();
  // test_gen_sort();
  test_clean();
  return 0;
}
