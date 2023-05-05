#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define STARTING_CAPACITY 8

typedef struct DA
{
    void **data;  // pointer to an array of pointers to elements
    int size;     // current number of elements in the array
    int capacity; // maximum number of elements the array can hold
} DA;

DA *DA_new(void)
{
    DA *da = malloc(sizeof(DA));
    if (da == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate memory.\n");
        exit(1);
    }
    da->data = malloc(sizeof(void *) * STARTING_CAPACITY);
    if (da->data == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate memory.\n");
        exit(1);
    }
    da->size = 0;
    da->capacity = STARTING_CAPACITY;
    return da;
}

int DA_size(DA *da)
{
    return da->size;
}

void DA_push(DA *da, void *x)
{
    if (da->size == da->capacity)
    {
        da->capacity *= 2;
        da->data = realloc(da->data, sizeof(void *) * da->capacity);
        if (da->data == NULL)
        {
            fprintf(stderr, "Error: Unable to allocate memory.\n");
            exit(1);
        }
    }
    da->data[da->size] = x;
    da->size++;
}

void *DA_pop(DA *da)
{
    if (da->size == 0)
    {
        return NULL;
    }
    void *x = da->data[da->size - 1];
    da->size--;
    if (da->size == da->capacity / 2)
    {
        da->capacity /= 2;
        da->data = realloc(da->data, sizeof(void *) * da->capacity);
        if (da->data == NULL)
        {
            fprintf(stderr, "Error: Unable to allocate memory.\n");
            exit(1);
        }
    }
    return x;
}

void DA_set(DA *da, void *x, int i)
{
    if (i >= 0 && i < da->size)
    {
        da->data[i] = x;
    }
}

void *DA_get(DA *da, int i)
{
    if (i >= 0 && i < da->size)
    {
        return da->data[i];
    }
    return NULL;
}

void DA_free(DA *da)
{
    free(da->data);
    free(da);
}

int main()
{
    DA *da = DA_new();

    assert(DA_size(da) == 0);

    // basic push and pop test
    int x = 5;
    float y = 12.4;
    DA_push(da, &x);
    DA_push(da, &y);
    assert(DA_size(da) == 2);

    assert(DA_pop(da) == &y);
    assert(DA_size(da) == 1);

    assert(DA_pop(da) == &x);
    assert(DA_size(da) == 0);
    assert(DA_pop(da) == NULL);

    // basic set/get test
    DA_push(da, &x);
    DA_set(da, &y, 0);
    assert(DA_get(da, 0) == &y);
    DA_pop(da);
    assert(DA_size(da) == 0);

    // expansion test
    DA *da2 = DA_new(); // use another DA to show it doesn't get overriden
    DA_push(da2, &x);
    int i, n = 100 * STARTING_CAPACITY, arr[n];
    for (i = 0; i < n; i++)
    {
        arr[i] = i;
        DA_push(da, &arr[i]);
    }
    assert(DA_size(da) == n);
    for (i = 0; i < n; i++)
    {
        assert(DA_get(da, i) == &arr[i]);
    }
    for (; n; n--)
        DA_pop(da);
    assert(DA_size(da) == 0);
    assert(DA_pop(da2) == &x); // this will fail if da doesn't expand

    DA_free(da);
    DA_free(da2);
    printf("OK\n");
}
