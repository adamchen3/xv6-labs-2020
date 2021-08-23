#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int p[2]; // p[0] for read, p[1] for write
  char buf[2];
  pipe(p);

  if (fork() == 0)
  {
    // 子进程
    read(p[0], buf, 1);
    close(p[0]);
    fprintf(1, "%d: received ping\n", getpid());
    write(p[1], "b", 1);
    close(p[1]);
  }
  else
  {
    // 父进程
    write(p[1], "a", 1);
    close(p[1]);
    wait((int *)0);
    read(p[0], buf, 1);
    close(p[0]);
    fprintf(1, "%d: received pong\n", getpid());
  }
  exit(0);
}
