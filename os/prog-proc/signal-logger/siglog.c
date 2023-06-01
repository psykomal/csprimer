#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <setjmp.h>

volatile uint64_t handled = 0;
volatile int zero = 0;
sigjmp_buf jmp_env_seg, jmp_env_bus, jmp_env_ill, jmp_env_sys, jmp_env_fpe;

void handle(int sig)
{
    handled |= (1 << sig);
    printf("Caught %d: %s (%d total)\n", sig, sys_siglist[sig],
           __builtin_popcount(handled));

    if (sig == SIGSEGV)
    {
        siglongjmp(jmp_env_seg, 1);
    }
    else if (sig == SIGBUS)
    {
        siglongjmp(jmp_env_bus, 1);
    }
    else if (sig == SIGILL)
    {
        siglongjmp(jmp_env_ill, 1);
    }
    else if (sig == SIGSYS)
    {
        siglongjmp(jmp_env_sys, 1);
    }
    else if (sig == SIGFPE)
    {
        siglongjmp(jmp_env_fpe, 1);
    }
    else if (sig == SIGTERM || sig == SIGINT)
    {
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    // Register all valid signals
    for (int i = 0; i < NSIG; i++)
    {
        signal(i, handle);
    }

    // trigger alarm
    alarm(1);

    // create child process and immeditely exit
    if (0 == fork())
    {
        exit(0);
    }

    // seg fault
    int *n = 0;
    if (sigsetjmp(jmp_env_seg, 1) == 0)
    {
        *n = 5;
    }

    int *ptr;
    if (sigsetjmp(jmp_env_bus, 1) == 0)
    {
        *ptr = 302;
    }

    if (sigsetjmp(jmp_env_ill, 1) == 0)
    {
        __asm__ __volatile__(".long 0xffffffff");
    }

    if (sigsetjmp(jmp_env_sys, 1) == 0)
    {
        __asm__ __volatile__("movq $999, %rax;"
                             "syscall");
    }

    if (sigsetjmp(jmp_env_fpe, 1) == 0)
    {
        printf("%d\n", 1 / zero);
    }

    // spin
    for (;;)
        sleep(1);
}
