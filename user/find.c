#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void searchAndPrint(char *, char *);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(1, "usage: find\n");
        exit(0);
    }

    char *path = argv[1];
    char *keyword = argv[2];

    searchAndPrint(path, keyword);
    exit(0);
}

void searchAndPrint(char *path, char *keyword)
{
    int fd;
    struct dirent de;
    struct stat st;
    int buf[512];

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

    switch (st.type)
    {
    case T_DIR:
        printf("This is a directory\n");
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
                for (int i = 0; i < de.inum; i++) {
                    
                }
        }
        break;
    default:
        printf("%s is not a directory!\n", path);
        break;
    }
}