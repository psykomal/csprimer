#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// getrlimit and setrlimit

int f(int depth, long int bottom)
{
    if (depth % 10000 == 0)
    {
        printf("[%d] frame %d %ld (%p)\n", getpid(), depth, bottom - (long)&depth, &depth);
    }
    for (;;)
        ;
    int left = f(depth + 1, bottom);
    return f;
}

int main(int argc, char *argv[])
{
    int depth = 0;
    f(depth, (long)&depth);
}