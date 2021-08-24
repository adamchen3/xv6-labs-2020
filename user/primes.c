#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void test(int[]);

int main(int argc, char *argv[])
{
    int fd[2]; // fd1[0] for read, fd1[1] for write

    pipe(fd);

    int pid = fork();

    if (pid > 0)
    {
        close(fd[0]);
        for (int i = 2; i <= 35; i++)
        {
            write(fd[1], &i, sizeof(i));
        }
        int end = 0;
        write(fd[1], &end, sizeof(end));
        wait((int *)0);
        close(fd[1]);
    }
    else
    {
        test(fd);
    }

    exit(0);
}

void test(int left[])
{
    close(left[1]);
    int right[2];
    int hasFork = 0;
    int num;
    int n = 1;
    int isFirstRead = 1;
    while (1)
    {
        read(left[0], &num, sizeof(num));
        if (num == 0)
        {
            break;
        }
        if (isFirstRead == 1)
        {
            isFirstRead = 0;
            n = num;
            fprintf(1, "prime %d\n", num);
            continue;
        }
        else if (num % n != 0)
        {
            if (hasFork == 0)
            {
                hasFork = 1;
                pipe(right);
                if (fork() == 0)
                {
                    close(left[0]);
                    // close(left[1]);
                    // 上面这个不能关了，关了会导致read(right[0])失败，具体原因不知。即使在真实的环境下也是如此。
                    // MD，其实思路一开始是对的，只是上面这个问题导致调试了好久，艹！！！
                    // 不知道是不是因为函数调用的关系，如果只在main中怎么操作应该能关的吧？
                    // fprintf(1, "right[0] = %d\n", right[0]);
                    // int bytes = read(right[0], &num, sizeof(num));
                    // fprintf(1, "read bytes %d\n", bytes);
                    test(right);
                    return;
                }
                else
                {
                    close(right[0]);
                }
            }
            write(right[1], &num, 4);
        }
    }
    if (hasFork)
    {
        int end = 0;
        write(right[1], &end, sizeof(end));
        wait((int *)0);
        close(right[1]);
    }
    close(left[0]);
    return;
}