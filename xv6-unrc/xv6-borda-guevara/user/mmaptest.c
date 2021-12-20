#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


void
mkfile(void)
{
  int fd = open("test.txt", O_CREATE | O_RDWR);
  for(int i=0; i<250; i++){
    printf(fd, "Archivo 1 numero %d\n\0",i);
  }
  close(fd);
}

void
permissionmmaptest(void)
{
  int fd = open("test.txt", O_WRONLY);
  uint dir1 = mmap(fd);
  close(fd);
  fd = open("test.txt", O_RDONLY);
  uint dir2 = mmap(fd);
  close(fd);
  fd = open("test.txt", O_WRONLY);
  uint dir3 = mmap(fd);

  if(dir1==-1 && dir2 > 0 && dir3 >0)
    printf(1,"Permission mmap test OK\n");
  else 
    printf(1,"Permission mmap  test Failed\n");
}

void
permissionmunmaptest(void)
{
  int fd = open("test2.txt", O_CREATE | O_WRONLY);
  printf(fd, "a");
  close(fd);
  fd = open("test2.txt", O_RDONLY);
  char *map = (char*) mmap(fd);
  *map = 'A';
  munmap(map);
  char buf;
  read(fd, &buf, 1);
  if(buf == 'a')
    printf(1, "munmap don't update read-only files OK\n");
  else
    printf(1, "munmap update read-only files\n");
  
  close(fd);

  fd = open("test2.txt", O_RDWR);
  map = (char*) mmap(fd);
  *map = 'B';
  munmap(map);
  read(fd, &buf, 1);
  if(buf == 'B')
    printf(1, "munmap update writable files OK\n");
  else
    printf(1, "munmap don't update writable files\n");
   close(fd);
}

int
main(void)
{
  mkfile();
  permissionmmaptest();
  permissionmunmaptest();
  return 0;
}
