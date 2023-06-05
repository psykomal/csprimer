#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD 4096
#define MAX_ARGV 256
#define PROMPT "> "
#define SEP " \t\n"

int main()
{
    char cmd[MAX_CMD];
    int argc;
    char *argv[MAX_ARGV];

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
        argc = 0;
        argv[argc] = strtok(cmd, SEP);

        while (argv[argc] != NULL)
        {
            argv[++argc] = strtok(NULL, SEP);
        }

        // printf("%d args: ", argc);
        // for (int i = 0; i < argc; i++)
        // {
        //     printf("%s, ", argv[i]);
        // }
        // printf("\n");

        // eval
        if (0 == strcmp(argv[0], "quit"))
            exit(0);
        if (0 == strcmp(argv[0], "help"))
        {
            printf("Builins are 'quit' and 'help'\n");
            continue;
        }
    }
}