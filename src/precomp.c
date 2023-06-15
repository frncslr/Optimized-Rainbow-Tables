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

void positions(int **filters, int *nb_filters, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "rb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    if ((fread(nb_filters, sizeof(int), 1, file)) != (size_t)1)
    {
        fprintf(stderr, "Reading file problem 1: %s\n", file_name);
        exit(ERROR_FWRITE);
    }

    if ((*filters = (int *)calloc(*nb_filters, sizeof(int))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    if ((fread(*filters, sizeof(int), *nb_filters, file)) != (size_t)*nb_filters)
    {
        fprintf(stderr, "Reading file problem : %s\n", file_name);
        exit(ERROR_FWRITE);
    }

    if (fclose(file))
    {
        fprintf(stderr, "Closing file problem : %s", file_name);
        exit(ERROR_FCLOSE);
    }
}

void clean(Points **table, int *table_size, int htable_size)
{
    Hashtable htable;
    if ((htable = (Points *)calloc(htable_size, sizeof(Points))) == NULL)
    {
        fprintf(stderr, "Memory allocation problem\n");
        exit(ERROR_ALLOC);
    }

    init(htable, htable_size);

    int nb_inserted = 0;
    for (Points *current = *table, *last = *table + *table_size; current < last; current++)
        nb_inserted += insert(htable, htable_size, current->start, current->end);

    if (*table_size != nb_inserted)
    {
        *table_size = nb_inserted;
        if ((*table = (Points *)realloc((void *)(*table), (*table_size) * sizeof(Points))) == NULL)
        {
            printf("Memory allocation problem");
            exit(ERROR_ALLOC);
        }
    }

    for (Points *inserted = *table, *current = htable, *last = htable + htable_size; current < last; current++)
        if (current->end != MAX)
        {
            inserted->start = current->start;
            inserted->end = current->end;
            inserted++;
        }

    free((void *)htable);
}

void generate(Points *table, int table_id, int *table_size, int *filters, int nb_filters, uint32_t *nb_hash)
{
    struct timeval start, end;
    double total_compute, total_clean;
    for (int col_start = 0, *col_end = filters, *last = filters + nb_filters; col_end < last; col_start = *(col_end++))
    {
        for (Points *current = table, *last = table + *table_size; current < last; current++)
        {
            gettimeofday(&start, 0);
            compute(&(current->end), table_id, col_start, *col_end, nb_hash);
            gettimeofday(&end, 0);
            total_compute += elapsed(&start, &end);
        }
        gettimeofday(&start, 0);
        clean(&table, table_size, hsize(*col_end));
        gettimeofday(&end, 0);
        total_clean += elapsed(&start, &end);
    }
    printf("Time to compute\t\t: %fs\nTime to clean\t\t: %fs\n", total_compute, total_clean);
}

void operations(int *filters, int nb_filters, uint32_t *expec_hash)
{
    double total = 0.0;
    for (int previous = 0, *current = filters, *last = filters + nb_filters; current < last; previous = *(current++))
        total += mci(previous) * (*current - previous);
    *expec_hash = (uint32_t)ceil(total);
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

void quicksort(Points *array, int low, int high)
{
    if (low < high)
    {

        // find the pivot element such that
        // elements smaller than pivot are on left of pivot
        // elements greater than pivot are on right of pivot
        int pi = partition(array, low, high);

        // recursive call on the left of pivot
        quicksort(array, low, pi - 1);

        // recursive call on the right of pivot
        quicksort(array, pi + 1, high);
    }
}

void sort(Points *table, int table_size)
{
    quicksort(table, 0, table_size - 1);
}

void precompute(Points **table, int table_id, int *table_size, int *filters, int nb_filters, uint32_t *nb_hash)
{
    initialize(*table, table_id, *table_size);

    generate(*table, table_id, table_size, filters, nb_filters, nb_hash);
    sort(*table, *table_size);
}

void export(Points *table, int table_size, const char *file_name)
{
    FILE *file;
    if ((file = fopen(file_name, "wb")) == (FILE *)NULL)
    {
        fprintf(stderr, "Opening file problem : %s\n", file_name);
        exit(ERROR_FOPEN);
    }

    if ((fwrite(table, sizeof(Points), table_size, file)) != (size_t)table_size)
    {
        fprintf(stderr, "Writing file problem : %s\n", file_name);
        exit(ERROR_FWRITE);
    }

    if (fclose(file))
    {
        fprintf(stderr, "Closing file problem : %s", file_name);
        exit(ERROR_FCLOSE);
    }
}

void cover(Points *table, int table_id, int table_size, int table_width, char *covered, int *coverage)
{
    uint32_t point;
    for (Points *current = table, *last = table + table_size; current < last; current++)
    {
        point = current->start;
        for (int col_id = 0; col_id < table_width; col_id++)
        {
            if (!covered[point])
            {
                covered[point] = 1;
                ++*coverage;
            }
            hash_reduction(&point, table_id, col_id);
        }
        if (point != current->end)
        {
            fprintf(stderr, "Cover problem : endpoint not retrieved\n");
            exit(ERROR_COVER);
        }
    }
}

void hashStats(uint32_t nb_hash, int *filters, int nb_filters)
{
    uint32_t expec_hash = 0;
    operations(filters, nb_filters, &expec_hash);
    int diff_hash = nb_hash - expec_hash;
    double diff_hash_perc = (double)diff_hash * 100 / expec_hash;
    printf("Hash operations :\n");
    printf("\texpected\t: %u\n", expec_hash);
    printf("\texperimental\t: %u\n", nb_hash);
    printf("\tdifference\t: %d (%3.2lf%%)\n", diff_hash, diff_hash_perc);
}

void epStats(int table_size, int expec_size)
{
    int diff_size = table_size - expec_size;
    double diff_size_perc = (double)diff_size * 100 / expec_size;
    printf("Unique endpoints :\n");
    printf("\texpected\t: %d\n", expec_size);
    printf("\texperimental\t: %d\n", table_size);
    printf("\tdifference\t: %d (%3.2lf%%)\n", diff_size, diff_size_perc);
}

void coverStats(int coverage, int space_size)
{
    double expec_coverage_perc = (1 - pow((double)1 - mt / N, (double)t)) * 100;
    double coverage_perc = (double)coverage * 100 / space_size;
    double diff_coverage_perc = coverage_perc - expec_coverage_perc;
    printf("Coverage of the table :\n");
    printf("\texpected\t: %3.2lf%%\n", expec_coverage_perc);
    printf("\texperimental\t: %3.2lf%%\n", coverage_perc);
    printf("\tdifference\t: %3.2lf%%\n", diff_coverage_perc);
}

void cdeStats(int table_size, int space_size, int nb_block, const char *spFile_name,const char *epFile_name,const char *idxFile_name)
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
    uint32_t file_size;
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