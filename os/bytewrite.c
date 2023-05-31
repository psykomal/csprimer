#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define ONE_MEG 1 << 20

int main()
{
    FILE *f = fopen("/tmp/foo", "wa");
    struct stat st;
    int prior_blocks = -1;
    char c = '.';
    printf("%ld\n", sizeof(c));
    for (int i = 0; i < ONE_MEG; i++)
    {
        write(fileno(f), ".", 1);
        fstat(fileno(f), &st);

        if (st.st_blocks != prior_blocks)
        {
            printf("Size: %lld, blocks: %lld, on disk: %lld\n", st.st_size, st.st_blocks, st.st_blocks * 512);
            prior_blocks = st.st_blocks;
        }
    }
    fstat(fileno(f), &st);
    printf("Size: %lld, blocks: %lld, on disk: %lld\n", st.st_size, st.st_blocks, st.st_blocks * 512);
}