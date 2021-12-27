#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


// Tries to map a file with different access types
// The expected output are:
//  O_RDONLY ---> >= 0
//  O_RDWR -----> >= 0
//  O_WRONLY ---> -1
void
accesstypesmmaptest(void)
{

  int fd = open("test.txt", O_CREATE | O_WRONLY);
  int dir1 = mmap(fd);
  close(fd);
  fd = open("test.txt", O_RDONLY);
  int dir2 = mmap(fd);
  close(fd);
  fd = open("test.txt", O_RDWR);
  int dir3 = mmap(fd);
  close(fd);

  if(dir1 == -1 && dir2 > 0 && dir3 >0)
    printf(1,"Access types mmap test OK\n");
  else 
    printf(1,"Access types  mmap  test Failed\n");

  munmap((char*)dir2);
  munmap((char*)dir3);
}

// Tries to write the fmaps to the files
// Only should be able to write having
// the right access type
void
accesstypesmunmaptest(void)
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

// Sees if different mappings uses different addresses
// and map their correspondient file
void
multiplemappingstest(void)
{
  int fd = open("test3.txt", O_CREATE | O_RDWR);
  int fd2 = open("test4.txt", O_CREATE | O_RDWR);
  printf(fd, "1");
  printf(fd2, "2");
  char* map = (char*)mmap(fd);
  char* map2 = (char*)mmap(fd2);
  close(fd);
  close(fd2);
  if(map == map2)
    printf(1, "The 2 files map at the same address. Failure\n");
  else
    printf(1, "The 2 files map diferent adresses. OK\n");
  if(*map == '1')
    printf(1, "The first map maps the right file OK\n");
  else
    printf(1, "The first map maps a wrong file Fail\n");
  
  if(*map2 == '2')
    printf(1, "The second map maps the right file OK\n");
  else
    printf(1, "The second map maps a wrong file Fail\n");

  munmap(map);
  munmap(map2);
}

// Sees if the addresses of a fmap enter in conflict with 
// sbrk addresses
void
sbrkmmaptest(void)
{
  char *buf =sbrk(1);
  int fd = open("test.txt", O_CREATE | O_RDWR);
  printf(fd, "0");
  char *addr = (char*)mmap(fd);
  close(fd);
  char *buf1 = sbrk(1);
  if(addr == buf || addr == buf1)
    printf(1, "mmap's addr conflicts with sbrk's addr Fail\n");
  else
    printf(1, "mmap's addr doesn't conflict with sbrk's addr OK\n");

  munmap(addr);
  
}

// Checks if a file can be mapped multiple times
void
doublemmap(void)
{
  int fd = open("test.txt", O_CREATE | O_RDWR);
  char* map = (char*)mmap(fd);
  char* map2 = (char*)mmap(fd);
  close(fd);
  if(map >= 0 && map2 >= 0 && map != map2)
    printf(1, "No conflict OK\n");
  else
    printf(1, "Conflict Fail\n");
  munmap(map);
  munmap(map2);
}

// Checks if the fmaps can be inherited by
// a son process
void
dadandsonmessaging(void)
{
  int fd = open("message.txt", O_CREATE | O_RDWR);
  printf(fd, "Placeholder\0");
  char* map = (char*)mmap(fd);
  close(fd);
  int pid = fork();
  if(pid) {
    wait();
    char buf[12];
    strcpy(buf, map);
    if(strcmp(buf, "Message son\0") == 0)
      printf(1, "Dad and son test OK\n");
    else
      printf(1, "Dad and son test Failed\n");
    munmap(map);
  }
  else { 
    strcpy(map, "Message son\0");
    munmap(map);
    exit();
  }
}

// Checks if munmap frees the space for new mappings.
void
freefmaptest(void)
{
  int fd = open("limitfmaps.txt", O_CREATE | O_RDONLY);
  int addr[16];
  for(int i = 0; i < 16; i++)
    addr[i] = mmap(fd);

  int newaddr = mmap(fd);
  munmap((char*)addr[0]);
  addr[0] = mmap(fd);
  
  if(newaddr == -1 && addr[0] >= 0)
    printf(1, "nunmap frees the fmap table OK\n");
  else
    printf(1, "nunmap doesn't free the fmap table Fail\n");
  
  for(int i = 0; i < 16; i++)
    munmap((char*)addr[i]);
}

// Checks if a page-aligned mmap conflicts with
// the begin of another mmap 
void
onepagefile(void)
{
  int fd = open("onepage.txt", O_CREATE | O_RDWR);
  for(int i = 0; i< 4096; i++)
    printf(fd, "1");
  mmap(fd);
  close(fd);
  fd = open("otherfile.txt", O_CREATE | O_RDWR);
  printf(fd, "2");
  char *addr2 = (char*)mmap(fd);
  close(fd);
  if(*(addr2) == '2')
    printf(1, "The files dont interfere OK\n");
  else
    printf(1, "The files interfere with each other Fail\n");
}

int
main(void)
{
  accesstypesmmaptest();
  accesstypesmunmaptest();
  multiplemappingstest();
  sbrkmmaptest();
  doublemmap();
  dadandsonmessaging();
  freefmaptest();
  onepagefile();
  return 0;
}
