#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void printInfo(int, char **);

char buf[512];

int main(int argc, char *argv[])
{
    char *args[MAXARG];

    for (int i = 1; i < argc; i++)
    {
        args[i - 1] = argv[i];
    }

    while (read(0, buf, sizeof(buf)) > 0)
    {
        int len = argc - 1;
        args[len++] = buf;
        char *p = buf;
        while(*p != '\n') {
            p++;
        }
        args[argc - 1] = buf;
        args[argc] = 0;

        if (fork() > 0)
        {
            wait((int *)0);
        }
        else
        {
            // printInfo(argc, args);
            exec(args[0], args);
            // printf("exec failed!\n");
            exit(1);
        }
    }

    exit(0);
}

void printInfo(int len, char *str[])
{
    for (int i = 0; i < len; i++)
    {
        printf("%s\n", str[i]);
    }
}