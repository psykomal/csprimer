#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CMD 4096
#define MAX_ARGV 256
#define MAX_CMDS 8
#define PROMPT "💀 "
#define SEP " \t\n"

volatile pid_t childpid = 0;
volatile pid_t childpids[MAX_CMDS];

void sigint_handler(int sig)
{
    if (!childpid)
        return;

    if (kill(childpid, SIGINT), 0)
    {
        perror("Error sending SIGINT to child");
    }
    return;
}

int main()
{
    char cmd[MAX_CMD];
    int argc;
    char *cmds[MAX_CMDS][MAX_ARGV];
    int argi, cmdi;
    char *tok;

    signal(SIGINT, sigint_handler);

    while (1)
    {
        printf(PROMPT);
        if (NULL == fgets(cmd, MAX_CMD, stdin) && ferror(stdin))
        {
            perror("fgets error");
            exit(1);
        }
        if (feof(stdin))
            exit(0);

        // tokenizing
        argi = 0;
        cmdi = 0;
        tok = strtok(cmd, SEP);

        while (tok != NULL)
        {
            if (strcmp(tok, "|") == 0)
            {
                cmds[cmdi++][argi] = NULL;
                argi = 0;
            }
            else
            {
                cmds[cmdi][argi++] = tok;
            }
            tok = strtok(NULL, SEP);
            if (tok == NULL)
            {
                cmds[cmdi][argi++] = tok;
            }
        }

        // eval
        if (cmds[0][0] == 0)
            continue;
        if (0 == strcmp(cmds[0][0], "quit"))
            exit(0);
        if (0 == strcmp(cmds[0][0], "help"))
        {
            printf("Builins are 'quit' and 'help'\n");
            continue;
        }

        int fds[2];
        int infd = 0;
        for (int i = 0; i <= cmdi; i++)
        {
            if (i < cmdi)
                pipe(fds);

            if ((childpids[i] = fork()) < 0)
            {
                perror("fork error");
                exit(1);
            }

            if (childpids[i] == 0)
            {
                if (i < cmdi)
                {
                    dup2(fds[1], 1);
                    close(fds[1]);
                    close(fds[0]);
                }
                dup2(infd, 0);

                if (execvp(cmds[i][0], cmds[i]) < 0)
                {
                    perror("execvp error");
                    exit(1);
                }
                exit(1);
            }

            // parent
            if (i < cmdi)
            {
                infd = fds[0];
                close(fds[1]);
            }
        }

        // parent
        int status;
        for (int i = 0; i <= cmdi; i++)
        {
            waitpid(childpids[i], &status, 0);
        }
        childpid = 0;
    }
}