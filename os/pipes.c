#include <unistd.h>

int main()
{
    int fds[2];
    pipe(fds);
    if (0 == fork())
    {
        dup2(fds[1], 1);
        execlp("ls", "ls", NULL);
    }

    dup2(fds[0], 0);
    close(fds[1]);
    execlp("wc", "wc", NULL);
}