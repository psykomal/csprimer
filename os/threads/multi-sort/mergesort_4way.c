#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "cputime.h"

void merge(int *arr, int n, int mid)
{
    int left = 0, right = mid, i;
    int *x = malloc(n * sizeof(int));
    // copy the ith item from either the left or right part
    for (i = 0; i < n; i++)
    {
        if (right == n)
            x[i] = arr[left++];
        else if (left == mid)
            x[i] = arr[right++];
        else if (arr[right] < arr[left])
            x[i] = arr[right++];
        else
            x[i] = arr[left++];
    }
    // transfer from temporary array back to given one
    for (i = 0; i < n; i++)
        arr[i] = x[i];
    free(x);
}

struct msort_args
{
    int *arr;
    int n;
};

void msort(int *arr, int n);

void *start(void *args)
{
    int *arr = ((struct msort_args *)args)->arr;
    int n = ((struct msort_args *)args)->n;
    msort(arr, n);
    return NULL;
};

void msort(int *arr, int n)
{
    if (n < 2)
        return;
    int mid = n / 2;
    msort(arr, mid);
    msort(arr + mid, n - mid);
    merge(arr, n, mid);
}

void msort_main(int *arr, int n)
{
    if (n < 2)
        return;
    int mid = n / 2;
    int q1 = mid / 2;
    int q3 = mid + (n - mid) / 2;
    pthread_t t1, t2, t3;
    struct msort_args args1 = {arr, q1};
    struct msort_args args2 = {arr + q1, mid - q1};
    struct msort_args args3 = {arr + mid, q3 - mid};
    pthread_create(&t1, NULL, start, (void *)&args1);
    pthread_create(&t2, NULL, start, (void *)&args2);
    pthread_create(&t3, NULL, start, (void *)&args3);
    msort(arr + q3, n - q3);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    // pthread_join(t2, NULL);
    merge(arr, mid, q1);
    merge(arr + mid, n - mid, q3 - mid);
    merge(arr, n, mid);
}

int main()
{
    int n = 1 << 24;
    int *arr = malloc(n * sizeof(int));
    // populate array with n many random integers
    srand(1234);
    for (int i = 0; i < n; i++)
        arr[i] = rand();

    fprintf(stderr, "Sorting %d random integers\n", n);

    // actually sort, and time cpu use
    struct profile_times t;
    profile_start(&t);
    msort_main(arr, n);
    profile_log(&t);

    // assert that the output is sorted
    for (int i = 0; i < n - 1; i++)
        if (arr[i] > arr[i + 1])
        {
            printf("error: arr[%d] = %d > arr[%d] = %d", i, arr[i], i + 1,
                   arr[i + 1]);
            return 1;
        }
    return 0;
}
