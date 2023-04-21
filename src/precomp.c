#include "../include/precomp.h"

void initialize(Points *table, int table_id, int table_size)
{
    Points *current;
    Points *last;
    uint32_t i, offset = table_id * table_size;
    for (i = 0, current = table, last = table + table_size; current < last; current++, i++)
    {
        current->start = i + offset;
        current->end = i + offset;
    }
}

void generate(Points *table, int table_id, int table_size)
{
    unsigned char hashed[SHA256_DIGEST_LENGTH];
    for (Points *current = table, *last = table + table_size; current < last; current++)
        for (uint32_t col_id = 0; col_id < T; col_id++)
            hash_reduction(&(current->end), hashed, table_id, col_id);
}

void swap(Points *a, Points *b)
{
    Points tmp = *a;
    *a = *b;
    *b = tmp;
}

int partition(Points *array, int low, int high)
{

    // select the rightmost element as pivot
    Points pivot = array[high];

    // pointer for greater element
    int i = (low - 1);

    // traverse each element of the array
    // compare them with the pivot
    for (int j = low; j < high; j++)
    {
        if (array[j].end <= pivot.end)
        {

            // if element smaller than pivot is found
            // swap it with the greater element pointed by i
            i++;

            // swap element at i with element at j
            swap(&array[i], &array[j]);
        }
    }

    // swap the pivot element with the greater element at i
    swap(&array[i + 1], &array[high]);

    // return the partition point
    return (i + 1);
}

void sort(Points *array, int low, int high)
{
    if (low < high)
    {

        // find the pivot element such that
        // elements smaller than pivot are on left of pivot
        // elements greater than pivot are on right of pivot
        int pi = partition(array, low, high);

        // recursive call on the left of pivot
        sort(array, low, pi - 1);

        // recursive call on the right of pivot
        sort(array, pi + 1, high);
    }
}

void clean(Points *table, int table_size, Points *perfect)
{
    uint32_t previous = table->end - 1;
    for (Points *current = table, *last = table + table_size, *new = perfect; current < last; current++)
        if (current->end > previous)
        {
            *new = *current;
            previous = (new ++)->end;
        }
}