#include <stdio.h>

int main()
{

    int d = 5;
    int32 d1 = 6;
    int32_t d3 = 5;
    void *x = &d;

    printf("%p\n%p  %d", x, x + 1, sizeof(x));
}