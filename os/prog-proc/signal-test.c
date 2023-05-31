#include <signal.h>
#include <stdio.h>

void handler(int sig)
{
    printf("Terminal resized\n");
}

int main()
{
    struct sigaction sa;
    sa.sa_handler = handler;
    sigaction(SIGWINCH, &sa, NULL);
    printf("Waiting for signal\n");
    while (1)
    {
    }
}