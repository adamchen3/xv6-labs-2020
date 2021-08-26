#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void exec2(char **);

int main(int argc, char *argv[])
{
    char *args[MAXARG];
    char buf[512];

    for (int i = 1; i < argc; i++)
    {
        args[i - 1] = argv[i];
    }

    while (read(0, buf, sizeof(buf)) > 0)
    {
        char a[512];
        int index = 0;
        for (int i = 0; i < sizeof(buf); i++)
        {
            if (buf[i] == '\n')
            {
                if (index > 0)
                {
                    a[index] = '\0';
                    args[argc - 1] = a;
                    args[argc] = '\0';
                    exec2(args);
                }
                index = 0;
            }
            else
            {
                a[index++] = buf[i];
            }
        }
    }

    exit(0);
}

void exec2(char *argv[])
{
    if (fork() > 0)
    {
        wait((int *)0);
    }
    else
    {
        exec(argv[0], argv);
        printf("exec failed!\n");
        exit(1);
    }
}