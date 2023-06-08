#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int main()
{
    pid_t pid;
    if ((pid = fork()) == 0)
    {
        printf("child pid : %d\n", pid);
    }
    else
    {
        printf("parent pid : %d\n", pid);
        while (1)
            ;
        int status;
        waitpid(pid, &status, 0);
    }
}
