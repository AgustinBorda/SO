#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
int
main(void)
{
  int sd[3];
  sd[0] = semget(1,0); // consumer sem
  sd[1] = semget(2,20); // producer sem
  sd[2] = semget(3,1); // mutex sem

  int fd = open("test.txt",0);
  if (fd < 0) {
    fd = open("test.txt", O_CREATE | O_WRONLY);
    printf(fd,"0");
    close(fd);
  }
  else {
    close(fd);
  }
  
  char* buf = sbrk(5);
  int n;

  for (int i = 0; i < 12; i++) {
    semdown(sd[0]);
    semdown(sd[2]);
    fd = open("test.txt", O_RDONLY);
    read(fd, buf, 3);
    n = atoi(buf);
    n--;
    close(fd);
    fd = open("test.txt", O_CREATE | O_WRONLY);
    printf(fd, "%d", n);
    close(fd);
    semup(sd[1]);
    semup(sd[2]);
  }
  for (int i=0; i < 3; i++)
    semclose(sd[i]);

  return 0;
}
