#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void searchAndPrint(char *, char *);
void strcat(char *dest, char *src);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(1, "usage: find\n");
        exit(0);
    }

    char buf[512];
    char *path = argv[1];
    char *keyword = argv[2];

    int fd;
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        exit(0);
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        exit(0);
    }

    if (st.type != T_DIR)
    {
        printf("%s is not a directory!\n", path);
        close(fd);
        exit(0);
    }

    close(fd);
    strcpy(buf, path);
    if (buf[strlen(path)] != '/')
    {
        buf[strlen(path)] = '/';
        buf[strlen(path) + 1] = '\0';
    }
    searchAndPrint(buf, keyword);
    exit(0);
}

void searchAndPrint(char *path, char *keyword)
{
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // printf("fd is %d\n", fd);
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        int fd2;
        struct stat st2;
        char buf[512];

        if (de.inum == 0)
            continue;

        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        {
            continue;
        }

        // printf("name is %s\n", de.name);
        //====================
        // 这两句很关键，因为打开需要指定绝对路径或者相对路径，
        // 如果没这两句，当递归深入时，只通过子文件名是打不开子文件的
        // 一句话就是，打开文件需要指定文件的决定路径或者相对路径
        strcpy(buf, path);
        strcat(buf, de.name);
        //=====================
        // fd2 = open(de.name, 0);  // 之前就是因为这里只通过文件名来打开，所以一直失败
        fd2 = open(buf, 0);         // 加上路径就ok了
        // printf("fd2 is %d\n", fd2);

        fstat(fd2, &st2);
        if (st2.type == T_DIR)
        {
            strcpy(buf, path);
            strcat(buf, de.name);
            strcat(buf, "/");
            close(fd2); //  因为下面调用会重新打开的，所以这里必需关掉，不然进去又打开一次，浪费fd
            searchAndPrint(buf, keyword);
        }
        else
        {
            if (strcmp(de.name, keyword) == 0)
            {
                printf("%s%s\n", path, de.name);
            }
            close(fd2);
        }
        // printf("fd2 close\n\n");
    }
    close(fd);
    return;
}

void strcat(char *dest, char *src)
{
    dest += strlen(dest);
    while ((*dest++ = *src++) != '\0')
        ;
}